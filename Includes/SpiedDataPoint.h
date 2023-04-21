#pragma once

#include <stdint.h>

/**
 * @brief Struct describing a data occurence. It's basically a value with the corresponding date.
 */
struct SpiedDataPoint
{
    float m_time; ///< Time in s
    float m_data; ///< Data value

    /**
     * @brief Construct a new Spied Data Point object
     *
     * @param x x-axis data : time
     * @param y y-axis data : value
     */
    SpiedDataPoint::SpiedDataPoint(float x = 0, float y = 0) : m_time(x),
                                                               m_data(y)
    {
    }
};

/**
 * @brief Struct describing an annotation displayed on a graph.
 * 
 */
struct DataAnnotation
{
    float m_x; ///< Time in s
    float m_y; ///< Data value

    DataAnnotation(float x = 0, float y = 0) : m_x(x), m_y(y) {}

    virtual const char *getLabel() const { return ""; };
};