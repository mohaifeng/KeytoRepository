import sys
import time
from madp_dll_repackage import *
import json

test_idex = 0
txt_path = r"test_result.txt"


def Test_Idex_Count(func):
    def wrapper(*args, **kwargs):
        global test_idex
        test_idex += 1
        with open(txt_path, "a", encoding="utf-8") as file:
            if test_idex == 1:
                file.write('\n')
                file.write('序号' + ' ' * 16)
                file.write('测试对象' + ' ' * 16)
                file.write('是否通过' + ' ' * 16)
                file.write('返回字典' + ' ' * 16 + '返回数据' + ' ' * 16)
                file.write('期望字典' + ' ' * 16 + '期望数据')
            file.write('\n' + ' ' * (4 - len(str(test_idex)) % 4) + str(test_idex))  # indent 缩进
        return func(*args, **kwargs)

    return wrapper


@Test_Idex_Count
def Bool_Func_Test(func_result: tuple, _target_result):
    bool_flag = True
    with open(txt_path, "a", encoding="utf-8") as file:
        file.write(' ' * 4 + json.dumps(func_result[1], indent=None, ensure_ascii=False))
        if func_result[0] == _target_result:
            file.write(' ' * 4 + 'Pass')
        else:
            file.write(' ' * 4 + 'No Pass')
            bool_flag = False
        file.write(' ' * 4 + json.dumps(func_result[0], indent=None, ensure_ascii=False))
        file.write(' ' * 4 + json.dumps(_target_result, indent=None, ensure_ascii=False))
    return bool_flag


@Test_Idex_Count
def KeytoResult_Func_Test(func_result: tuple, _target_result, target_data):
    with open(txt_path, "a", encoding="utf-8") as file:
        file.write(' ' * 4 + json.dumps(func_result[2], indent=None, ensure_ascii=False))
        if (func_result[0] == _target_result) & (func_result[1] == target_data):
            file.write(' ' * 4 + 'Pass')
        else:
            file.write(' ' * 4 + 'No Pass')
        file.write(' ' * 4 + json.dumps(func_result[0], indent=None, ensure_ascii=False))
        file.write(' ' * 4 + json.dumps(func_result[1], indent=None, ensure_ascii=False))
        file.write(' ' * 4 + json.dumps(_target_result, indent=None, ensure_ascii=False))
        file.write(' ' * 4 + json.dumps(target_data, indent=None, ensure_ascii=False))


@Test_Idex_Count
def Madp_Ack_Func_Test(func_result: tuple, _target_result):
    with open(txt_path, "a", encoding="utf-8") as file:
        file.write(' ' * 4 + json.dumps(func_result[1], indent=None, ensure_ascii=False))
        if func_result[0] == _target_result:
            file.write(' ' * 4 + 'Pass')
        else:
            file.write(' ' * 4 + 'No Pass')
        file.write(' ' * 4 + json.dumps(func_result[0], indent=None, ensure_ascii=False))
        file.write(' ' * 4 + json.dumps(_target_result, indent=None, ensure_ascii=False))


if __name__ == '__main__':
    Sys_Config(True, False, False, False, True, True)
    print('测试开始')
    # 串口测试
    test_com = int(input('请确认测试接口(0：RS485 1：RS232 2：CAN 3:TCP)：'))
    match test_com:
        case 0:
            target_result = True
            com = 'com' + input('串口号(eg:36): ')
            baud = int(input('波特率: '))
            if not Bool_Func_Test(Sys_Open_Port(com, baud), target_result):
                sys.exit()
            Bool_Func_Test(Sys_Close_Port(), target_result)
            Sys_Open_Port(com, baud)
        case 1:
            target_result = True
            com = 'com' + input('串口号(eg:36): ')
            baud = int(input('波特率: '))
            if not Bool_Func_Test(Sys_Open_Port(com, baud), target_result):
                sys.exit()
            Bool_Func_Test(Sys_Close_Port(), target_result)
            Sys_Open_Port(com, baud)
        case 2:
            target_result = True
            dev_num = int(input('设备号:'))
            can_com = int(input('端口号:'))
            can_baud = int(input('波特率: '))
            if not Bool_Func_Test(Sys_CAN_Open(dev_usbcan2, dev_num, can_com, can_baud), target_result):
                sys.exit()
            Bool_Func_Test(Sys_CAN_Close(), target_result)
            Sys_CAN_Open(dev_usbcan2, dev_num, can_com, can_baud)
        case 3:
            target_result = True
            IP_addr = input('IP地址: ')
            com = int(input('端口号:'))
            if not Bool_Func_Test(Sys_TCP_Connect(IP_addr, com), target_result):
                sys.exit()
            Bool_Func_Test(Sys_TCP_Disconnect(), target_result)
