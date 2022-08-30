#pragma once
#include <mutex>
#include "MBICircularBuffer.h"

/**
 * @brief Circular buffer with no automatic growing nor dynamic memory allocation and thread safe.
 * Thread protection is done through a internal mutex.
 *
 * @tparam T Type of the objects to store
 */
template <typename T>
class MBISyncCircularBuffer : public MBICircularBuffer<T>
{
    std::mutex m_mut;

public:
    /**
     * @brief Construct a new MBISyncCircularBuffer object with the specified capacity
     *
     * @param capacity Size of the buffer
     */
    MBISyncCircularBuffer(int capacity = 60) : MBICircularBuffer(capacity) {}
    virtual ~MBISyncCircularBuffer(){};

    void push(T data)
    {
        std::lock_guard<std::mutex> guard(m_mut);
        MBICircularBuffer::push(data);
    }

    T pop()
    {
        std::lock_guard<std::mutex> guard(m_mut);
        return MBICircularBuffer::pop();
    }

    T &operator[](int idx)
    {
        std::lock_guard<std::mutex> guard(m_mut);
        return MBICircularBuffer::operator[](idx);
    }

    /**
     * @brief Get the current size of the buffer
     *
     * @return int
     */
    int size()
    {
        std::lock_guard<std::mutex> guard(m_mut);
        return MBICircularBuffer::size();
    }

    /**
     * @brief Retreive the last inserted object
     *
     * @return T
     */
    T last()
    {
        std::lock_guard<std::mutex> guard(m_mut);
        return MBICircularBuffer::last();
    }
};