#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <cstddef>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>

class IHasher
{
public:
    virtual ~IHasher() = default;
    [[nodiscard]] virtual std::string hash(const std::string& data) const = 0;
};


class Crc32Hasher final : public IHasher
{
public:
    [[nodiscard]] std::string hash(const std::string& data) const override
    {
        boost::crc_32_type result;

        result.process_bytes(data.data(), data.size());
        std::ostringstream ss;
        ss << std::hex << std::setw(8) << std::setfill('0') << result.checksum();
        return ss.str();
    }
};

class Md5Hasher final : public IHasher
{
public:
    [[nodiscard]] std::string hash(const std::string& data) const override
    {
        boost::uuids::detail::md5 md5;

        md5.process_bytes(data.data(), data.size());

        boost::uuids::detail::md5::digest_type digest;
        md5.get_digest(digest);

        const auto* bytes = reinterpret_cast<const unsigned char*>(&digest);
        std::ostringstream ss;
        for (std::size_t i = 0; i < sizeof(digest); ++i)
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(bytes[i]);

        return ss.str();
    }
};