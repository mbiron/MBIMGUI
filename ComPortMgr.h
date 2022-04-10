#pragma once
#include <vector>

class ComPortMgr
{
public:
    struct COMPORT
    {
        char port[6];
        COMPORT operator=(COMPORT &src)
        {
            strcpy_s(port, src.port);
            return *this;
        }
    };

private:
    COMPORT m_currentPort;

public:
    void GetAvailableComPorts(std::vector<COMPORT> &ports)
    {
        COMPORT availablesports[] = {"COM1", "COM2", "COM7"};
        for (int i = 0; i < 3; i++)
        {
            ports.push_back(availablesports[i]);
        }
    }
    void StartCom(COMPORT port)
    {
        m_currentPort = port;
    }
    void StopCom() {}
    float getNextData()
    {
        return (float)((rand() % 100) / 100.0);
    }
};