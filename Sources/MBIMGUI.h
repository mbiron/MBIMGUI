class MBIMGUI
{
    private:
        int m_width;
        int m_height;

    public:
        MBIMGUI(int width, int height) : m_width(width), m_height(height) {};
        ~MBIMGUI(){};

        void Display();
};