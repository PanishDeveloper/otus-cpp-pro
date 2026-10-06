#pragma once

#include <cstddef>
#include <array>
#include <map>
#include <tuple>
#include <type_traits>
#include <utility>
#include <stdexcept>

namespace sparse
{
    template <typename T, T Default, size_t N = 2>
    class Matrix
    {
    public:
        using Key = std::array<int, N>;

        Matrix() = default;

        [[nodiscard]] T get(const Key& key) const
        {
            auto it = m_data.find(key);
            return it == m_data.end() ? Default : it->second;
        }
        void set(const Key& key, const T& value)
        {
            if (value == Default)
            {
                if (m_data.erase(key) > 0) ++m_version;
            }
            else
                m_data[key] = value;
        }
        [[nodiscard]] std::size_t size() const noexcept { return m_data.size(); }
        [[nodiscard]] bool        empty() const noexcept { return m_data.empty(); }
        [[nodiscard]] std::size_t version() const noexcept { return m_version; }


        template <std::size_t K>
        class Proxy
        {
        public:
            template <std::size_t K2 = K, typename = std::enable_if_t<K2 < N>>
            auto operator[](int idx)
            {
                std::array<int, K + 1> next{};
                for (std::size_t i = 0; i < K; ++i) next[i] = m_indices[i];
                next[K] = idx;
                return Proxy<K + 1>(m_matrix, next);
            }

            operator T() const
            {
                static_assert(K == N, "Proxy is convertible to T only at terminal level (K == N)");
                return m_matrix->get(m_indices);
            }

            Proxy& operator=(const T& value)
            {
                static_assert(K == N, "Proxy assignment is only valid at terminal level (K == N)");
                m_matrix->set(m_indices, value);
                return *this;
            }

            Proxy& operator=(const Proxy& other)
            {
                if (this == &other) return *this;
                operator=(static_cast<T>(other));
                return *this;
            }

        private:
            friend class Matrix;
            friend class Proxy<K - 1>;
            friend class Proxy<K + 1>;
            Proxy(Matrix* m, const std::array<int, K>& idx) noexcept : m_matrix(m), m_indices(idx){}

            Matrix* m_matrix;
            std::array<int, K> m_indices;
        };

        auto operator[](int idx)
        {
            return Proxy<1>(this, std::array<int, 1>{idx});
        }

        class Iterator
        {
        public:
            using BaseIt = typename std::map<Key, T>::const_iterator;

            Iterator(BaseIt it, const Matrix* matrix) noexcept : m_it(it), m_matrix(matrix), m_seen(matrix->version()) {}

            Iterator& operator++() { checkAlive(), ++m_it; return *this; }
            Iterator operator++(int) { Iterator tmp = *this; ++*this; return tmp; }

            bool operator==(const Iterator& other) const { return m_it == other.m_it; }
            bool operator!=(const Iterator& other) const { return m_it != other.m_it; }

            auto operator*() const
            {
                checkAlive();
                return unpack(m_it->first, m_it->second, std::make_index_sequence<N>{});
            }
        private:
            void checkAlive() const
            {
                if (m_matrix->version() != m_seen)
                    throw std::logic_error("Matrix modified during iteration");
            }

            template <std::size_t... Is>
            auto unpack(const Key& key, const T& value, std::index_sequence<Is...>) const
            {
                return std::make_tuple(key[Is]..., value);
            }
            BaseIt m_it;
            const Matrix* m_matrix;
            std::size_t m_seen;
        };

        [[nodiscard]] Iterator begin() const { return Iterator(m_data.cbegin(), this); }
        [[nodiscard]] Iterator end()   const { return Iterator(m_data.cend(), this); }

    private:
        std::map<Key, T> m_data;
        std::size_t m_version = 0;
    };
}
