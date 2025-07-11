import sys
from madp_dll_repackage import *


def Count_Time(fun, *args):
    start_time = time.time()
    fun(*args)
    end_time = time.time()
    print('Count_Time: ', (end_time - start_time) * 1000)


def Handle_Func_Test(_result):
    if _result:
        print('测试通过')
    else:
        print('测试未通过')
        re = input('是否继续测试？(Y/N)')
        if re != 'Y':
            sys.exit()


adp_id_lst = []
z_id_lst = []

com = ''
baud = 0
dev_num, can_com, can_baud = 0, 0, 0

if __name__ == '__main__':
    Sys_Config(False, False, False, False)
    # 串口测试
    test_com = int(input('请确认测试接口(0：RS485 1：RS232 2：CAN 3:TCP)：'))
    match test_com:
        case 0:
            com = 'com' + input('串口号(eg:36): ')
            baud = int(input('波特率: '))
            Handle_Func_Test(Sys_Open_Port(com, baud))
            Handle_Func_Test(Sys_Close_Port())
            Sys_Open_Port(com, baud)
        case 1:
            com = 'com' + input('串口号(eg:36): ')
            baud = int(input('波特率: '))
            Handle_Func_Test(Sys_Open_Port(com, baud))
            Handle_Func_Test(Sys_Close_Port())
            Sys_Open_Port(com, baud)
        case 2:
            dev_num = int(input('设备号:'))
            can_com = int(input('端口号:'))
            can_baud = int(input('波特率: '))
            Handle_Func_Test(Sys_CAN_Open(dev_usbcan2, dev_num, can_com, can_baud))
            Handle_Func_Test(Sys_CAN_Close())
            Sys_CAN_Open(dev_usbcan2, dev_num, can_com, can_baud)
        case 3:
            IP_addr = input('IP地址: ')
            com = int(input('端口号:'))
            Handle_Func_Test(Sys_TCP_Connect(IP_addr, com))
            Handle_Func_Test(Sys_TCP_Disconnect())
    while True:
        cmd_idex = int(input('请输入测试序号(输入0结束测试)：'))
        match cmd_idex:
            case 0:
                print('测试结束')
                sys.exit()
            case 1:
                test_str1 = 'm0,4'
                test_str2 = '1It500,100,0|41Zz50000'
                Handle_Func_Test(Cmd_Transmit_wait_ack(0, 'k', test_str1))
                Handle_Func_Test(Cmd_Transmit_wait_ack(0, 'E', test_str2))
                Wait_Cmd_Finish(0)
            case 2:
                Handle_Func_Test(Cmd_E(0, '1It500,100,0|41Zz50000', True))
            case 3:
                Cmd_Transmit_wait_ack(0, 'E', '1It500,100,0|41Zz50000')
                Handle_Func_Test(Wait_Cmd_Finish(0))
                Handle_Func_Test(Wait_Cmd_Finish(0))
            case 4:
                Handle_Func_Test(RW_Register(0, 'M0,9600'))
                Handle_Func_Test(RW_Register(0, 'm0,1'))
                Handle_Func_Test(RW_Register(0, '1D0X2000,54,20'))
                Handle_Func_Test(RW_Register(0, '1d0X2000,54'))
                Handle_Func_Test(RW_Register(0, '1Wr54,30'))
                Handle_Func_Test(RW_Register(0, '1Rr54'))
                Handle_Func_Test(RW_Register(0, '1Wp2,0'))
                Handle_Func_Test(RW_Register(0, '1Rp5'))
            case 5:
                Cmd_Transmit_wait_ack(0, 'E', '1It500,100,0|41Zz50000')
                Handle_Func_Test(Check_SysStatus(0))
                Wait_Cmd_Finish(0)
            case 6:
                Cmd_Transmit_wait_ack(0, 'E', '1Ld0,1000')
                time.sleep(2)
                Handle_Func_Test(Get_Error_Msg(0))
            case 7:
                Handle_Func_Test(Cmd_T())
            case 8:
                Handle_Func_Test(Cmd_t(0))
            case 9:
                Handle_Func_Test(Group_Z_Init(0, [41, 42], True))
                Handle_Func_Test(Group_Adp_Init(0, [1, 2], True))
                Handle_Func_Test(Mul_PickUpTip(0, [1, 2], True, 50, 80, 180))
                Handle_Func_Test(Mul_LldAspirateFollow(0, [1, 2], True, 200, 200, 180, 0, 20, 0, 30, 80, 78))
                Handle_Func_Test(Mul_DispenseFollow(0, [1, 2], True, 100, 200, 50, 180, 78))
                Handle_Func_Test(Mul_DispenseEmpty(0, [1, 2], True, 200, 0, 180))
            case 10:
                Handle_Func_Test(Group_Adp_Init(0, [1, 2], True))
                Handle_Func_Test(Group_Adp_Aspirate(0, [1, 2], True, 100))
                Handle_Func_Test(Group_Adp_Plld(0, [1, 2], True))
                Handle_Func_Test(Group_Adp_Dispense(0, [1, 2], True, 50))
                Handle_Func_Test(Group_Adp_Move(0, [1, 2], True, 500))
                Handle_Func_Test(Group_Adp_PushTip(0, [1, 2], True))
            case 11:
                Group_Z_Init(0, [41, 42], True)
                Group_Z_MovePos(0, [41, 42], True, 50)
                Group_Z_PickUpTip(0, [41, 42], True)
                Group_Z_MoveUp(0, [41, 42], True, 10)
                Group_Z_MoveDown(0, [41, 42], True, 10)
