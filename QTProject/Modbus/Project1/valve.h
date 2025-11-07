#ifndef VALVE_H
#define VALVE_H

#include <QObject>

class Valve
{
public:
    Valve();
    ~Valve();
public:
    int SerBaudRateSwitch(int idex);
public:
    quint8 current_addr;
    quint8 target_addr;
    quint16 max_speed;
    quint16 min_speed;
    quint16 acceleration;
    quint16 deceleration;
    quint16 rated_current;
    quint16 max_channel;
    quint16 can_baudrate;
    quint16 ser_baudrate;
    quint16 state;
    quint16 current_channel;
    quint16 target_channel;
    quint16 dev_id;
    quint16 dev_model;
    quint16 dev_version;
};

typedef enum
{
    OPTIMAL_SWITCH=0x01,
    COUNTERCLOCKWISE_SWITCH,
    CLOCKWISE_SWITCH,
    INITIALIZATION=0x05,
    STOP,
    CLEAR_ERROR,
    MAX_SPEED=0x51,
    MIN_SPEED,
    ACCELERATION,
    DECELERATION,
    RATED_CURRENT,
    MAX_CHANNEL=0x58,
    CAN_BAUDRATE=0x6D,
    SER_BAUDRATE,
    ADDRESS,
    STATE=0x90,
    CURRENT_CHANNEL,
    RESET=0xEE,
    SAVE,
    DEV_ID,
    DEV_MODEL,
    DEV_VERSION,
}Valve_Modbus_Reg;

#endif // VALVE_H
