#pragma once

class MBIWindow{
    struct MBSize
    {
        int height;
        int width;
        MBSize(): height(0),width(0){};
        MBSize(int h,int w): height(h),width(w){};
    };
    private :
        MBSize m_size;
    public:
        MBIWindow(int height, int width) : m_size(MBSize(height,width)) {}
        virtual MBSize GetWindowSize() const {return m_size;}
        virtual void Display() = 0 ;
};