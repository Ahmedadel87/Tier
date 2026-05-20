#pragma once

#include <filesystem>
#include <iterator>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace SourceManager 
{
    std::string read_file(const fs::path& path);
    
    using FileID = uint32_t;

    struct SourceLocation
    {
        uint32_t offset;
        FileID file_id;
    };

    struct FileEntry
    {
        fs::path path;
        std::string contents;
        std::vector<uint32_t> line_offsets = {0};
    };

    class SourceManager
    {
        private:
            std::unordered_map<FileID, FileEntry> id_to_file;
            FileID nextID;

        public:
            SourceManager();
            FileID add_file(const fs::path& path); 
            const FileEntry& get_file(const FileID& id) const;
            const std::pair<uint32_t, uint32_t> get_line_column(const SourceLocation location);
    };
}