while True:
    cmd_idex = int(input('请输入测试序号(输入0结束测试)：'))
    match cmd_idex:
        case 0:
            print('测试结束')
            sys.exit()
        case 1:
            test_str1 = 'm0,4'
            test_str2 = '1-2It500,100,0|41-42Zz50000'
            test1_target_result = {"isRes": True, "add": 170, "group": 0, "seq": 0, "cmd": "k", "state": 0,
                                   "msg": "9600,38400,500,250718100"}
            test2_target_result = {"isRes": True, "add": 170, "group": 0, "seq": 0, "cmd": "E", "state": 1, "msg": ""}
            Madp_Ack_Func_Test(Cmd_Transmit_wait_ack(0, 'k', test_str1), test1_target_result)
            Madp_Ack_Func_Test(Cmd_Transmit_wait_ack(0, 'E', test_str2), test2_target_result)
            Wait_Cmd_Finish(0)
        case 2:
            test1_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,2:0,41:0,42:0"}
            test1_target_data = None
            KeytoResult_Func_Test(Cmd_E(0, '1-2It500,100,0|41-42Zz50000', True), test1_target_result, test1_target_data)
        case 3:
            test1_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,41:0"}
            test1_target_data = None
            Cmd_Transmit_wait_ack(0, 'E', '1It500,100,0|41Zz50000')
            KeytoResult_Func_Test(Wait_Cmd_Finish(0), test1_target_result, test1_target_data)
            KeytoResult_Func_Test(Wait_Cmd_Finish(0), test1_target_result, test1_target_data)
        case 4:
            test1_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": ""}
            test2_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "9600"}
            test3_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": ""}
            test4_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "20,20"}
            test5_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": ""}
            test6_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "30,30"}
            test7_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": ""}
            test8_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1,1"}
            test1_target_data = []
            test2_target_data = [9600]
            test3_target_data = []
            test4_target_data = [20,20]
            test5_target_data = []
            test6_target_data = [30,20]
            test7_target_data = []
            test8_target_data = [1,1]
            KeytoResult_Func_Test(RW_Register(0, 'M0,9600'), test1_target_result, test1_target_data)
            KeytoResult_Func_Test(RW_Register(0, 'm0,1'), test2_target_result, test2_target_data)
            KeytoResult_Func_Test(RW_Register(0, '1-2D0X2000,54,20'), test3_target_result, test3_target_data)
            KeytoResult_Func_Test(RW_Register(0, '1-2d0X2000,54'), test4_target_result, test4_target_data)
            KeytoResult_Func_Test(RW_Register(0, '1-2Wr54,30'), test5_target_result, test5_target_data)
            KeytoResult_Func_Test(RW_Register(0, '1-2Rr54'), test6_target_result, test6_target_data)
            KeytoResult_Func_Test(RW_Register(0, '1-2Wp2,0'), test7_target_result, test7_target_data)
            KeytoResult_Func_Test(RW_Register(0, '1-2Rp5'), test8_target_result, test8_target_data)
        case 5:
            test1_target_result = {"isOk": True, "isFinish": False, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "None"}
            test1_target_data = {}
            Cmd_Transmit_wait_ack(0, 'E', '1It500,100,0|41Zz50000')
            KeytoResult_Func_Test(Check_SysStatus(0), test1_target_result, test1_target_data)
            Wait_Cmd_Finish(0)
        case 6:
            test1_target_result = {"isOk": True, "isFinish": False, "errCode": 7, "errNodeCode": {},
                                   "errMsg": "runing node error:1:22", "msg": "runing node error:1:22"}
            test1_target_data = None
            Cmd_Transmit_wait_ack(0, 'E', '1Ld0,1000')
            time.sleep(2)
            KeytoResult_Func_Test(Get_Error_Msg(0), test1_target_result, test1_target_data)
        case 7:
            test1_target_result = {"isOk": True, "isFinish": False, "errCode": 0, "errNodeCode": {}, "errMsg": "None",
                                   "msg": ""}
            test1_target_data = None
            KeytoResult_Func_Test(Cmd_T(), test1_target_result, test1_target_data)
        case 8:
            test1_target_result = {"isOk": True, "isFinish": False, "errCode": 0, "errNodeCode": {}, "errMsg": "None",
                                   "msg": ""}
            test1_target_data = None
            KeytoResult_Func_Test(Cmd_t(0), test1_target_result, test1_target_data)
        case 9:
            test1_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "41:0,42:0"}
            test2_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,2:0"}
            test3_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "41:0,42:0"}
            test4_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,41:0,42:0,2:0"}
            test5_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,41:0,42:0,2:0"}
            test6_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,41:0,42:0,2:0"}
            test_target_data = None

            KeytoResult_Func_Test(Group_Z_Init(0, [41, 42], True), test1_target_result, test_target_data)
            KeytoResult_Func_Test(Group_Adp_Init(0, [1, 2], True), test2_target_result, test_target_data)
            KeytoResult_Func_Test(Mul_PickUpTip(0, [1, 2], True, 50, 80, 180), test3_target_result,
                                  test_target_data)
            KeytoResult_Func_Test(Mul_LldAspirateFollow(0, [1, 2], True, 200, 200, 180, 0, 20, 0, 30, 80, 78),
                                  test4_target_result, test_target_data)
            KeytoResult_Func_Test(Mul_DispenseFollow(0, [1, 2], True, 100, 200, 50, 180, 78), test5_target_result,
                                  test_target_data)
            KeytoResult_Func_Test(Mul_DispenseEmpty(0, [1, 2], True, 200, 0, 180), test6_target_result,
                                  test_target_data)
        case 10:
            test1_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,2:0"}
            test2_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,2:0"}
            test3_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,2:0"}
            test4_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,2:0"}
            test5_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,2:0"}
            test6_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "1:0,2:0"}
            test_target_data = None
            KeytoResult_Func_Test(Group_Adp_Init(0, [1, 2], True), test1_target_result, test_target_data)
            KeytoResult_Func_Test(Group_Adp_Aspirate(0, [1, 2], True, 100), test2_target_result, test_target_data)
            KeytoResult_Func_Test(Group_Adp_Plld(0, [1, 2], True), test3_target_result, test_target_data)
            KeytoResult_Func_Test(Group_Adp_Dispense(0, [1, 2], True, 50), test4_target_result, test_target_data)
            KeytoResult_Func_Test(Group_Adp_Move(0, [1, 2], True, 500), test5_target_result, test_target_data)
            KeytoResult_Func_Test(Group_Adp_PushTip(0, [1, 2], True), test6_target_result, test_target_data)
        case 11:
            test1_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "41:0,42:0"}
            test2_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "41:0,42:0"}
            test3_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "41:0,42:0"}
            test4_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "41:0,42:0"}
            test5_target_result = {"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "",
                                   "msg": "41:0,42:0"}
            test_target_data = None
            KeytoResult_Func_Test(Group_Z_Init(0, [41, 42], True), test1_target_result, test_target_data)
            KeytoResult_Func_Test(Group_Z_MovePos(0, [41, 42], True, 50), test2_target_result, test_target_data)
            KeytoResult_Func_Test(Group_Z_PickUpTip(0, [41, 42], True), test3_target_result, test_target_data)
            KeytoResult_Func_Test(Group_Z_MoveUp(0, [41, 42], True, 10), test4_target_result, test_target_data)
            KeytoResult_Func_Test(Group_Z_MoveDown(0, [41, 42], True, 10), test5_target_result, test_target_data)
