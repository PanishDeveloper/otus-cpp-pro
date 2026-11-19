#include <boost/program_options.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include "scanner.hpp"
#include "duplicate_finder.hpp"
#include "hasher.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("scan-dir", po::value<std::vector<std::string>>()->multitoken(), "dirs to scan")
        ("exclude,e", po::value<std::vector<std::string>>(), "dirs to exclude")
        ("depth,d", po::value<int>()->default_value(0), "scan depth")
        ("min-size,m", po::value<std::uintmax_t>()->default_value(1), "min file size")
        ("mask", po::value<std::vector<std::string>>(), "file masks")
        ("block-size,s", po::value<std::size_t>()->required(), "block size S")
        ("hash,H", po::value<std::string>()->default_value("crc32"), "hash: crc32 or md5");

    po::positional_options_description p;
    p.add("scan-dir", -1);

    po::variables_map vm;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return 0;
        }
        po::notify(vm);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    if (!vm.count("scan-dir"))
    {
        std::cerr << "Error: no scan directories\n";
        return 1;
    }

    int depth = vm["depth"].as<int>();
    if (depth < 0)
    {
        std::cerr << "Error: depth must be >= 0\n";
        return 1;
    }

    std::size_t block_size = vm["block-size"].as<std::size_t>();
    if (block_size == 0)
    {
        std::cerr << "Error: block size must be > 0\n";
        return 1;
    }

    std::unique_ptr<IHasher> hasher;
    std::string hash_name = vm["hash"].as<std::string>();
    if (hash_name == "crc32")
        hasher = std::make_unique<Crc32Hasher>();
    else if (hash_name == "md5")
        hasher = std::make_unique<Md5Hasher>();
    else
    {
        std::cerr << "Error: unknown hash " << hash_name << "\n";
        return 1;
    }

    ScanOptions opts;
    opts.scan_dirs = vm["scan-dir"].as<std::vector<std::string>>();
    opts.depth     = depth;
    opts.min_size  = vm["min-size"].as<std::uintmax_t>();
    if (vm.count("exclude"))
        opts.exclude_dirs = vm["exclude"].as<std::vector<std::string>>();
    if (vm.count("mask"))
        opts.masks = vm["mask"].as<std::vector<std::string>>();

    try
    {
        Scanner scanner(opts);
        auto files = scanner.scan();
        auto groups = DuplicateFinder::find(files, block_size, *hasher);

        bool first = true;
        for (const auto& group : groups)
        {
            if (!first) std::cout << "\n";
            first = false;
            for (const auto& path : group)
                std::cout << path.string() << "\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }

    return 0;
}