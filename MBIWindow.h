#pragma once

class MBIWindow{
    private :
        ImVec2 m_size;
        std::string m_name;
    public:
        MBIWindow(std::string name, int height, int width) : m_name(name), m_size(ImVec2((float)width,(float)height)) {}
        virtual ImVec2 GetWindowSize() const {return m_size;}
        std::string GetName() const {return m_name;}
        virtual void Display() = 0 ;
};