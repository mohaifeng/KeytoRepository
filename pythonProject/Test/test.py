import time
import Com.Port.serialport as sp
import Adp.pipette as pip


def Get_Height():
    open_cmd = '30 3E 56 63 31 0D'
    close_cmd = '30 3E 56 63 30 0D'

    bal = sp.ser
    bal.PortSend(bytes.fromhex(open_cmd))
    time.sleep(1)
    bal.PortSend(bytes.fromhex(close_cmd))
    time.sleep(1)


if __name__ == '__main__':
    sp.Reset_Ser_Baud(0, 'com48', 38400)
    adp = pip.pipette()
    adp.address = 4
    adp.Transmit(adp.adp_cmd.It(64000, 100, 2))
    adp.Wait_Rx_Finish()
    adp.Wait_StatusIdle()
    time.sleep(3)
    adp.Transmit(adp.adp_cmd.Ia(15000, 300))
    adp.Wait_Rx_Finish()
    adp.Wait_StatusIdle()
    time.sleep(3)
    for i in range(0, 60):
        adp.Transmit(adp.adp_cmd.Ia(100, 300))
        adp.Wait_Rx_Finish()
        adp.Wait_StatusIdle()
        time.sleep(1)
        Get_Height()
        time.sleep(1)
