#pragma once

#include <memory>
#include <iterator>
#include <cstddef>

template <typename T, typename Allocator = std::allocator<T>>
class FixedList
{
public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;

private:
    struct Node
    {
        T data;
        Node* next;
    };

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using NodeTraits = std::allocator_traits<NodeAllocator>;

    NodeAllocator m_nodeAllocator;
    Node* m_head;
    Node* m_tail;
    size_type m_size;

public:
    FixedList() : m_head(nullptr), m_tail(nullptr), m_size(0) {}

    FixedList(const FixedList&) = delete;
    FixedList& operator=(const FixedList&) = delete;

    ~FixedList()
    {
        clear();
    }

    void push_back(const T& value)
    {
        Node* newNode = NodeTraits::allocate(m_nodeAllocator, 1);

        NodeTraits::construct(m_nodeAllocator, newNode);
        newNode->data = value;
        newNode->next = nullptr;

        if (m_tail)
        {
            m_tail->next = newNode;
            m_tail = newNode;
        }
        else
        {
            m_head = m_tail = newNode;
        }
        ++ m_size;
    }

    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(Node* node) : m_node(node) {}

        reference operator*() const { return m_node->data; }
        pointer operator->() const { return &m_node->data; }

        Iterator& operator++()
        {
            m_node = m_node->next;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const
        {
            return m_node == other.m_node;
        }

        bool operator!=(const Iterator& other) const
        {
            return !(*this == other);
        }

    private:
        Node* m_node;
    };

    Iterator begin() { return Iterator(m_head); }
    Iterator end() { return Iterator(nullptr); }

    void clear()
    {
        Node* cur = m_head;
        while (cur)
        {
            Node* next = cur->next;
            NodeTraits::destroy(m_nodeAllocator, cur);
            NodeTraits::deallocate(m_nodeAllocator, cur, 1);
            cur = next;
        }
        m_head = m_tail = nullptr;
        m_size = 0;
    }

    [[nodiscard]] size_type size() const { return m_size; }
    [[nodiscard]] bool empty() const { return m_size == 0; }
};