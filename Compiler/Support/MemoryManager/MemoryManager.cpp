#include <cstddef>
#include <vector>
#include <cstdint>
#include <cassert>

namespace MemoryManager
{
        template<size_t block_size>
        class Arena
        {
                private:

                uintptr_t end;
                uintptr_t current;

                std::vector<std::byte*> blocks;

                uintptr_t align(uintptr_t ptr, size_t alignment)
                {
                        assert(alignment != 0 && "alignment should not be 0");
                        assert((alignment & (alignment - 1)) == 0 && "alignment must be power of 2");

                        return (ptr + alignment - 1) & ~(alignment - 1);
                }

                public: 

                Arena() 
                {
                        blocks.push_back
                        (
                        new std::byte[block_size]
                        );

                        current = reinterpret_cast<uintptr_t>(blocks.back());
                        end = current + block_size;
                }

                ~Arena()
                {
                        for (auto block : blocks) delete[] block;
                }

                std::byte* alloc(size_t size, size_t alignment)
                {
                        assert(size <= block_size && "request allocation size exceeds block size");

                        if(align(current, alignment) + size > end)
                        {
                                blocks.push_back
                                (
                                        new std::byte[block_size]
                                );
                                
                                current = reinterpret_cast<uintptr_t>(blocks.back());
                                end = current + block_size;

                                current = align(current, alignment);
                                std::byte* ptr = reinterpret_cast<std::byte*>(current);

                                current += size;

                                return ptr;
                        }

                        current = align(current, alignment);

                        std::byte* ptr = reinterpret_cast<std::byte*>(current);

                        current += size;

                        return ptr;
                }
        };
};