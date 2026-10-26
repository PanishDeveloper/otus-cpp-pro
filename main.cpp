#include "bulk_processor.hpp"
#include "clock.hpp"
#include "console_sink.hpp"
#include "file_sink.hpp"
#include "network_sink.hpp"

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>

#ifdef _WIN32
namespace
{
    class WinsockGuard
    {
    public:
        WinsockGuard()
        {
            WSADATA wsa{};
            if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
                throw std::runtime_error("WSAStartup failed");
        }
        ~WinsockGuard() { WSACleanup(); }

        WinsockGuard(const WinsockGuard&) = delete;
        WinsockGuard& operator=(const WinsockGuard&) = delete;
    };
}
#endif

namespace
{
    std::optional<std::uint64_t> parseUnsigned(const std::string& s, std::uint64_t min, std::uint64_t max)
    {
        if (s.empty()) return std::nullopt;

        for (unsigned char c : s)
        {
            if (!std::isdigit(c)) return std::nullopt;
        }

        try
        {
            const auto out = std::stoull(s);
            if (out >= min && out <= max)
                return out;
        }
        catch (...)
        {
            return std::nullopt;
        }
        return std::nullopt;
    }

    void processInput(bulk::BulkProcessor& processor)
    {
        for (std::string line; std::getline(std::cin, line);)
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back(); // CRLF

            processor.addLine(line);
        }
        processor.finish();
    }
}

int main(int argc, char** argv)
{
    if (argc != 2 && argc != 4)
    {
        std::cerr << "Usage: bulk <N> [<host> <port>]\n";
        return 1;
    }

    const auto n = parseUnsigned(argv[1], 1, std::numeric_limits<std::size_t>::max());
    if (!n)
    {
        std::cerr << "N must be a positive integer\n";
        return 1;
    }

    std::optional<std::uint64_t> port;
    if (argc == 4)
    {
        port = parseUnsigned(argv[3], 1, 65535);
        if (!port)
        {
            std::cerr << "port must be an integer in range 1..65535\n";
            return 1;
        }
    }

    try
    {
#ifdef _WIN32
        WinsockGuard winsock;
#endif
        bulk::SystemClock clock;
        bulk::BulkProcessor processor(static_cast<std::size_t>(*n), clock);
        bulk::ConsoleSink console;
        bulk::FileSink file;

        processor.addSink(console);
        processor.addSink(file);

        if (argc == 4)
        {
            bulk::NetworkSink network(argv[2], static_cast<std::uint16_t>(*port));
            processor.addSink(network);
            processInput(processor);
        }
        else
        {
            processInput(processor);
        }

        return processor.failures() == 0 ? 0 : 2;
    }
    catch (const std::exception& e)
    {
        std::cerr << "fatal: " << e.what() << "\n";
        return 3;
    }
}