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
    single_data=0;
    crc16=0;
}
Modbus::~Modbus()
{

}
QByteArray Modbus::ModbusSenddataConfig()
{
    send_buff.clear();
    send_buff.append(addr);//写地址
    send_buff.append(cmd);//写命令
    Appenduint16RegAddrBigEndian(send_buff,regaddr);//写寄存器地址
    switch (cmd)
    {
    case 0x03:
        Appenduint16RegNumBigEndian(send_buff,regnum);//写寄存器个数
        break;
    case 0x06:
        Appendint16BigEndian(send_buff,single_data);//写数据
        break;
    case 0x10:
        Appendint16BigEndian(send_buff,regnum);//写寄存器个数
        Appenduint8BigEndian(send_buff,databuff.size());//写数据个数
        AppendQByteArray(send_buff,databuff);//写数据
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

quint16 Modbus::ModbusSingleRegByteToInt(QByteArray &response)
{
    QByteArray data= response.mid(3,2);
    return TwoBytesToQuint16(data,true);

}
void Modbus::AppendQByteArray(QByteArray &byteArray,QByteArray &value)
{
    byteArray.append(value);
}

void Modbus::Appenduint8BigEndian(QByteArray &byteArray,quint8 value)
{
    byteArray.append(static_cast<char>(value & 0xFF));
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

MOEM::MOEM()
{
    moemsend.head=0xAB;
    moemsend.idex='0';
    moemsend.end=0xAC;
    moemresult.head=0xBB;
    moemresult.end=0xBC;
}
MOEM::~MOEM()
{

}

QByteArray MOEM::MoemSenddataConfig(quint8 addr,quint8 group_ID,char frametype,const QByteArray &scriptbuffer)
{
    QByteArray data;
    moemsend.addr=addr;
    moemsend.group_ID=group_ID;
    moemsend.frametype=frametype;
    moemsend.scriptbuffer=scriptbuffer;
    data.append(moemsend.head);
    data.append((moemsend.addr&0x0F)|((moemsend.group_ID<<4)&0xF0));
    data.append(moemsend.idex);
    data.append(moemsend.frametype);
    data.append(moemsend.scriptbuffer);
    data.append(moemsend.end);
    moemsend.crc16=calculateCRC16(data);
    data.append(moemsend.crc16>>8&0xFF);
    data.append(moemsend.crc16&0xFF);
    moemsend.idex++;
    if(moemsend.idex >'9')
    {
        moemsend.idex='0';
    }
    return data;
}

MOEM::MOEMSend MOEM::GetMOEMSendStu()
{
    return moemsend;
}

quint16 calculateCRC16(const QByteArray &data, quint16 polynomial)
{
    quint16 crc = 0xFFFF;
    for (int i = 0; i < data.size(); ++i)
    {
        crc ^= (quint8)data.at(i);

        for (int j = 0; j < 8; ++j)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= polynomial;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

MOEM::MOEMResult MOEM::ResponseDataConfig(const QByteArray &data)
{
    MOEM::MOEMResult tmp;
    int headidex = -1;
    int endidex = -1;
    if (data.size() < 8)
    {
        return tmp;
    }
    for (int i = 0; i < data.size(); ++i)
    {
        if (static_cast<quint8>(data[i]) == 0xBB) // 找到帧头
        {
            headidex = i;
        }
        if (static_cast<quint8>(data[i]) == 0xBC) // 找到帧尾
        {
            endidex = i;
            break;
        }
    }
    if((headidex < 0)||(endidex<0)||(headidex > endidex)||((data.size()-endidex-1) < 2))
    {
        return tmp;
    }
    tmp.head=static_cast<quint8>(data[headidex]);
    tmp.addr=static_cast<quint8>(data[headidex+1])&0xF0;
    tmp.group_ID=static_cast<quint8>(data[headidex+1])&0x0F;
    tmp.idex=static_cast<char>(data[headidex+2]);
    tmp.frametype=static_cast<char>(data[headidex+3]);
    tmp.madpstatus=static_cast<char>(data[headidex+4]);
    tmp.responsestring=QString::fromLatin1(data.mid(headidex+5,endidex-headidex-5));
    tmp.end=static_cast<quint8>(data[endidex]);
    tmp.crc16=static_cast<quint8>(data[endidex+1] << 8) | static_cast<quint8>(data[endidex+2]);
    return tmp;
}
