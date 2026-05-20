#include "SourceManager.hpp"
#include <algorithm>

namespace SourceManager
{
    const bool valid_path(const fs::path& path)
    {
        if(fs::exists(path) && !path.empty())
        {
            return true;
        }
        return false;
    }

    std::string read_file(const fs::path& path) 
    {
        if(!valid_path(path)) throw std::runtime_error("Invalid path.");

        std::ifstream file(path, std::ios::binary);

        return std::string
        (
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
    }

    SourceManager::SourceManager() : nextID(0) { }

    FileID SourceManager::add_file(const fs::path& path)
    {
        if(!valid_path(path)) throw std::runtime_error("Invalid path.");

        FileEntry entry;
        entry.path = path;
        entry.contents = read_file(path);
        entry.line_offsets = std::vector<uint32_t>{0};
        
        id_to_file[nextID] = entry;
        return nextID++;
    }

    const FileEntry& SourceManager::get_file(const FileID& id) const
    {
        return id_to_file.at(id);
    }

    const std::pair<uint32_t, uint32_t> SourceManager::get_line_column(const SourceLocation location) 
    {
        const FileEntry& file = get_file(location.file_id);
        auto it = std::upper_bound(file.line_offsets.begin(), file.line_offsets.end(), location.offset);
        return std::pair
        {
            static_cast<uint32_t>((it - file.line_offsets.begin()) - 1), // line number
            location.offset - *std::prev(it) // column number
        };
    }

}