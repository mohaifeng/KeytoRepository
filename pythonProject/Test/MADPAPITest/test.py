import time

from madp_dll_repackage import *

if __name__ == '__main__':
    # Sys_CAN_Open(dev_usbcan2)
    # Sys_Open_Port('com36', 38400)
    # RW_Register(1, 'M4,0|m4,1')
    # Cmd_E(2, '1-2It500|1-2Ia100000,10', False)
    # time.sleep(2)
    # Cmd_T()

    Sys_CAN_Open(dev_usbcan2)
    Sys_Config(True, False, False, False, True, False)
    test_lst = [
        (1, 'E', '1-2It500|1-2Ia100000,10'),
        (1, 'E', '1-2It500|1-2Ia100000,10'),
        (1, 'E', '1-2It500|1-2Ia100000,10'),
        (0, 'E', '1-2Ld1,1000'),
    ]
    for i in range(len(test_lst)):
        Cmd_Transmit_wait_ack(*test_lst[i][0:])
        time.sleep(2)
        match i:
            case 0:
                Cmd_t(test_lst[i][0])
                result = RW_Register(test_lst[i][0], '1-2Rr1')
            case 1:
                Cmd_t(2)
                result = RW_Register(test_lst[i][0], '1-2Rr1')
                Cmd_t(test_lst[i][0])
            case 2:
                Cmd_t(test_lst[i][0], 1)
                result = RW_Register(test_lst[i][0], '1-2Rr1')
                Cmd_t(test_lst[i][0])
            case 3:
                Wait_Cmd_Finish(test_lst[i][0])
                Cmd_t(test_lst[i][0])
                result = Check_SysStatus(test_lst[i][0])
        print(result)
