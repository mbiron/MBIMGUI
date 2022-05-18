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
            if (m_circbuff.full())
            {
                if ((m_circbuff.m_end != m_circbuff.m_capacity))
                {
                    if (m_ptr == &(m_circbuff.m_buff[m_circbuff.m_end]))
                    {
                        m_ptr = &(m_circbuff.m_buff[m_circbuff.m_capacity]);
                        return *this;
                    }
                }
            }
            // if (m_circbuff.m_end != m_circbuff.m_capacity)
            // {
            //     // If last element of the FIFO is also the last of the buffer,
            //     // reach the end of the buffer to stop the loop
            //     m_ptr++;
            // }

            if ((m_ptr == &(m_circbuff.m_buff[m_circbuff.m_capacity - 1])) && (m_circbuff.m_end != m_circbuff.m_capacity))
            {
                // Go back to start
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

        friend bool operator==(const MBICircularIterator &a, const MBICircularIterator &b)
        {
            return a.m_ptr == b.m_ptr;
        };
        friend bool operator!=(const MBICircularIterator &a, const MBICircularIterator &b)
        {
            return a.m_ptr != b.m_ptr;
        };

    private:
        pointer m_ptr;
        MBICircularBuffer &m_circbuff;
    };

    MBICircularBuffer(int capacity = 100)
        : m_capacity(capacity), m_buff(std::unique_ptr<T[]>(new T[capacity + 1])), m_begin(0), m_end(0)
    {
    }

    int size() const
    {
        return (m_end >= m_begin) ? (m_end - m_begin) : (m_capacity - m_begin + m_end + 1);
    }

    void push(T data)
    {
        if (full())
        {
            m_buff[m_begin] = data;
            if (m_begin == m_capacity - 1)
            {
                m_begin = 0;
            }
            else
            {
                increasecount(m_begin);
            }
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

    bool full()
    {
        return size() == m_capacity;
    }

    MBICircularIterator begin()
    {
        return MBICircularIterator(&(m_buff[m_begin]), *this);
    }

    MBICircularIterator end()
    {
        if (full())
        {
            return MBICircularIterator(&(m_buff[m_capacity]), *this);
        }
        else
        {
            return MBICircularIterator(&(m_buff[m_end]), *this);
        }
    }

    T operator[](size_t idx) const {return m_buff[m_idx]} T &operator[](size_t idx) { return m_buff[m_idx] }
};
