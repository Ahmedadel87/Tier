#pragma once

#include <cstddef>
#include <vector>
#include <cstdint>
#include <cassert>
#include <utility>
#include <type_traits>
#include <new>

namespace Mem
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

			struct Destructor {
				void* object;
				void (*destroy)(void*);
			};

			std::vector<Destructor> destructors;

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
				for (auto it = destructors.rbegin(); it != destructors.rend(); ++it)
					it->destroy(it->object);

				for (auto block : blocks)
					delete[] block;
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

			template<typename T, typename... Args>
			T* make(Args&&... args)
			{
				void* memory = alloc(sizeof(T), alignof(T));

				T* object = new (memory) T(std::forward<Args>(args)...);

				if constexpr (!std::is_trivially_destructible_v<T>)
				{
					destructors.push_back({
						object,
						[](void* ptr) {
							static_cast<T*>(ptr)->~T();
						}
					});
				}

				return object;
			}
	};
};