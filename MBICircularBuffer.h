#pragma once

#include <memory>

/**
 * @brief Circular buffer with no automatic growing and dynamic memory allocation
 *
 * @tparam T
 */
template <typename T>
class MBICircularBuffer
{
private:
    std::unique_ptr<T[]> m_buff;
    int m_capacity;
    int m_begin;
    int m_end;

    void increasecount(int &count)
    {
        (count < m_capacity) ? count++ : count = 0;
    }

public:
    class MBICircularIterator;
    friend class MBICircularIterator;
    class MBICircularIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;   // or also value_type*
        using reference = T &; // or also value_type&
    public:
        MBICircularIterator(pointer ptr, MBICircularBuffer &buff) : m_ptr(ptr), m_circbuff(buff) {}

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }
        // Prefix increment
        MBICircularIterator &operator++()
        {
            if (m_ptr == &(m_circbuff.m_buff[m_circbuff.m_capacity - 1]))
            {
                m_ptr = &(m_circbuff.m_buff[0]);
            }
            else
            {
                m_ptr++;
            }
            return *this;
        }

        // Postfix increment
        MBICircularIterator operator++(int)
        {
            MBICircularIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const MBICircularIterator &a, const MBICircularIterator &b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!=(const MBICircularIterator &a, const MBICircularIterator &b) { return a.m_ptr != b.m_ptr; };

    private:
        pointer m_ptr;
        MBICircularBuffer &m_circbuff;
    };

    MBICircularBuffer(int capacity = 100)
        : m_capacity(capacity), m_buff(std::unique_ptr<T[]>(new T[capacity])), m_begin(0), m_end(0)
    {
    }

    int size() const
    {
        return (m_end >= m_begin) ? (m_end - m_begin) : (m_capacity - m_begin + m_end);
    }

    void push(T data)
    {
        if (size() == m_capacity)
        {
            m_buff[m_begin] = data;
            increasecount(m_begin);
        }
        else
        {
            m_buff[m_end] = data;
        }
        increasecount(m_end);
    }

    void reset()
    {
        m_begin = 0;
        m_end = 0;
    }

    T pop()
    {
        int count = m_begin;
        increasecount(m_begin);
        return m_buff[count];
    }

    MBICircularIterator begin()
    {
        return MBICircularIterator(&(m_buff[m_begin]), *this);
    }

    MBICircularIterator end()
    {
        return MBICircularIterator(&(m_buff[m_end]), *this);
    }

    T operator[](size_t idx) const {return m_buff[m_idx]} T &operator[](size_t idx) { return m_buff[m_idx] }
};
