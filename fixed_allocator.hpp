#pragma once

#include <cstddef>
#include <new>
#include <memory>

template <typename T, size_t N>
class FixedAllocator
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using  reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind
    {
        using other = FixedAllocator<U, N>;
    };

    FixedAllocator() : m_start(nullptr), m_next(nullptr) ,m_end(nullptr)
    {
        m_start = static_cast<char*>(::operator new(N * sizeof(T)));
        m_next = m_start;
        m_end = m_start + N * sizeof(T);
    }

    FixedAllocator(const FixedAllocator&) noexcept : FixedAllocator() {}

    template <typename U>
    FixedAllocator(const FixedAllocator<U, N>&) noexcept : FixedAllocator() {}

    ~FixedAllocator()
    {
        ::operator delete(m_start);
    }

    pointer allocate(std::size_t n, const void* /*hint*/ = nullptr)
    {
        if (n == 0) return nullptr;
        if (n > N) throw std::bad_alloc();
        auto remaining = static_cast<size_t>(m_end - m_next) / sizeof(T);
        if (remaining < n) throw std::bad_alloc();
        auto result = reinterpret_cast<pointer>(m_next);
        m_next += n * sizeof(T);
        return result;
    }

    static void deallocate(pointer p, size_type n) noexcept
    {
        (void)p; (void)n;
    }

    template <typename U, typename... Args>
    static void construct(U* p,  Args&&... args)
    {
        new (p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    static void destroy(U* p) noexcept
    {
        p->~U();
    }

    bool operator==(const FixedAllocator& other) const noexcept
    {
        return m_start == other.m_start;
    }

    bool operator!=(const FixedAllocator& other) const noexcept
    {
        return !(*this == other);
    }
private:
    char* m_start;
    char* m_next;
    char* m_end;
};