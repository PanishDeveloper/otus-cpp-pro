#pragma once

#include "block_sink.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif

namespace bulk
{
    namespace net
    {
#ifdef _WIN32
        using socket_t = SOCKET;
        using length_t = int;
        constexpr socket_t invalidSocket = INVALID_SOCKET;
        inline void closeSocket(socket_t s) { ::closesocket(s); }
#else
        using socket_t = int;
        using length_t = std::size_t;
        constexpr socket_t invalidSocket = -1;
        inline void closeSocket(socket_t s) { ::close(s); }
#endif
#ifdef MSG_NOSIGNAL
        constexpr int sendFlags = MSG_NOSIGNAL; // Linux: не получать SIGPIPE
#else
        constexpr int sendFlags = 0;
#endif
    }

    class NetworkSink final : public BlockSink
    {
    public:
        NetworkSink(std::string host, std::uint16_t port)
            : m_host(std::move(host)), m_port(std::to_string(port)) {}

        ~NetworkSink() override { disconnect(); }

        NetworkSink(const NetworkSink&) = delete;
        NetworkSink& operator=(const NetworkSink&) = delete;

        void onBlock(const Block& block) override
        {
            const std::string payload = "bulk: " + joinCommands(block) + "\n";

            connectIfNeeded();
            if (sendAll(payload)) return;

            // соединение могло порваться - одна попытка переподключиться
            disconnect();
            connectIfNeeded();
            if (!sendAll(payload))
            {
                disconnect();
                throw std::runtime_error("NetworkSink: send failed");
            }
        }

    private:
        [[nodiscard]] bool sendAll(const std::string& data) const
        {
            std::size_t sent = 0;
            while (sent < data.size())
            {
                const auto n = ::send(m_socket, data.data() + sent,
                                      static_cast<net::length_t>(data.size() - sent), net::sendFlags);
                if (n <= 0) return false;
                sent += static_cast<std::size_t>(n);
            }
            return true;
        }

        void disconnect() noexcept
        {
            if (m_socket != net::invalidSocket)
            {
                net::closeSocket(m_socket);
                m_socket = net::invalidSocket;
            }
        }

        void connectIfNeeded()
        {
            if (m_socket != net::invalidSocket) return;

            addrinfo hints{};
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;

            addrinfo* raw = nullptr;
            if (::getaddrinfo(m_host.c_str(), m_port.c_str(), &hints, &raw) != 0)
                throw std::runtime_error("NetworkSink: cannot resolve " + m_host);
            std::unique_ptr<addrinfo, decltype(&::freeaddrinfo)> list(raw, &::freeaddrinfo);

            for (const addrinfo* ai = list.get(); ai != nullptr; ai = ai->ai_next)
            {
                const net::socket_t s = ::socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
                if (s == net::invalidSocket) continue;

#ifdef SO_NOSIGPIPE // macOS/BSD
                int on = 1;
                ::setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on));
#endif
                if (::connect(s, ai->ai_addr, static_cast<int>(ai->ai_addrlen)) == 0)
                {
                    m_socket = s;
                    return;
                }
                net::closeSocket(s);
            }
            throw std::runtime_error("NetworkSink: connect failed");
        }

        std::string m_host;
        std::string m_port;
        net::socket_t m_socket = net::invalidSocket;
    };
}
