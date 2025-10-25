#include "protocol.h"

Modbus::Modbus()
{
    addr=0;
    cmd=0;
    regaddr=0;
    regnum=0;
    for (int i = 0; i < BUFF_SIZE; ++i)
    {
        buff[i] = 0;
    }
    send_buff.clear();
    crc16=0;
}
Modbus::~Modbus()
{

}
QByteArray Modbus::ModbusSenddataConfig()
{
    send_buff.clear();
    send_buff.append(addr);
    send_buff.append(cmd);
    Appenduint16RegAddrBigEndian(regaddr);
    switch (cmd)
    {
    case 0x03:
        Appenduint16RegNumBigEndian(regnum);
        break;
    case 0x06:
        break;
    case 0x10:
        break;
    default:
        break;
    }
    quint16 crc = calculateCRC16_Modbus(send_buff);
    Appenduint16Crc16LittleEndian(crc);
    return send_buff;

}

void Modbus::ModbusResponseConfig(QString response)
{
    QByteArray byteArray = QByteArray::fromHex(response.toLatin1());
}

// 添加16位整数（大端序）
void Modbus::Appendint16BigEndian(qint16 value)
{
    send_buff.append(static_cast<char>((value >> 8) & 0xFF)); // 高字节
    send_buff.append(static_cast<char>(value & 0xFF));        // 低字节
}

// 添加16位整数（小端序）
void Modbus::Appendint16LittleEndian(qint16 value)
{
    send_buff.append(static_cast<char>(value & 0xFF));        // 低字节
    send_buff.append(static_cast<char>((value >> 8) & 0xFF)); // 高字节
}
// 添加寄存器地址（大端序）
void Modbus::Appenduint16RegAddrBigEndian(quint16 value)
{
    send_buff.append(static_cast<char>(value >> 8 & 0xFF));        // 高字节
    send_buff.append(static_cast<char>((value ) & 0xFF)); // 低字节
}
// 添加寄存器地址（小端序）
void Modbus::Appenduint16RegAddrLittleEndian(quint16 value)
{
    send_buff.append(static_cast<char>(value & 0xFF));        // 低字节
    send_buff.append(static_cast<char>((value >> 8) & 0xFF)); // 高字节
}

// 添加寄存器个数（大端序）
void Modbus::Appenduint16RegNumBigEndian(quint16 value)
{
    send_buff.append(static_cast<char>(value >> 8 & 0xFF));        // 高字节
    send_buff.append(static_cast<char>((value ) & 0xFF)); // 低字节
}
// 添加寄存器个数（小端序）
void Modbus::Appenduint16RegNumLittleEndian(quint16 value)
{
    send_buff.append(static_cast<char>(value & 0xFF));        // 低字节
    send_buff.append(static_cast<char>((value >> 8) & 0xFF)); // 高字节
}

// 添加crc16（大端序）
void Modbus::Appenduint16Crc16BigEndian(quint16 value)
{
    send_buff.append(static_cast<char>(value >> 8 & 0xFF));        // 高字节
    send_buff.append(static_cast<char>((value ) & 0xFF)); // 低字节
}
// 添加crc16（小端序）
void Modbus::Appenduint16Crc16LittleEndian(quint16 value)
{
    send_buff.append(static_cast<char>(value & 0xFF));        // 低字节
    send_buff.append(static_cast<char>((value >> 8) & 0xFF)); // 高字节
}

quint16 Modbus::calculateCRC16_Modbus(const QByteArray &data)
{
    quint16 crc = 0xFFFF;

    for (int i = 0; i < data.length(); ++i) {
        crc ^= static_cast<quint8>(data[i]);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}
