#pragma once
#include <filesystem>
#include <vector>
#include <atomic>
#include <cstdint>

namespace lazy
{
    struct JunkItem
    {
        std::filesystem::path path;
        std::uintmax_t size{};
    };

    std::vector<JunkItem> ScanFolder(const std::filesystem::path& root,
        const std::atomic<bool>& cancel);

    std::filesystem::path UserTempFolder();
}