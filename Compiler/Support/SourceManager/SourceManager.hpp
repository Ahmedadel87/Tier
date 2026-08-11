#pragma once

#include <filesystem>
#include <iterator>
#include <cstdint>
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
        uint16_t length;
        FileID file_id;
    };

    SourceLocation after(SourceLocation location);
    SourceLocation before(SourceLocation location);

    struct FileEntry
    {
        FileID id;
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
            [[nodiscard("To access the file the id is required.")]] FileID add_file(const fs::path& path); 
            void update_file(FileEntry& new_file);
            const FileEntry& get_file(const FileID id) const;
            std::pair<uint32_t, uint32_t> get_line_column(const SourceLocation& location) const;
            std::string_view get_string(const SourceLocation& location) const;
            std::string_view get_line(FileID file_id, size_t line_number) const;
    };
}