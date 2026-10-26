#include <gtest/gtest.h>

#include "block.hpp"
#include "bulk_processor.hpp"
#include "clock.hpp"
#include "block_sink.hpp"

#include <cstddef>
#include <ctime>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace bulk;

namespace
{
    // Заглушка часов
    class FixedClock final : public Clock
    {
    public:
        explicit FixedClock(std::vector<std::time_t> times) : m_times(std::move(times)) {}
        std::time_t now() const override { return m_times[m_index++]; }
    private:
        std::vector<std::time_t> m_times;
        mutable std::size_t      m_index = 0;
    };

    // Сборщик блоков в память (вместо консоли/файла)
    class MockSink final : public BlockSink
    {
    public:
        void onBlock(const Block& block) override
        {
            blocks.emplace_back(block.commands().begin(), block.commands().end());
            startedAt.push_back(block.startedAt());
        }
        std::vector<std::vector<std::string>> blocks;
        std::vector<std::time_t>              startedAt;
    };

    // Сломанный sink (для проверки отказоустойчивости)
    class ThrowingSink final : public BlockSink
    {
    public:
        void onBlock(const Block&) override { throw std::runtime_error("boom"); }
    };

    // Хелпер для запуска
    std::vector<std::vector<std::string>> run(std::size_t n, const std::vector<std::string>& lines)
    {
        FixedClock clock(std::vector<std::time_t>(lines.size() + 1, 0));
        BulkProcessor proc(n, clock);
        MockSink sink;
        proc.addSink(sink);
        for (const auto& line : lines) proc.addLine(line);
        proc.finish();
        return sink.blocks;
    }
}

// 1. Базовый статический блок
TEST(BulkTest, StaticBlocks)
{
    auto r = run(3, {"cmd1", "cmd2", "cmd3", "cmd4", "cmd5"});
    ASSERT_EQ(r.size(), 2u);
    EXPECT_EQ(r[0], (std::vector<std::string>{"cmd1", "cmd2", "cmd3"}));
    EXPECT_EQ(r[1], (std::vector<std::string>{"cmd4", "cmd5"})); // EOF принудительно закрыл
}

// 2. Динамический блок закрывается скобками
TEST(BulkTest, DynamicBlocks)
{
    auto r = run(3, {"cmd1", "cmd2", "{", "cmd3", "cmd4", "}"});
    ASSERT_EQ(r.size(), 2u);
    EXPECT_EQ(r[0], (std::vector<std::string>{"cmd1", "cmd2"})); // { закрыла статику
    EXPECT_EQ(r[1], (std::vector<std::string>{"cmd3", "cmd4"})); // } закрыла динамику
}

// 3. Вложенные скобки игнорируются
TEST(BulkTest, NestedBracesIgnored)
{
    auto r = run(3, {"{", "cmd5", "{", "cmd7", "}", "cmd9", "}"});
    ASSERT_EQ(r.size(), 1u);
    EXPECT_EQ(r[0], (std::vector<std::string>{"cmd5", "cmd7", "cmd9"}));
}

// 4. Блок отбрасывается
TEST(BulkTest, EofDropsDynamicBlock)
{
    auto r = run(3, {"{", "cmd10", "cmd11"}); // нет закрывающей }
    EXPECT_TRUE(r.empty());
}

// 5. Пример из ТЗ
TEST(BulkTest, FullExampleFromTask)
{
    auto r = run(3, {
        "cmd1", "cmd2", "{", "cmd3", "cmd4", "}",
        "{", "cmd5", "cmd6", "{", "cmd7", "cmd8", "}", "cmd9", "}",
        "{", "cmd10", "cmd11"
    });

    ASSERT_EQ(r.size(), 3u);
    EXPECT_EQ(r[0], (std::vector<std::string>{"cmd1", "cmd2"}));
    EXPECT_EQ(r[1], (std::vector<std::string>{"cmd3", "cmd4"}));
    EXPECT_EQ(r[2], (std::vector<std::string>{"cmd5", "cmd6", "cmd7", "cmd8", "cmd9"}));
}

// 6. Время берется снаружи (для имен файлов)
TEST(BulkTest, StartedAtIsTimeOfFirstCommand)
{
    FixedClock clock(std::vector<std::time_t>{100, 101});
    BulkProcessor proc(2, clock);
    MockSink sink;
    proc.addSink(sink);

    proc.addLine("a");
    proc.addLine("b"); // блок 1 закрылся, время 100
    proc.addLine("c");
    proc.finish();     // блок 2 закрылся, время 101

    ASSERT_EQ(sink.startedAt.size(), 2u);
    EXPECT_EQ(sink.startedAt[0], 100);
    EXPECT_EQ(sink.startedAt[1], 101);
}

// 7. Один упавший sink не роняет остальные
TEST(BulkTest, BrokenSinkDoesNotKillOthers)
{
    FixedClock clock(std::vector<std::time_t>{100});
    BulkProcessor proc(2, clock);

    ThrowingSink bad;
    MockSink     good;
    proc.addSink(bad);
    proc.addSink(good);

    proc.addLine("cmd1");
    proc.addLine("cmd2");
    proc.finish();

    EXPECT_EQ(proc.failures(), 1u);
    ASSERT_EQ(good.blocks.size(), 1u); // хороший sink всё получил
}

// 8. N = 1: каждая команда - свой блок
TEST(BulkTest, BlockSizeOne)
{
    auto r = run(1, {"a", "b"});
    ASSERT_EQ(r.size(), 2u);
}

// 9. Пустой динамический блок ничего не выводит
TEST(BulkTest, EmptyDynamicBlock)
{
    EXPECT_TRUE(run(3, {"{", "}"}).empty());
}

// 10. Лишняя } на нулевой глубине игнорируется
TEST(BulkTest, ExtraClosingBraceIgnored)
{
    auto r = run(2, {"}", "a", "b"});
    ASSERT_EQ(r.size(), 1u);
    EXPECT_EQ(r[0], (std::vector<std::string>{"a", "b"}));
}

// 11. { посреди незаполненной статики закрывает её досрочно
TEST(BulkTest, OpenBraceFlushesPartialStatic)
{
    auto r = run(3, {"a", "{", "b", "}"});
    ASSERT_EQ(r.size(), 2u);
    EXPECT_EQ(r[0], (std::vector<std::string>{"a"}));
    EXPECT_EQ(r[1], (std::vector<std::string>{"b"}));
}
