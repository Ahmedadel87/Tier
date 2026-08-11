#include "SourceManager.hpp"
#include <algorithm>
#include <iostream>

namespace SourceManager
{
    SourceLocation after(SourceLocation location)
    {
        return {.offset=location.offset+location.length, .length=0, .file_id=location.file_id};
    }

    SourceLocation before(SourceLocation location)
    {
        return {.offset=location.offset-1, .length=0, .file_id=location.file_id};
    }

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
        entry.id = nextID;
        entry.path = path;
        entry.contents = read_file(path);
        entry.contents += ' ';
        entry.line_offsets = std::vector<uint32_t>{0};

        for (size_t i = 0; i < entry.contents.size(); i++)
        {
            char c = entry.contents[i];

            if (c == '\n')
            {
                entry.line_offsets.push_back(i + 1);
            }
        }
        
        entry.line_offsets.push_back(entry.contents.size()+1); //* WARNING, IF YOU REMOVE THE +1 THE ENTIRE SOURCE MANAGER BREAKS

        id_to_file[nextID] = entry;
        return nextID++;
    }

    void SourceManager::update_file(FileEntry& new_file)
    {
        id_to_file[new_file.id] = new_file;
    }

    const FileEntry& SourceManager::get_file(const FileID id) const
    {
        return id_to_file.at(id);
    }   

    std::pair<uint32_t, uint32_t> SourceManager::get_line_column(const SourceLocation& location) const
    {
        const FileEntry& file = get_file(location.file_id);
        auto it = std::upper_bound(file.line_offsets.begin(), file.line_offsets.end(), location.offset);
        return std::pair
        {
            static_cast<uint32_t>((it - file.line_offsets.begin()) - 1), // line number
            location.offset - *std::prev(it) // column number
        };
    }

    std::string_view SourceManager::get_string(const SourceLocation& location) const
    {
        const FileEntry& file = get_file(location.file_id);
        
        return std::string_view(
            file.contents.data() + location.offset,
            location.length
        );
    }

    std::string_view SourceManager::get_line(FileID file_id, size_t line_number) const
    {
        const FileEntry& file = get_file(file_id);
        
        return std::string_view(
            file.contents.data() + file.line_offsets[line_number],
            (file.line_offsets[line_number + 1] - 1) - file.line_offsets[line_number]
        );
    }
}