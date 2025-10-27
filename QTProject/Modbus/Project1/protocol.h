#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>

#define BUFF_SIZE 10

class Modbus
{
public:
    explicit Modbus();
    ~Modbus();
public:
    QByteArray ModbusSenddataConfig();
    qint16 ModbusByteToInt(QByteArray &response);
    void Appendint16BigEndian(QByteArray &byteArray,qint16 value);
    void Appendint16LittleEndian(QByteArray &byteArray,qint16 value) ;
    void Appenduint16RegAddrBigEndian(QByteArray &byteArray,quint16 value) ;
    void Appenduint16RegAddrLittleEndian(QByteArray &byteArray,quint16 value);
    void Appenduint16RegNumBigEndian(QByteArray &byteArray,quint16 value) ;
    void Appenduint16RegNumLittleEndian(QByteArray &byteArray,quint16 value);
    void Appenduint16Crc16BigEndian(QByteArray &byteArray,quint16 value) ;
    void Appenduint16Crc16LittleEndian(QByteArray &byteArray,quint16 value);
    quint16 calculateCRC16_Modbus(const QByteArray &data);
    quint16 TwoBytesToQuint16(const QByteArray &data, bool isBigEndian);
public:
    bool crc16_isBigEndian;
    bool data_isBigEndian;
    quint8 addr;//设备地址
    quint8 cmd;//读写命令
    quint16 regaddr;//寄存器地址
    quint16 regnum;//读写寄存器个数
    QByteArray  databuff;//数据数组
    QByteArray send_buff;//格式化后发送字节命令
    quint16 crc16;

};

#endif // PROTOCOL_H
