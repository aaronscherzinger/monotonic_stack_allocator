#pragma once

#include <assert.h>
#include <cstddef>
#include <cstdint>

template<size_t N = 1024, size_t ALIGN = alignof(std::max_align_t)>
class MonotonicStackAllocator
{
static_assert(ALIGN > 0, "Alignment must be > 0");

public:
    MonotonicStackAllocator()
    : m_pos(m_memory + N)
    {
    }

    /// allocate memory
    void* Alloc(size_t size, size_t align = alignof(std::max_align_t))
    {
        // assert alignment is != 0 and power of two
        assert(size > 0 && align > 0);
        assert((align & (align - 1)) == 0);
        
        uintptr_t allocSize = static_cast<uintptr_t>(size);
        uintptr_t alignSize = static_cast<uintptr_t>(align);
        uintptr_t newPtr = reinterpret_cast<uintptr_t>(m_pos);
        // prevent underflow
        if (allocSize > newPtr)
        {
            return nullptr;
        }

        // take alignment into account
        newPtr = (newPtr - allocSize) & ~(alignSize - 1);

        // check capacity
        if (newPtr < reinterpret_cast<uintptr_t>(m_memory))
        {
            return nullptr;
        }

        m_pos = reinterpret_cast<char*>(newPtr);

        return reinterpret_cast<void*>(newPtr);
    }

    /// allocate memory for specific objects (does NOT perform construction)
    template<typename T>
    T* AllocObjects(size_t numInstances)
    {
        return reinterpret_cast<T*>(Alloc(numInstances * sizeof(T), alignof(T)));
    }

    bool Owns(void* ptr)
    {
        return (reinterpret_cast<uintptr_t>(ptr) >= reinterpret_cast<uintptr_t>(m_memory))
            && (reinterpret_cast<uintptr_t>(ptr) < reinterpret_cast<uintptr_t>(m_memory + N ));
    }

    void Free(void* /*ptr*/)
    {
        // do nothing - cannot free individual allocations
    }

    /// "free" all memory by resetting the stack pointer
    void FreeAll() { m_pos = m_memory + N; }

    size_t GetRemainingCapacity() const { return static_cast<size_t>(reinterpret_cast<uintptr_t>(m_pos) - reinterpret_cast<uintptr_t>(m_memory)); }

private:
    alignas(ALIGN) char m_memory[N];
    char* m_pos;
};
