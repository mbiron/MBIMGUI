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
    using WriteLock = std::unique_lock<std::shared_mutex>;
    using ReadLock = std::shared_lock<std::shared_mutex>;
    mutable std::shared_mutex m_mut;

public:
    /**
     * @brief Construct a new MBISyncCircularBuffer object with the specified capacity
     *
     * @param capacity Size of the buffer
     */
    explicit MBISyncCircularBuffer(size_t capacity = 60) : MBICircularBuffer(capacity) {}
    ~MBISyncCircularBuffer(){};

    /**
     * @brief Add an object at the end of the buffer. If the buffer is full, the object will replace the oldest one.
     *
     * @param data Object to add
     */
    void push(const T &data) noexcept override
    {
        WriteLock w_lock(m_mut);
        MBICircularBuffer::push(data);
    }
    /**
     * @brief Empty and reset the buffer
     *
     */
    void reset() noexcept override
    {
        WriteLock w_lock(m_mut);
        MBICircularBuffer::reset();
    }

    /**
     * @brief Retreive and remove the oldest object inserted into the buffer
     *
     * @return T Oldest object inserted into the buffer
     */
    T pop() noexcept override
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
    T &operator[](size_t idx) override
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
    const T &operator[](size_t idx) const override
    {
        ReadLock r_lock(m_mut);
        return MBICircularBuffer::operator[](idx);
    }

    /**
     * @brief Get the current size of the buffer
     *
     * @return int
     */
    size_t size() const noexcept override
    {
        ReadLock r_lock(m_mut);
        return MBICircularBuffer::size();
    }

    /**
     * @brief Retreive the first inserted object
     *
     * @return T
     */
    const T &first() const override
    {
        ReadLock r_lock(m_mut);
        return MBICircularBuffer::first();
    }

    /**
     * @brief Retreive the last inserted object
     *
     * @return T
     */
    const T &last() const override
    {
        ReadLock r_lock(m_mut);
        return MBICircularBuffer::last();
    }

/**
     * @brief Reterive a const iterator in circular mode on the last object in the buffer.
     * This iterator can then be used to walk through the buffer without stopping, keeping read new data.
     *
     * @warning When using circular mode, there is no end() to stop the operation. Be careful when using it in a loop condition.
     *
     * @return MBIConstCircularIterator
     */
    MBICircularBuffer::MBIConstCircularIterator clastcirc() const override
    {
        ReadLock r_lock(m_mut);
        return MBICircularBuffer::clastcirc();
    }

    /**
     * @brief Reterive a const iterator on the oldest object in the buffer
     *
     * @return MBIConstCircularIterator
     */
    MBICircularBuffer::MBIConstCircularIterator cbegin() const override
    {
        ReadLock r_lock(m_mut);
        return MBICircularBuffer::cbegin();
    }

    /**
     * @brief Reterive a const iterator on the end the buffer
     *
     * @return MBIConstCircularIterator
     */
    MBICircularBuffer::MBIConstCircularIterator cend() const override
    {
        ReadLock r_lock(m_mut);
        return MBICircularBuffer::cend();
    }
};
