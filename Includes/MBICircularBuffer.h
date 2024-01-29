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
        pointer m_ptr;
        int m_counter;
        MBICircularBuffer &m_circbuff;
        bool m_bCircular;

    public:
        /**
         * @brief Construct a new MBICircularIterator object. This iterator allows to walk through the circular buffer ONCE.
         * This means that it's safe to use it in a foreach style loop, even if the circular buffer is full. After reaching
         * the oldest object, the iterator will became invalid.
         *
         * @param ptr Pointer on the iterator ellement
         * @param buff Circular buffer object containing the pointer.
         * @param bCircular Make the iterator circular. If true, the iterator will keep walking through the buffer indefinitely.
         * @warning When using circular mode, there is no end() to stop the operation. Don't use it as a loop condition.
         */
        explicit MBICircularIterator(pointer ptr, MBICircularBuffer &buff, bool bCircular = false) : m_ptr(ptr),
                                                                                                     m_circbuff(buff),
                                                                                                     m_counter(0),
                                                                                                     m_bCircular(bCircular)
        {
        }
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
        reference operator*() const noexcept { return *m_ptr; }
        /**
         * @brief Retreive item
         *
         * @return pointer on the current item
         */
        pointer operator->() noexcept { return m_ptr; }

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
                this->m_bCircular = other.m_bCircular;
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
            /* In circular mode, we never reach the end. In normal mode, as begin == end, we rely on a counter to detect the last element */
            if (m_bCircular == false && m_circbuff.full() && m_counter == m_circbuff.m_capacity - 1)
            {
                // The last element is stored at the last place of the buffer
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

            if (m_bCircular == false)
            {
                m_counter++;
            }
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
        friend bool operator==(const MBICircularIterator &a, const MBICircularIterator &b) noexcept
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
        friend bool operator!=(const MBICircularIterator &a, const MBICircularIterator &b) noexcept
        {
            return a.m_ptr != b.m_ptr;
        };
    };

    /**
     * @brief Forward const iterator on a MBICircularBuffer object
     *
     */
    class MBIConstCircularIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const T;
        using pointer = const T *;
        using reference = const T &;

    private:
        pointer m_ptr;
        int m_counter;
        const MBICircularBuffer &m_circbuff;
        bool m_bCircular;

    public:
        /**
         * @brief Construct a new MBIConstCircularIterator object. This iterator allows to walk through the circular buffer ONCE.
         * This means that it's safe to use it in a foreach style loop, even if the circular buffer is full. After reaching
         * the oldest object, the iterator will became invalid.
         *
         * @param ptr Pointer on the iterator ellement
         * @param buff Circular buffer object containing the pointer.
         * @param bCircular Make the iterator circular. If true, the iterator will keep walking through the buffer indefinitely.
         * @warning When using circular mode, there is no end() to stop the operation. Don't use it as a loop condition.
         */
        explicit MBIConstCircularIterator(pointer ptr, const MBICircularBuffer &buff, bool bCircular = false) : m_ptr(ptr),
                                                                                                                m_circbuff(buff),
                                                                                                                m_counter(0),
                                                                                                                m_bCircular(bCircular)
        {
        }
        MBIConstCircularIterator() = delete;

        /**
         * Operators
         *
         */

        /**
         * @brief Retreive item
         *
         * @return reference on the current item
         */
        reference operator*() const noexcept { return *m_ptr; }
        /**
         * @brief Retreive item
         *
         * @return pointer on the current item
         */
        pointer operator->() noexcept { return m_ptr; }

        /**
         * @brief Assignment operator
         *
         * @param other
         * @return MBICircularIterator&
         */
        MBIConstCircularIterator &operator=(const MBIConstCircularIterator &other)
        {
            if (this != &other && this->m_circbuff.m_buff == other.m_circbuff.m_buff) // not a self-assignment
            {
                this->m_ptr = other.m_ptr;
                this->m_counter = other.m_counter;
                this->m_bCircular = other.m_bCircular;
            }
            return *this;
        }

        /**
         * @brief Prefix increment
         *
         * @return MBICircularIterator&
         */
        MBIConstCircularIterator &operator++()
        {
            /* In circular mode, we never reach the end. In normal mode, as begin == end, we rely on a counter to detect the last element */
            if (m_bCircular == false && m_circbuff.full() && m_counter == m_circbuff.m_capacity - 1)
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
            if (m_bCircular == false)
            {
                m_counter++;
            }
            return *this;
        }

        /**
         * @brief Postfix increment
         *
         * @return MBICircularIterator
         */
        MBIConstCircularIterator operator++(int)
        {
            MBIConstCircularIterator tmp = *this;
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
        friend bool operator==(const MBIConstCircularIterator &a, const MBIConstCircularIterator &b) noexcept
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
        friend bool operator!=(const MBIConstCircularIterator &a, const MBIConstCircularIterator &b) noexcept
        {
            return a.m_ptr != b.m_ptr;
        };
    };

