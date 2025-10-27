#include "protocol.h"
#include <QtEndian>

Modbus::Modbus()
{
    crc16_isBigEndian=false;
    data_isBigEndian=true;
    addr=0;
    cmd=0;
    regaddr=0;
    regnum=0;
    databuff.clear();
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
    Appenduint16RegAddrBigEndian(send_buff,regaddr);
    switch (cmd)
    {
    case 0x03:
        Appenduint16RegNumBigEndian(send_buff,regnum);
        break;
    case 0x06:
        break;
    case 0x10:
        break;
    default:
        break;
    }
    quint16 crc = calculateCRC16_Modbus(send_buff);
    if(crc16_isBigEndian)
    {
        Appenduint16Crc16BigEndian(send_buff,crc);
    }
    else
    {
        Appenduint16Crc16LittleEndian(send_buff,crc);
    }
    return send_buff;

}

qint16 Modbus::ModbusByteToInt(QByteArray &response)
{
    response.clear();
    return 0;
}

// 添加16位整数（大端序）
void Modbus::Appendint16BigEndian(QByteArray &byteArray,qint16 value)
{
    byteArray.append(static_cast<char>((value >> 8) & 0xFF)); // 高字节
    byteArray.append(static_cast<char>(value & 0xFF));        // 低字节
}

// 添加16位整数（小端序）
void Modbus::Appendint16LittleEndian(QByteArray &byteArray,qint16 value)
{
    byteArray.append(static_cast<char>(value & 0xFF));        // 低字节
    byteArray.append(static_cast<char>((value >> 8) & 0xFF)); // 高字节
}
// 添加寄存器地址（大端序）
void Modbus::Appenduint16RegAddrBigEndian(QByteArray &byteArray,quint16 value)
{
    byteArray.append(static_cast<char>(value >> 8 & 0xFF));        // 高字节
    byteArray.append(static_cast<char>((value ) & 0xFF)); // 低字节
}
// 添加寄存器地址（小端序）
void Modbus::Appenduint16RegAddrLittleEndian(QByteArray &byteArray,quint16 value)
{
    byteArray.append(static_cast<char>(value & 0xFF));        // 低字节
    byteArray.append(static_cast<char>((value >> 8) & 0xFF)); // 高字节
}

// 添加寄存器个数（大端序）
void Modbus::Appenduint16RegNumBigEndian(QByteArray &byteArray,quint16 value)
{
    byteArray.append(static_cast<char>(value >> 8 & 0xFF));        // 高字节
    byteArray.append(static_cast<char>((value ) & 0xFF)); // 低字节
}
// 添加寄存器个数（小端序）
void Modbus::Appenduint16RegNumLittleEndian(QByteArray &byteArray,quint16 value)
{
    byteArray.append(static_cast<char>(value & 0xFF));        // 低字节
    byteArray.append(static_cast<char>((value >> 8) & 0xFF)); // 高字节
}

// 添加crc16（大端序）
void Modbus::Appenduint16Crc16BigEndian(QByteArray &byteArray,quint16 value)
{
    byteArray.append(static_cast<char>(value >> 8 & 0xFF));        // 高字节
    byteArray.append(static_cast<char>((value ) & 0xFF)); // 低字节
}
// 添加crc16（小端序）
void Modbus::Appenduint16Crc16LittleEndian(QByteArray &byteArray,quint16 value)
{
    byteArray.append(static_cast<char>(value & 0xFF));        // 低字节
    byteArray.append(static_cast<char>((value >> 8) & 0xFF)); // 高字节
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

quint16 Modbus::TwoBytesToQuint16(const QByteArray &data, bool isBigEndian)
{
    quint16 result=0;
    if (data.size() != 2)
    {
        return 0;
    }
    quint8 highByte = static_cast<quint8>(data.at(0));
    quint8 lowByte = static_cast<quint8>(data.at(1));
    if(isBigEndian)
    {
        // 大端序：高位在前
        result = (highByte << 8) | lowByte;
    }
    else
    {
        // 小端序：低位在前
        result = (lowByte << 8) | highByte;
    }
    return result;
}


