#pragma once

#include <string>
#include <cstddef>
#include <cstdint>
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

        const std::uint32_t crc = result.checksum();
        return std::string(reinterpret_cast<const char*>(&crc), sizeof(crc));
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

        return std::string(reinterpret_cast<const char*>(&digest), sizeof(digest));
    }
};