#pragma once

/**
 * @brief Abstract class to define renderer interface.
 *
 */
class Renderer
{
public:
    /**
     * @brief Internal intialization of the renderer
     *
     * @return true If succeed
     * @return false If an error occured
     */
    virtual bool Init() = 0;
    /**
     * @brief To be called after all graphical component have been submitted.
     * Render all component.
     *
     */
    virtual void Render() = 0;
    /**
     * @brief Initialize a new frame. To be called before submitting any graphical component.
     * 
     */
    virtual void NewFrame() = 0;
    /**
     * @brief Resize current window 
     * 
     * @param param New size of the window. Format depending on current backend.
     */
    virtual void Resize(void *param) = 0;
    /**
     * @brief Stop all rendering and shutdown the renderer engine.
     * 
     */
    virtual void Shutdown() = 0;
    /**
     * @brief Free memory and release all graphical objects.
     * 
     */
    virtual void Destroy() = 0;
};