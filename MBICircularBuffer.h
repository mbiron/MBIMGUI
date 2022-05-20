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
    bool m_full;

    void increasecount(int &count)
    {
        count = (count + 1) % m_capacity;
    }

public:
    class MBICircularIterator;
    friend class MBICircularIterator;
    class MBICircularIterator
    {
    private:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;   // or also value_type*
        using reference = T &; // or also value_type&

        pointer m_ptr;
        int m_counter;
        MBICircularBuffer &m_circbuff;

    public:
        MBICircularIterator(pointer ptr, MBICircularBuffer &buff) : m_ptr(ptr), m_circbuff(buff), m_counter(0) {}

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }
        // Prefix increment
        MBICircularIterator &operator++()
        {
            if (m_circbuff.full() && m_counter == m_circbuff.m_capacity - 1)
            {
                // If buffer is full, as begin == end, we rely on a counter to detect the last element
                m_ptr = &(m_circbuff.m_buff[m_circbuff.m_capacity]);
            }
            else if (m_ptr == &(m_circbuff.m_buff[m_circbuff.m_capacity - 1]))
            {
                // Go back to start
                m_ptr = &(m_circbuff.m_buff[0]);
            }
            else
            {
                // Move in the buffer
                m_ptr++;
            }
            m_counter++;
            return *this;
        }

        // Postfix increment
        MBICircularIterator operator++(int)
        {
            MBICircularIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const MBICircularIterator &a, const MBICircularIterator &b)
        {
            return a.m_ptr == b.m_ptr;
        };
        friend bool operator!=(const MBICircularIterator &a, const MBICircularIterator &b)
        {
            return a.m_ptr != b.m_ptr;
        };
    };

    MBICircularBuffer(int capacity = 100) : m_capacity(capacity),
                                            m_buff(std::unique_ptr<T[]>(new T[capacity + 1])),
                                            m_begin(0),
                                            m_end(0),
                                            m_full(false)
    {
    }

    int size() const
    {
        if (m_full && m_end == m_begin)
        {
            return m_capacity;
        }
        else
        {
            return (m_end >= m_begin) ? (m_end - m_begin) : (m_capacity - m_begin + m_end);
        }
    }

    void push(T data)
    {
        m_buff[m_end] = data;

        if (m_full)
        {
            increasecount(m_begin);
        }
        increasecount(m_end);
        if (m_begin == m_end)
            m_full = true;
    }

    void reset()
    {
        m_begin = 0;
        m_end = 0;
        m_full = false;
    }

    int getoffset() const
    {
        return m_begin;
    }

    T pop()
    {
        if (empty())
            return T();

        int count = m_begin;
        increasecount(m_begin);
        m_full = false;
        return m_buff[count];
    }

    bool full() const
    {
        return m_full;
    }

    bool empty() const
    {
        return (!m_full && m_begin == m_end);
    }

    MBICircularIterator begin()
    {
        return MBICircularIterator(&(m_buff[m_begin]), *this);
    }

    MBICircularIterator end()
    {
        if (m_full)
        {
            return MBICircularIterator(&(m_buff[m_capacity]), *this);
        }
        else
        {
            return MBICircularIterator(&(m_buff[m_end]), *this);
        }
    }

    T operator[](int idx) const
    {
        if (idx > size())
            return T();
        int index = (m_begin + idx) % size();
        return m_buff[index];
    }
};
