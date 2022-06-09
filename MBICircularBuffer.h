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
    /**
     * @brief Forward iterator on a MBICircularBuffer object
     *
     */
    class MBICircularIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;

    private:
        pointer m_ptr;
        int m_counter;
        MBICircularBuffer &m_circbuff;

    public:
        /**
         * @brief Construct a new MBICircularIterator object
         *
         * @param ptr
         * @param buff
         */
        MBICircularIterator(pointer ptr, MBICircularBuffer &buff) : m_ptr(ptr), m_circbuff(buff), m_counter(0) {}
        MBICircularIterator() = delete;

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

    /**
     * @brief Construct a new MBICircularBuffer object
     *
     * @param capacity capacity of the buffer
     */
    MBICircularBuffer(int capacity = 100) : m_capacity(capacity),
                                            m_buff(std::unique_ptr<T[]>(new T[capacity + 1])),
                                            m_begin(0),
                                            m_end(0),
                                            m_full(false)
    {
    }

    /**
     * @brief Return the current buffer size
     *
     * @return int number of objects in the buffer
     */
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
    /**
     * @brief Add an object at the end of the buffer. If the buffer is full, the object will replace the oldest one.
     *
     * @param data Object to add
     */
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
    /**
     * @brief Empty and reset the buffer
     *
     */
    void reset()
    {
        m_begin = 0;
        m_end = 0;
        m_full = false;
    }
    /**
     * @brief Retreive and remove the oldest object inserted into the buffer
     *
     * @return T Oldest object inserted into the buffer
     */
    T pop()
    {
        if (empty())
            return T();

        int count = m_begin;
        increasecount(m_begin);
        m_full = false;
        return m_buff[count];
    }

    /**
     * @brief Retreive the last object inserted into the buffer
     *
     * @return T Last object inserted into the buffer
     */
    T last() const
    {
        if (empty())
            return T();
        return m_buff[m_end - 1];
    }
    /**
     * @brief Retreive the oldest object inserted into the buffer
     *
     * @return T Oldest object inserted into the buffer
     */
    T first() const
    {
        if (empty())
            return T();
        return m_buff[m_begin];
    }

    /**
     * @brief Check if the buffer is full
     *
     * @return true If the buffer is full
     * @return false If the buffer is not full
     */
    bool full() const
    {
        return m_full;
    }
    /**
     * @brief Check if the buffer is empty
     *
     * @return true If the buffer is empty
     * @return false If the buffer is not empty
     */
    bool empty() const
    {
        return (!m_full && m_begin == m_end);
    }

    /**
     * @brief Reterive an iterator on the oldest object in the buffer
     *
     * @return MBICircularIterator
     */
    MBICircularIterator begin()
    {
        return MBICircularIterator(&(m_buff[m_begin]), *this);
    }

    /**
     * @brief Reterive an iterator on the end the buffer
     *
     * @return MBICircularIterator
     */
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

    const T &operator[](int idx) const
    {
        if (idx > size())
            return T();
        int index = (m_begin + idx) % size();
        return m_buff[index];
    }

    T &operator[](int idx)
    {
        int index = (m_begin + idx) % size();
        return m_buff[index];
    }
};
