#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

#include "hasher.hpp"
#include "file_info.hpp"
#include "comparator.hpp"
#include "scanner.hpp"
#include "duplicate_finder.hpp"

namespace
{
    class BayanTest : public ::testing::Test
    {
    protected:
        fs::path temp_dir;

        void SetUp() override
        {
            temp_dir = fs::temp_directory_path()
           / ("bayan_test_" + std::to_string(
                  std::chrono::steady_clock::now().time_since_epoch().count()));

            fs::create_directories(temp_dir);
        }

        void TearDown() override
        {
            fs::remove_all(temp_dir);
        }

        void createFile(const std::string& name, const std::string& content) const
        {
            std::ofstream out((temp_dir / name).string(), std::ios::binary);
            out << content;
        }
    };
}

TEST(HasherTest, Crc32)
{
    Crc32Hasher hasher;
    constexpr std::uint32_t expected = 0xf7d18982u;
    EXPECT_EQ(hasher.hash("Hello"),
              std::string(reinterpret_cast<const char*>(&expected), sizeof(expected)));
}

TEST(HashTest, Md5Length)
{
    Md5Hasher hasher;
    EXPECT_EQ(hasher.hash("Hello").size(), 16u);
}

TEST_F(BayanTest, FileInfoLazyLoading)
{
    createFile("test.txt", "Hello, World!");
    FileInfo fi(temp_dir / "test.txt", 13);
    Crc32Hasher hasher;

    EXPECT_FALSE(fi.isBlockHashed(0));
    fi.getBlockHash(0, 5, hasher);
    EXPECT_TRUE(fi.isBlockHashed(0));
    EXPECT_FALSE(fi.isBlockHashed(1));
}

TEST_F(BayanTest, ZeroPadding)
{
    createFile("test.txt", "Hello, Worl");
    FileInfo fi(temp_dir / "test.txt", 11);
    Crc32Hasher hasher;

    EXPECT_EQ(fi.getBlockHash(2, 5, hasher),
              hasher.hash(std::string("l\0\0\0\0", 5)));
}

TEST_F(BayanTest, Comparator)
{
    createFile("a.txt", "Hello, World!");
    createFile("b.txt", "Hello, World!");
    createFile("c.txt", "Hello, C++!!!");

    FileInfo fa(temp_dir / "a.txt", 13);
    FileInfo fb(temp_dir / "b.txt", 13);
    FileInfo fc(temp_dir / "c.txt", 13);
    Crc32Hasher hasher;

    EXPECT_TRUE(FileComparator::compare(fa, fb, 5, hasher));
    EXPECT_FALSE(FileComparator::compare(fa, fc, 5, hasher));
}

TEST_F(BayanTest, DuplicateFinder)
{
    createFile("a.txt", "Hello, World!");
    createFile("b.txt", "Hello, World!");
    createFile("c.txt", "Hello, C++!!!");

    std::vector<FileInfo> files;
    files.emplace_back(temp_dir / "a.txt", 13);
    files.emplace_back(temp_dir / "b.txt", 13);
    files.emplace_back(temp_dir / "c.txt", 13);

    Crc32Hasher hasher;
    auto groups = DuplicateFinder::find(files, 5, hasher);

    ASSERT_EQ(groups.size(), 1u);
    ASSERT_EQ(groups[0].size(), 2u);
}

TEST_F(BayanTest, ScannerFilters)
{
    createFile("a.txt", "data");
    createFile("b.dat", "data");
    fs::create_directories(temp_dir / "subdir");
    createFile("subdir/c.txt", "data");

    ScanOptions opts;
    opts.scan_dirs.push_back(temp_dir.string());
    opts.masks = {"*.txt"};
    opts.min_size = 0;
    opts.depth = 0;

    Scanner scanner(opts);
    auto files = scanner.scan();

    ASSERT_EQ(files.size(), 1u);
    EXPECT_EQ(files[0].path.filename().string(), "a.txt");
}