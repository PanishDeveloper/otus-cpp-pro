#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <cassert>

#include "hasher.hpp"

namespace fs = std::filesystem;

class FileInfo
{
public:
    FileInfo(fs::path p, std::uintmax_t s) : path(std::move(p)), size(s) {}

    std::string getBlockHash(std::size_t block_idx, std::size_t block_size, const IHasher& hasher)
    {
        assert(block_size > 0);

        if (block_idx >= blockCount(block_size))
            throw std::out_of_range("block_idx out of range: " + std::to_string(block_idx));

        if (block_idx >= m_block_hashes.size())
            m_block_hashes.resize(block_idx + 1);

        if (m_block_hashes[block_idx].empty())
        {
            std::ifstream file(path.string(), std::ios::binary);
            if (!file)
                throw std::runtime_error("Cannot open: " + path.string());

            const auto offset = static_cast<std::streamoff>(block_idx)
                              * static_cast<std::streamoff>(block_size);
            file.seekg(offset, std::ios::beg);

            std::string data(block_size, '\0');

            std::size_t to_read = block_size;
            if (static_cast<std::uintmax_t>(block_idx * block_size + block_size) > size)
                to_read = static_cast<std::size_t>(size - block_idx * block_size);

            file.read(data.data(), static_cast<std::streamsize>(to_read));

            if (static_cast<std::size_t>(file.gcount()) != to_read)
                throw std::runtime_error("Short read: " + path.string());

            m_block_hashes[block_idx] = hasher.hash(data);
        }

        return m_block_hashes[block_idx];
    }

    [[nodiscard]] std::size_t blockCount(std::size_t block_size) const
    {
        assert(block_size > 0);
        return (size + block_size - 1) / block_size;
    }

    [[nodiscard]] bool isBlockHashed(std::size_t block_idx) const
    {
        return block_idx < m_block_hashes.size() && !m_block_hashes[block_idx].empty();
    }

    fs::path path;
    std::uintmax_t size;

private:
    std::vector<std::string> m_block_hashes;
};