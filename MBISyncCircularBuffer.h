#pragma once
#include "MBICircularBuffer.h"
/**
 * @brief Circular buffer with no automatic growing nor dynamic memory allocation and thread safe
 *
 * @tparam T
 */
template <typename T>
class MBISyncCircularBuffer : MBICircularBuffer<T>
{
    std::mutex m_mut;

public:
    MBISyncCircularBuffer(int capacity = 100) : MBICircularBuffer(capacity) {}
    virtual ~MBISyncCircularBuffer(){};

    void push(T data)
    {
        std::lock_guard<std::mutex> guard(m_mut);
        MBICircularBuffer::push(data);
    }

    void reset()
    {
        std::lock_guard<std::mutex> guard(m_mut);
        MBICircularBuffer::reset();
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

    int size()
    {
        std::lock_guard<std::mutex> guard(m_mut);
        return MBICircularBuffer::size(); 
    }
};