#pragma once
class Renderer
{
    public:
        virtual bool Init() = 0;
        virtual void Render() = 0;
        virtual void NewFrame() = 0;
        virtual void Resize(void * param) = 0;
        virtual void Shutdown() = 0;
        virtual void Destroy() = 0;

};