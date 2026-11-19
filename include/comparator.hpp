#pragma once

#include "file_info.hpp"

class FileComparator
{
public:
    static bool compare(FileInfo& a, FileInfo& b, std::size_t block_size, const IHasher& hasher)
    {
        if (a.size != b.size)
            return false;

        const std::size_t num_blocks = a.blockCount(block_size);
        for (std::size_t i = 0; i < num_blocks; ++i)
        {
            if (a.getBlockHash(i, block_size, hasher) != b.getBlockHash(i, block_size, hasher))
                return false;
        }
        return true;
    }
};
