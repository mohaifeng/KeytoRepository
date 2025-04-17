from binascii import a2b_hex
import Com.Port.serialport as sp


def GetWeight():
    ba = sp.bal
    fin_data = ''
    ba.PortClean()
    while True:
        rec_data = ba.PortReceive_byte(5000)
        if rec_data:
            rec_data = bytes.fromhex(rec_data).hex().upper()
            if rec_data == '0A':
                fin_data += rec_data
                fin_data = fin_data.replace('0D0A', '')
                # print(fin_data)
                return fin_data
            else:
                fin_data += rec_data
                continue


def GetStableWeight():
    count = 0
    while True:
        weight_hex = GetWeight()
        if '3F' in weight_hex:
            count = 0
        else:
            weight_hex_2 = GetWeight()
            if weight_hex == weight_hex_2:
                count += 1
                if count == 5:
                    weight_int = float(a2b_hex(weight_hex_2).decode())
                    return weight_int
            else:
                count = 0
