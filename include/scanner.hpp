#pragma once

#include <filesystem>
#include <system_error>
#include <boost/regex.hpp>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <cstdint>

#include "file_info.hpp"

namespace fs = std::filesystem;

struct ScanOptions
{
    std::vector<std::string> scan_dirs;
    std::vector<std::string> exclude_dirs;
    std::vector<std::string> masks;
    int depth = 0;
    std::uintmax_t min_size = 1;
};

class Scanner
{
public:
    explicit Scanner(ScanOptions opts) : m_opts(std::move(opts)) {}

    [[nodiscard]] std::vector<FileInfo> scan() const
    {
        std::vector<FileInfo> result;
        std::set<fs::path> processed;
        std::vector<fs::path> excludes = resolveExcludes();
        std::vector<boost::regex> regexes = buildMasks();

        for (const auto& dir_str : m_opts.scan_dirs)
        {
            fs::path root(dir_str);
            {
                std::error_code chk;
                if (!fs::is_directory(root, chk) || chk) continue;
            }

            std::error_code ec;
            fs::path canon_root = fs::canonical(root, ec);
            if (ec) continue;

            if (isExcluded(canon_root, excludes)) continue;

            auto add_file = [&](const fs::directory_entry& entry)
            {
                std::error_code file_ec;
                fs::path canon = fs::canonical(entry.path(), file_ec);
                if (file_ec) return;

                if (processed.count(canon)) return;
                processed.insert(canon);

                if (!matchesMask(entry.path().filename().string(), regexes)) return;

                std::error_code size_ec;
                const auto size = fs::file_size(entry.path(), size_ec);
                if (size_ec) return;

                if (size <= m_opts.min_size) return;

                result.emplace_back(entry.path(), size);
            };

            if (m_opts.depth == 0)
            {
                std::error_code iter_ec;
                for (fs::directory_iterator it(canon_root, iter_ec), end;
                     it != end && !iter_ec;
                     it.increment(iter_ec))
                {
                    std::error_code type_ec;
                    if (it->is_regular_file(type_ec) && !type_ec) add_file(*it);
                }
            }
            else
            {
                std::error_code iter_ec;
                for (fs::recursive_directory_iterator it(canon_root, fs::directory_options::skip_permission_denied, iter_ec), end;
                                                         it != end && !iter_ec; it.increment(iter_ec))
                {
                    std::error_code type_ec;
                    if (it->is_directory(type_ec) && !type_ec)
                    {
                        if (isExcluded(it->path(), excludes)) it.disable_recursion_pending();
                        if (it.depth() >= m_opts.depth) it.disable_recursion_pending();
                    }
                    else if (it->is_regular_file(type_ec) && !type_ec)
                    {
                        add_file(*it);
                    }
                }
            }
        }
        return result;
    }

private:
    [[nodiscard]] std::vector<fs::path> resolveExcludes() const
    {
        std::vector<fs::path> res;
        res.reserve(m_opts.exclude_dirs.size());
        for (const auto& ex : m_opts.exclude_dirs)
        {
            std::error_code ec;
            if (fs::exists(ex, ec) && !ec && fs::is_directory(ex, ec) && !ec)
            {
                fs::path canon = fs::canonical(ex, ec);
                if (!ec) res.push_back(std::move(canon));
            }
        }
        return res;
    }

    [[nodiscard]] std::vector<boost::regex> buildMasks() const
    {
        std::vector<boost::regex> res;
        res.reserve(m_opts.masks.size());

        static const std::string regex_meta = "\\^$.|+()[]{}";

        for (const auto& mask : m_opts.masks)
        {
            std::string re;
            re.reserve(mask.size() * 2);
            for (char c : mask)
            {
                if (c == '*') re += ".*";
                else if (c == '?') re += '.';
                else
                {
                    if (regex_meta.find(c) != std::string::npos) re += '\\';
                    re += c;
                }
            }
            res.emplace_back(re, boost::regex::icase);
        }
        return res;
    }

    static bool matchesMask(const std::string& filename,
                            const std::vector<boost::regex>& regexes)
    {
        if (regexes.empty()) return true;
        return std::any_of(regexes.begin(), regexes.end(),
            [&filename](const boost::regex& re) {
                return boost::regex_match(filename, re);
            });
    }

    static bool isExcluded(const fs::path& p, const std::vector<fs::path>& excludes)
    {
        std::error_code ec;
        fs::path canon = fs::canonical(p, ec);
        if (ec) return false;

        return std::any_of(excludes.begin(), excludes.end(),
            [&canon](const fs::path& ex) {
                auto mismatch = std::mismatch(ex.begin(), ex.end(),
                                              canon.begin(), canon.end());
                return mismatch.first == ex.end();
            });
    }

    ScanOptions m_opts;
};