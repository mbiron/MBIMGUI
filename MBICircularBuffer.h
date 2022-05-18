#pragma once

/**
 * @brief Circular buffer with no automatic growing and dynamic memory allocation
 *
 * @tparam T
 */
template <typename T>
class MBICircularBuffer
{
private:
    T *buff;
    int capacity;
    int begin;
    int end;

    void increasecount(int count)
    {
        (count < capacity) ? count++ : count = 1; 
    }

public:
    MBICircularBuffer(int buffsize = 100) : capacity(buffsize)
    {
        buff = new T[capacity];
        begin = 0;
        end = 0;
    }

    ~MBICircularBuffer()
    {
        delete buff;
    }

    int Size() const
    {
        return (end >= begin) ? (end - begin) : (capacity - begin + end);
    }

    void Push(T data)
    {
        if (Size() < capacity)
        {
            buff[end] = data;
            increasecount(end);
        }
    }

    T Pop()
    {
        int count = begin;
        increasecount(begin);
        return buff[count];
    }

    T operator[](size_t idx) const {return buff[idx]} 
    T& operator[](size_t idx) {return buff[idx]} 
};