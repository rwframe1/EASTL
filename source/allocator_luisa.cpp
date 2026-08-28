//
// Created by Mike Smith on 2021/12/24.
//

#include <EASTL/allocator.h>
#include <EASTL/internal/config.h>
#include <mimalloc.h>
namespace eastl
{
	allocator::allocator(const char* EASTL_NAME(pName))
	{
#if EASTL_NAME_ENABLED
		mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
	}

	allocator::allocator(const allocator& EASTL_NAME(alloc))
	{
#if EASTL_NAME_ENABLED
		mpName = alloc.mpName;
#endif
	}

	allocator::allocator(const allocator&, const char* EASTL_NAME(pName))
	{
#if EASTL_NAME_ENABLED
		mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
	}

	allocator& allocator::operator=(const allocator& EASTL_NAME(alloc))
	{
#if EASTL_NAME_ENABLED
		mpName = alloc.mpName;
#endif
		return *this;
	}

	namespace detail
	{
		inline static allocator*& GetDefaultAllocatorRef() noexcept
		{
			static allocator a;
			static allocator* pa = &a;
			return pa;
		}
	} // namespace detail
	EASTL_API allocator* GetDefaultAllocator() { return detail::GetDefaultAllocatorRef(); }

	EASTL_API allocator* SetDefaultAllocator(allocator* pAllocator)
	{
		allocator* const pPrevAllocator = GetDefaultAllocator();
		detail::GetDefaultAllocatorRef() = pAllocator;
		return pPrevAllocator;
	}

	bool operator==(const allocator&, const allocator&)
	{
		return true;
	}

#if !defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	bool operator!=(const allocator&, const allocator&)
	{
		return false;
	}
#endif

	void* allocator::reallocate(void* originPtr, size_t n)
	{
		return mi_realloc(originPtr, n);
	}

	void* allocator::allocate(size_t n, int /* flags */)
	{
		return mi_malloc(n);
	}


	void* allocator::allocate(size_t n, size_t alignment, size_t offset [[maybe_unused]], int flags)
	{
		if (alignment == 0u)
			return nullptr;

		if (alignment <= EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT)
		{
			return (offset % alignment) == 0u ? allocate(n, flags) : nullptr;
		}
		return (offset % alignment) == 0u ? mi_malloc_aligned(n, alignment) : nullptr;
	}


	void allocator::deallocate(void* p, size_t)
	{
		mi_free(p);
	}

} // namespace eastl