import time

from madp_dll_repackage import *

if __name__ == '__main__':
    Sys_CAN_Open(dev_usbcan2)
    # Sys_Open_Port('com36', 38400)
    # RW_Register(1, 'M4,0|m4,1')
    # Cmd_E(2, '1-2It500|1-2Ia100000,10', False)
    # time.sleep(2)
    # Cmd_T()

    # Sys_CAN_Open(dev_usbcan2)
    Sys_Config(True, False, False, False, True, True)
    test_lst = [
        (0, '1-2It500,100,0|41-42Zz50000', True),
        (15, '1-2It500,100,0|41-42Zz50000', True),
        (0, '1-2It500,100,0|41-42Zz50000', False),
        (0, '1-2It500,100,0|41-42Zz50000', True),
    ]
    for i in range(len(test_lst)):
        re=(Cmd_E(*test_lst[i][0:]))
        print(re)
