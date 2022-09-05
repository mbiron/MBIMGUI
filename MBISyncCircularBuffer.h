#pragma once
#include <mutex>
#include <shared_mutex>
#include "MBICircularBuffer.h"

/**
 * @brief Circular buffer with no automatic growing nor dynamic memory allocation and thread safe.
 * Thread protection is done through a read/write lock mecanism.
 *
 * @tparam T Type of the objects to store
 */
template <typename T>
class MBISyncCircularBuffer : public MBICircularBuffer<T>
{
    // std::mutex m_mut;

    typedef std::unique_lock<std::shared_mutex> WriteLock;
    typedef std::shared_lock<std::shared_mutex> ReadLock;
    mutable std::shared_mutex m_mut;

public:
    /**
     * @brief Construct a new MBISyncCircularBuffer object with the specified capacity
     *
     * @param capacity Size of the buffer
     */
    MBISyncCircularBuffer(int capacity = 60) : MBICircularBuffer(capacity) {}
    ~MBISyncCircularBuffer(){};

    /**
     * @brief Add an object at the end of the buffer. If the buffer is full, the object will replace the oldest one.
     *
     * @param data Object to add
     */
    void push(T data)
    {
        WriteLock w_lock(m_mut);
        MBICircularBuffer::push(data);
    }

    /**
     * @brief Retreive and remove the oldest object inserted into the buffer
     *
     * @return T Oldest object inserted into the buffer
     */
    T pop()
    {
        WriteLock w_lock(m_mut);
        return MBICircularBuffer::pop();
    }

    /**
     * @brief Access an element in the buffer. The idx is an offset from the beginning of the buffer (offset from the oldest object inserted)
     *
     * @param idx Offset of the element
     * @return const T&
     */
    T &operator[](int idx)
    {
        WriteLock w_lock(m_mut);
        return MBICircularBuffer::operator[](idx);
    }

    /**
     * @brief Access an element in the buffer. The idx is an offset from the beginning of the buffer (offset from the oldest object inserted)
     *
     * @param idx Offset of the element
     * @return const T&
     */
    const T &operator[](int idx) const
    {
        ReadLock w_lock(m_mut);
        return MBICircularBuffer::operator[](idx);
    }

    /**
     * @brief Get the current size of the buffer
     *
     * @return int
     */
    int size() const
    {
        ReadLock w_lock(m_mut);
        return MBICircularBuffer::size();
    }

    /**
     * @brief Retreive the last inserted object
     *
     * @return T
     */
    const T last() const
    {
        ReadLock w_lock(m_mut);
        return MBICircularBuffer::last();
    }
};