private:
    std::unique_ptr<T[]> m_buff;
    size_t m_capacity;
    size_t m_begin;
    size_t m_end;
    bool m_full;

    /**
     * @brief Increase internal counter and circle back if needed.
     *
     * @param count Reference of the counter to move forward.
     */
    void inline increasecount(size_t &count) noexcept
    {
        count = (count + 1) % m_capacity;
    }

    /**
     * @brief Internal size calculation to avoid overloading class to deadlock (occurs when size() is called as an inner call like in operator[]).
     *
     * @return int size of the buffer
     */
    size_t inline size_unlocked() const noexcept
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
    explicit MBICircularBuffer(size_t capacity = 100) noexcept : m_capacity(capacity),
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
    virtual size_t size() const noexcept
    {
        return size_unlocked();
    }
    /**
     * @brief Add an object at the end of the buffer. If the buffer is full, the object will replace the oldest one.
     *
     * @param data Object to add
     */
    virtual void push(const T &data) noexcept
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
    virtual void reset() noexcept
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
    virtual T pop() noexcept
    {
        if (empty())
            return T();

        size_t count = m_begin;
        increasecount(m_begin);
        m_full = false;
        return m_buff[count];
    }

    /**
     * @brief Remove the n oldest object inserted into the buffer
     *
     * @param n Number of object to remove
     */
    void remove(size_t n) noexcept
    {
        size_t test = m_begin;
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
    virtual const T &last() const
    {
        if (empty())
            throw std::out_of_range("Buffer is empty");
        return m_buff[m_end - 1];
    }

    /**
     * @brief Retreive the last object inserted into the buffer
     *
     * @return T Last object inserted into the buffer
     */
    T &last()
    {
        if (empty())
            throw std::out_of_range("Buffer is empty");
        return m_buff[m_end - 1];
    }

    /**
     * @brief Retreive the oldest object inserted into the buffer
     *
     * @return T Oldest object inserted into the buffer
     */
    virtual const T &first() const
    {
        if (empty())
            throw std::out_of_range("Buffer is empty");
        return m_buff[m_begin];
    }

    /**
     * @brief Retreive the oldest object inserted into the buffer
     *
     * @return T Oldest object inserted into the buffer
     */
    T &first()
    {
        if (empty())
            throw std::out_of_range("Buffer is empty");
        return m_buff[m_begin];
    }

    /**
     * @brief Check if the buffer is full
     *
     * @return true If the buffer is full
     * @return false If the buffer is not full
     */
    bool full() const noexcept
    {
        return m_full;
    }
    /**
     * @brief Check if the buffer is empty
     *
     * @return true If the buffer is empty
     * @return false If the buffer is not empty
     */
    bool empty() const noexcept
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
     * @brief Reterive a const iterator in circular mode on the last object in the buffer.
     * This iterator can then be used to walk through the buffer without stopping, keeping read new data.
     *
     * @warning When using circular mode, there is no end() to stop the operation. Be careful when using it in a loop condition.
     *
     * @return MBIConstCircularIterator
     */
    virtual MBIConstCircularIterator clastcirc() const
    {
        if (empty())
        {
            return MBIConstCircularIterator(&(m_buff[m_begin]), *this, true);
        }
        else
        {
            return MBIConstCircularIterator(&(m_buff[m_end - 1]), *this, true);
        }
    }

    /**
     * @brief Reterive a const iterator on the oldest object in the buffer
     *
     * @return MBIConstCircularIterator
     */
    virtual MBIConstCircularIterator cbegin() const
    {
        return MBIConstCircularIterator(&(m_buff[m_begin]), *this);
    }

    /**
     * @brief Reterive a const iterator on the end the buffer
     *
     * @return MBIConstCircularIterator
     */
    virtual MBIConstCircularIterator cend() const
    {
        if (m_full)
        {
            return MBIConstCircularIterator(&(m_buff[m_capacity]), *this);
        }
        else
        {
            return MBIConstCircularIterator(&(m_buff[m_end]), *this);
        }
    }

    /**
     * @brief Access an element in the buffer. The idx is an offset from the beginning of the buffer (offset from the oldest object inserted)
     *
     * @param idx Offset of the element
     * @return const T&
     */
    virtual const T &operator[](size_t idx) const
    {
        const size_t size = size_unlocked();

        size_t index = (m_begin + (idx % size)) % size;
        return m_buff[index];
    }

    /**
     * @brief Access an element in the buffer. The idx is an offset from the beginning of the buffer (offset from the oldest object inserted)
     *
     * @param idx Offset of the element
     * @return T&
     */
    virtual T &operator[](size_t idx)
    {
        const size_t size = size_unlocked();
        size_t index = (m_begin + (idx % size)) % size;
        return m_buff[index];
    }
};
