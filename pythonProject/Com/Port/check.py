from binascii import crc32


def Xor_Checksum_8_Bit(data: str):
    """
    计算8位异或校验和
    :param data: 字符串
    :return: 8位字符串
    """
    data = bytes.fromhex(data.replace(' ', ''))
    checksum = 0
    for byte in data:
        checksum ^= byte
    check_sum = hex(checksum & 0xFF).replace('0x', '').zfill(2).upper()
    return check_sum  # 确保返回的是8位值


def Uchar_Checksum_8_Bit(tmp: str):
    """
    8位和校验
    :param tmp: 校验字符串：str
    :return: 十六进制和校验（1byte）：str
    """
    tmp = bytes.fromhex(tmp)
    length = len(tmp)
    checksum = 0
    for t in range(0, length):
        checksum += int.from_bytes(tmp[t:t + 1], "little", signed=False)
        checksum &= 0xFF  # 强制截断
    check_sum = hex(checksum).replace('0x', '').zfill(2).upper()
    return check_sum


def Uchar_Checksum_16_Bit(tmp: str):
    """
    16位和校验
    :param tmp: 校验字符串：str
    :return: 十六进制和校验（1word）：str
    """
    tmp = bytes.fromhex(tmp)
    length = len(tmp)
    checksum = 0
    for t in range(0, length):
        checksum += int.from_bytes(tmp[t:t + 1], "little", signed=False)
        checksum &= 0xFFFF  # 强制截断
    check_sum = hex(checksum).replace('0x', '').zfill(4).upper()
    return check_sum


def Uchar_Checksum_32_Bit(tmp: str):
    """
    32位和校验
    :param tmp: 校验字符串：str
    :return: 十六进制和校验（4byte）：str
    """
    tmp = bytes.fromhex(tmp)
    length = len(tmp)
    checksum = 0
    for t in range(0, length):
        checksum += int.from_bytes(tmp[t:t + 1], "little", signed=False)
        checksum &= 0xFFFFFFFF  # 强制截断
    check_sum = hex(checksum).replace('0x', '').zfill(8).upper()
    return check_sum


# CRC16校验
def CRC_16(string):
    """
    CRC16校验
    :param string: 数据字符串
    :return: 校验字符串
    """
    data = bytearray.fromhex(string)
    crc = 0xFFFF
    for pos in data:
        crc ^= pos
        for c in range(8):
            if crc & 0x0001:
                crc >>= 1
                crc ^= 0xA001
            else:
                crc >>= 1
    crc16 = hex(crc & 0xFFFF).replace('0x', '').zfill(4).upper()
    return crc16


def CRC16_MODBUS(string: str):
    data = bytearray.fromhex(string)
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc >>= 1
                crc ^= 0xA001
            else:
                crc >>= 1
    crc16_modbus = hex(crc & 0xFFFF).replace('0x', '').zfill(4).upper()
    fin_data = ''
    for i in range(0, len(crc16_modbus) // 2 + 1, 2):
        tmp = crc16_modbus[i:i + 2]
        fin_data = tmp + fin_data
    return fin_data


def CRC_8(string: str):
    """
    CRC8校验
    :param string: 数据字符串
    :return: 校验字符串
    """
    data = bytearray.fromhex(string)
    crc = 0
    for pos in data:
        crc ^= pos
        for c in range(8):
            if crc & 0x80:
                crc = (crc << 1) ^ 0x31
            else:
                crc <<= 1
    crc8 = hex(crc & 0xFF).replace('0x', '').zfill(2).upper()
    return crc8


def CRC_32(data: str):
    """
    计算字符串或字节流的CRC32校验和。
    :param data: 字符串或字节流
    :return: 计算得到的CRC32校验和
    """
    data_hex = bytearray.fromhex(data)
    return hex(crc32(data_hex) & 0xffffffff).replace('0x', '').zfill(8).upper()


if __name__ == '__main__':
    print(Xor_Checksum_8_Bit('02 30 5A 52 03'))
