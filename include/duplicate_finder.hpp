#pragma once

#include <map>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <exception>
#include <iostream>

#include "comparator.hpp"

class DuplicateFinder
{
public:
    static std::vector<std::vector<fs::path>> find(
        std::vector<FileInfo>& files, std::size_t block_size, const IHasher& hasher)
    {
        std::map<std::uintmax_t, std::vector<FileInfo>> by_size;
        for (auto& f : files)
            by_size[f.size].push_back(std::move(f));

        std::vector<std::vector<fs::path>> result;

        for (auto& [size, group] : by_size)
        {
            if (group.size() < 2) continue;

            std::vector<std::vector<std::size_t>> matches;
            for (std::size_t i = 0; i < group.size(); ++i)
            {
                bool added = false;
                for (auto& match : matches)
                {
                    bool same = false;
                    try
                    {
                        same = FileComparator::compare(group[match[0]], group[i], block_size, hasher);
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "warning: cannot compare "
                                  << group[match[0]].path << " and " << group[i].path
                                  << ": " << e.what() << "\n";
                    }

                    if (same)
                    {
                        match.push_back(i);
                        added = true;
                        break;
                    }
                }
                if (!added) matches.push_back({i});  // число групп неизвестно
            }

            for (const auto& match : matches)
            {
                if (match.size() < 2) continue;

                std::vector<fs::path> paths;
                paths.reserve(match.size());
                for (auto idx : match)
                    paths.push_back(group[idx].path);

                std::sort(paths.begin(), paths.end());
                result.push_back(std::move(paths));
            }
        }
        return result;
    }
};