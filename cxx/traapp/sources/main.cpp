#include <traaxx/depth.hpp>
#include <traaxx/traaxx.hpp>

#include <nlohmann/json.hpp>

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <sys/resource.h>

namespace
{
    using json = nlohmann::json;

    struct CliError
    {
        std::string code;
        std::string message;
    };

    struct Args
    {
        std::string impl;
        std::string tree_path;
        std::string case_path;
        std::uint64_t warmup = 3;
        std::uint64_t repeat = 20;
    };

    Args parseArgs(int argc, char** argv)
    {
        Args args;
        for (int i = 1; i < argc; ++i)
        {
            auto const flag = std::string{argv[i]};
            auto const next = [&]() -> std::string
            {
                if (i + 1 >= argc)
                {
                    throw CliError{"args_mismatch", "missing value for " + flag};
                }
                return argv[++i];
            };
            if (flag == "--impl")
            {
                args.impl = next();
            }
            else if (flag == "--tree")
            {
                args.tree_path = next();
            }
            else if (flag == "--case")
            {
                args.case_path = next();
            }
            else if (flag == "--warmup")
            {
                args.warmup = std::stoull(next());
            }
            else if (flag == "--repeat")
            {
                args.repeat = std::stoull(next());
            }
            else
            {
                throw CliError{"args_mismatch", "unknown flag " + flag};
            }
        }
        if (args.impl.empty() || args.tree_path.empty() || args.case_path.empty())
        {
            throw CliError{"args_mismatch", "--impl, --tree and --case are required"};
        }
        if (args.impl != "std")
        {
            throw CliError{"args_mismatch", "--impl " + args.impl + " is not built into this binary yet"};
        }
        return args;
    }

    json readJsonFile(const std::string& path, const std::string& errorCode)
    {
        auto file = std::ifstream{path};
        if (!file)
        {
            throw CliError{"file_not_found", "cannot open " + path};
        }
        try
        {
            auto parsed = json{};
            file >> parsed;
            return parsed;
        }
        catch (const json::parse_error& e)
        {
            throw CliError{errorCode, e.what()};
        }
    }

    std::int64_t peakRssBytes()
    {
        struct rusage usage
        {
        };
        getrusage(RUSAGE_SELF, &usage);
        return static_cast<std::int64_t>(usage.ru_maxrss) * 1024;
    }

    traaxx::Tree<std::uint32_t> loadUint32Tree(const json& corpus)
    {
        auto parent = corpus.at("parent").get<std::vector<std::uint32_t>>();
        auto sibling = corpus.at("sibling").get<std::vector<std::uint32_t>>();
        auto data = corpus.at("data").get<std::vector<std::uint32_t>>();
        return traaxx::Tree<std::uint32_t>{std::move(parent), std::move(sibling), std::move(data)};
    }
}

int main(int argc, char** argv)
{
    try
    {
        auto const args = parseArgs(argc, argv);
        auto const corpus = readJsonFile(args.tree_path, "invalid_tree_json");
        auto const caseJson = readJsonFile(args.case_path, "invalid_case_json");
        auto const payloadKind = corpus.at("payload_type").at("kind").get<std::string>();
        auto const payloadValue = corpus.at("payload_type").at("value").get<std::string>();
        if (payloadKind != "primitive" || payloadValue != "uint32")
        {
            throw CliError{"invalid_tree_json", "unsupported payload_type"};
        }
        auto const tree = loadUint32Tree(corpus);
        auto const op = caseJson.at("op").get<std::string>();
        if (op != "depth")
        {
            throw CliError{"unknown_op", "unsupported op: " + op};
        }
        auto const rssBefore = peakRssBytes();
        for (std::uint64_t i = 0; i < args.warmup; ++i)
        {
            static_cast<void>(traaxx::depth(tree.parent(), static_cast<std::uint32_t>(tree.parent().size())));
        }
        auto timings = std::vector<std::int64_t>{};
        timings.reserve(args.repeat);
        auto result = traaxx::DepthResult<std::uint32_t>{};
        for (std::uint64_t i = 0; i < args.repeat; ++i)
        {
            auto const start = std::chrono::steady_clock::now();
            result = traaxx::depth(tree.parent(), static_cast<std::uint32_t>(tree.parent().size()));
            auto const end = std::chrono::steady_clock::now();
            timings.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        }
        auto const rssAfter = peakRssBytes();
        auto output = json{};
        output["result"] = result.depth_vector;
        output["timing_ns"] = timings;
        output["iterations"] = result.iterations;
        output["peak_rss_delta_bytes"] = rssAfter - rssBefore;
        std::cout << output.dump() << "\n";
        return 0;
    }
    catch (const CliError& error)
    {
        auto err = json{};
        err["error"] = error.code;
        err["message"] = error.message;
        std::cerr << err.dump() << "\n";
        return 1;
    }
    catch (const std::exception& e)
    {
        auto err = json{};
        err["error"] = "invalid_case_json";
        err["message"] = e.what();
        std::cerr << err.dump() << "\n";
        return 1;
    }
}
