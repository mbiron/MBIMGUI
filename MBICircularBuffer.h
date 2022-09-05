#pragma once

#include <memory>

/**
 * @brief Circular buffer with no automatic growing nor dynamic memory allocation after construction
 *
 * @tparam T Type of the objects to store
 */
template <typename T>
class MBICircularBuffer
{
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
        pointer m_ptr; ///<
        int m_counter;
        MBICircularBuffer &m_circbuff;

    public:
        /**
         * @brief Construct a new MBICircularIterator object. This iterator allows to walk through the circular buffer ONCE.
         * This means that it's safe to use it in a foreach style loop, even if the circular buffer is full. After reaching
         * the oldest object, the iterator will became invalid.
         *
         * @param ptr Pointer on the iterator ellement
         * @param buff Circular buffer object containing the pointer.
         */
        MBICircularIterator(pointer ptr, MBICircularBuffer &buff) : m_ptr(ptr), m_circbuff(buff), m_counter(0) {}
        MBICircularIterator() = delete;

        /**
         * Operators
         *
         */

        /**
         * @brief Retreive item
         *
         * @return reference on the current item
         */
        reference operator*() const { return *m_ptr; }
        /**
         * @brief Retreive item
         *
         * @return pointer on the current item
         */
        pointer operator->() { return m_ptr; }

        /**
         * @brief Assignment operator
         *
         * @param other
         * @return MBICircularIterator&
         */
        MBICircularIterator &operator=(const MBICircularIterator &other)
        {
            if (this != &other && this->m_circbuff.m_buff == other.m_circbuff.m_buff) // not a self-assignment
            {
                this->m_ptr = other.m_ptr;
                this->m_counter = other.m_counter;
            }
            return *this;
        }

        /**
         * @brief Prefix increment
         *
         * @return MBICircularIterator&
         */
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

        /**
         * @brief Postfix increment
         *
         * @return MBICircularIterator
         */
        MBICircularIterator operator++(int)
        {
            MBICircularIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        /**
         * @brief Comparison operator
         *
         * @param a
         * @param b
         * @return true
         * @return false
         */
        friend bool operator==(const MBICircularIterator &a, const MBICircularIterator &b)
        {
            return a.m_ptr == b.m_ptr;
        };
        /**
         * @brief Comparison operator
         *
         * @param a
         * @param b
         * @return true
         * @return false
         */
        friend bool operator!=(const MBICircularIterator &a, const MBICircularIterator &b)
        {
            return a.m_ptr != b.m_ptr;
        };
    };

private:
    std::unique_ptr<T[]> m_buff;
    int m_capacity;
    int m_begin;
    int m_end;
    bool m_full;

    /**
     * @brief Increase internal counter and circle back if needed.
     *
     * @param count Reference of the counter to move forward.
     */
    void inline increasecount(int &count)
    {
        count = (count + 1) % m_capacity;
    }

    /**
     * @brief Internal size calculation to avoid overloading class to deadlock (occurs when size() is called as an inner call like in operator[]).
     *
     * @return int size of the buffer
     */
    int inline size_unlocked() const
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

public:
    /**
     * @brief Construct a new MBICircularBuffer object.
     *
     * @param capacity capacity of the buffer to be constructed.
     */
    MBICircularBuffer(int capacity = 100) : m_capacity(capacity),
                                            m_buff(std::unique_ptr<T[]>(new T[capacity + 1])),
                                            m_begin(0),
                                            m_end(0),
                                            m_full(false)
    {
    }

    virtual ~MBICircularBuffer(){};
    /*
        MBICircularBuffer &operator=(const MBICircularBuffer &other)
        {
            if (this != &other && this->m_capacity == other.m_capacity) // not a self-assignment
            {
                this->m_begin = other.m_begin;
                this->m_end = other.m_end;
                this->m_full = other.m_full;
                std::copy(&other.m_buff[0], &other.m_buff[0] + other.m_capacity, &this->m_buff[0]);
            }
            return *this;
        }
    */
    /**
     * @brief Return the current buffer size
     *
     * @return int number of objects in the buffer
     */
    virtual int size() const
    {
        return size_unlocked();
    }
    /**
     * @brief Add an object at the end of the buffer. If the buffer is full, the object will replace the oldest one.
     *
     * @param data Object to add
     */
    virtual void push(T data)
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
    virtual T pop()
    {
        if (empty())
            return T();

        int count = m_begin;
        increasecount(m_begin);
        m_full = false;
        return m_buff[count];
    }

    /**
     * @brief Remove the n oldest object inserted into the buffer
     *
     * @param n Number of object to remove
     */
    void remove(int n)
    {
        int test = m_begin;
        if (size_unlocked() >= n)
        {
            if ((m_begin + n) <= m_end)
            {
                m_begin += n;
            }
            else
            {
                m_begin = n - (m_capacity - m_begin);
            }
            m_full = false;
        }
    }

    /**
     * @brief Retreive the last object inserted into the buffer
     *
     * @return T Last object inserted into the buffer
     */
    virtual const T last() const
    {
        if (empty())
            return T();
        return m_buff[m_end - 1];
    }

    /**
     * @brief Retreive the last object inserted into the buffer
     *
     * @return T Last object inserted into the buffer
     */
    virtual T last()
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

    /**
     * @brief Access an element in the buffer. The idx is an offset from the beginning of the buffer (offset from the oldest object inserted)
     *
     * @param idx Offset of the element
     * @return const T&
     */
    const T &operator[](int idx) const
    {
        if (idx > size_unlocked())
        {
            if (m_full)
            {
                return m_buff[m_capacity];
            }
            else
            {
                return m_buff[m_end];
            }
        }

        int index = (m_begin + idx) % m_capacity;
        return m_buff[index];
    }

    /**
     * @brief Access an element in the buffer. The idx is an offset from the beginning of the buffer (offset from the oldest object inserted)
     *
     * @param idx Offset of the element
     * @return const T&
     */
    virtual T &operator[](int idx)
    {
        if (idx > size_unlocked())
            return *end();

        int index = (m_begin + idx) % m_capacity;
        return m_buff[index];
    }
};
