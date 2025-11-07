#include "valve.h"

Valve::Valve()
{
    ser_baudrate=0;
    can_baudrate=500;
}
Valve::~Valve()
{

}

int Valve::SerBaudRateSwitch(int idex)
{
    int ser_baudrate;
    switch(idex)
    {
    case 0:
        ser_baudrate=9600;
        break;
    case 1:
        ser_baudrate=19200;
        break;
    case 2:
        ser_baudrate=38400;
        break;
    case 3:
        ser_baudrate=57600;
        break;
    case 4:
        ser_baudrate=115200;
        break;
    default:
        ser_baudrate=9600;
        break;
    }
    return ser_baudrate;
}
