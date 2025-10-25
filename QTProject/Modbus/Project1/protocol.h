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
    void ModbusResponseConfig(QString response);
    void Appendint16BigEndian(qint16 value);
    void Appendint16LittleEndian(qint16 value) ;
    void Appenduint16RegAddrBigEndian(quint16 value) ;
    void Appenduint16RegAddrLittleEndian(quint16 value);
    void Appenduint16RegNumBigEndian(quint16 value) ;
    void Appenduint16RegNumLittleEndian(quint16 value);
    void Appenduint16Crc16BigEndian(quint16 value) ;
    void Appenduint16Crc16LittleEndian(quint16 value);
    quint16 calculateCRC16_Modbus(const QByteArray &data);
public:
    quint8 addr;
    quint8 cmd;
    quint16 regaddr;
    quint16 regnum;
    qint16  buff[BUFF_SIZE];
    QByteArray send_buff;
    quint16 crc16;

};

#endif // PROTOCOL_H
