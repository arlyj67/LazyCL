#include "Scanner.h"
#include <windows.h>

namespace fs = std::filesystem;

namespace lazy
{
    fs::path UserTempFolder()
    {
        wchar_t buf[MAX_PATH + 1]{};
        DWORD n = GetTempPathW(MAX_PATH, buf);
        return fs::path(std::wstring(buf, n));
    }

    std::vector<JunkItem> ScanFolder(const fs::path& root,
        const std::atomic<bool>& cancel)
    {
        std::vector<JunkItem> items;
        std::error_code ec;

        fs::recursive_directory_iterator it(
            root, fs::directory_options::skip_permission_denied, ec);
        fs::recursive_directory_iterator end;

        for (; !ec && it != end && !cancel; it.increment(ec))
        {
            std::error_code e2;
            if (it->is_symlink(e2))
            {
                it.disable_recursion_pending();
                continue;
            }
            if (it->is_regular_file(e2))
            {
                auto size = it->file_size(e2);
                if (!e2) items.push_back({ it->path(), size });
            }
        }
        return items;
    }
}