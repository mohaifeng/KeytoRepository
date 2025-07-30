import sys
import time
from madp_dll_repackage import *
import json

test_idex = 0
txt_path = r"test_result.txt"


def Bool_Func_Test(func_result: tuple, _target_result):
    global test_idex
    test_idex += 1
    bool_flag = True
    with open(txt_path, "a", encoding="utf-8") as file:
        file.write('序号: ' + str(test_idex) + '\n')
        file.write('测试对象: ' + str(func_result[1]) + '\n')
        file.write('返回数据: ' + str(func_result[0]) + '\n')
        file.write('期望数据: ' + str(_target_result) + '\n')
        if func_result[0] == _target_result:
            file.write('是否通过: ' + 'Pass')
        else:
            file.write('是否通过: ' + 'Failed')
            bool_flag = False
    return bool_flag


def KeytoResult_Func_Test(func_result: tuple, _target_result, target_data):
    global test_idex
    test_idex += 1
    with open(txt_path, "a", encoding="utf-8") as file:
        file.write('序号: ' + str(test_idex) + '\n')
        file.write('测试对象: ' + json.dumps(func_result[2], indent=None, ensure_ascii=False))
        file.write('返回字典: ' + json.dumps(func_result[0], indent=None, ensure_ascii=False))
        file.write('返回数据: ' + json.dumps(func_result[1], indent=None, ensure_ascii=False))
        file.write('期望字典: ' + json.dumps(_target_result, indent=None, ensure_ascii=False))
        file.write('期望数据: ' + json.dumps(target_data, indent=None, ensure_ascii=False))
        if (func_result[0] == _target_result) & (func_result[1] == target_data):
            file.write('是否通过: ' + 'Pass')
        else:
            file.write('是否通过: ' + 'Failed')


def Madp_Ack_Func_Test(func_result: tuple, _target_result):
    global test_idex
    test_idex += 1
    with open(txt_path, "a", encoding="utf-8") as file:
        file.write('序号: ' + str(test_idex) + '\n')
        file.write('测试对象: ' + json.dumps(func_result[1], indent=None, ensure_ascii=False))
        file.write('返回数据: ' + json.dumps(func_result[0], indent=None, ensure_ascii=False))
        file.write('期望数据: ' + json.dumps(_target_result, indent=None, ensure_ascii=False))
        del func_result[0]['seq']
        del _target_result['seq']
        if func_result[0] == _target_result:
            file.write('是否通过: ' + 'Pass')
        else:
            file.write('是否通过: ' + 'Failed')


def Func_Serial_Test(_com: str, _baudrate: int):
    bool_target_result = True
    # 串口通讯测试
    if not Bool_Func_Test(Sys_Open_Port(_com, _baudrate), bool_target_result):
        sys.exit()
    Bool_Func_Test(Sys_Close_Port(), bool_target_result)
    Sys_Open_Port(_com, _baudrate)


def Func_CAN_Test(_dev_num: int, _can_com: int, _baudrate: int):
    bool_target_result = True
    # 串口通讯测试
    if not Bool_Func_Test(Sys_CAN_Open(dev_usbcan2, _dev_num, _can_com, _baudrate), bool_target_result):
        sys.exit()
    Bool_Func_Test(Sys_CAN_Close(), bool_target_result)
    Sys_CAN_Open(dev_usbcan2, _dev_num, _can_com, _baudrate)


def Func_TCP_Test(_IP_addr: str, _com: int):
    bool_target_result = True
    # 串口通讯测试
    if not Bool_Func_Test(Sys_TCP_Connect(_IP_addr, _com), bool_target_result):
        sys.exit()
    Bool_Func_Test(Sys_TCP_Disconnect(), bool_target_result)
    Sys_TCP_Connect(_IP_addr, _com)


def Func_StringCommandSendWaitAck_Test():
    test_lst = [
        (0, 'k', 'm0,3'),
        (1, 'E', '1-2It500,100,0|41-42Zz50000'),
        (1, 'E', '41It500,100,0'),
        (1, 'q', ''),
        (1, 'i', ''),
        (2, 'l', ''),
    ]
    test_target_result = [
        {'isRes': True, 'add': 0, 'group': 0, 'seq': 0, 'cmd': 'k', 'state': 0, 'msg': '38400,38400,500'},
        {'isRes': True, 'add': 0, 'group': 1, 'seq': 0, 'cmd': 'E', 'state': 1, 'msg': ''},
        {'isRes': True, 'add': 0, 'group': 1, 'seq': 0, 'cmd': 'E', 'state': 1, 'msg': ''},
        {'isRes': True, 'add': 0, 'group': 1, 'seq': 0, 'cmd': 'q', 'state': 7, 'msg': '41:14'},
        {'isRes': True, 'add': 0, 'group': 1, 'seq': 0, 'cmd': 'i', 'state': 7,
         'msg': "id:41 write dic:4000.2 error code:14 cmd string:'41It500,100,0'"},
        {'isRes': True, 'add': 0, 'group': 2, 'seq': 0, 'cmd': 'l', 'state': 2, 'msg': ''},
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_StringCommandSendWaitAck_Test：测试用例有误')
        sys.exit()
    for i in range(len(test_lst)):
        Madp_Ack_Func_Test(Cmd_Transmit_wait_ack(*test_lst[i][0:]), test_target_result[i])
        if test_lst[i][1] == 'E':
            Wait_Cmd_Finish(0)


def Func_RunActionScript_Test():
    test_lst = [
        (0, '1-2It500,100,0|41-42Zz50000', True),
        (0, '1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|', True),
        (1, '1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
            '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|', True),

    ]
    test_target_result = [
        ({"isOk": True, "isFinish": True, "errCode": 0, "errNodeCode": {}, "errMsg": "", "msg": "1:0,2:0,41:0,42:0"},
         None),
        ({'isOk': False, 'isFinish': False, 'errCode': 5, 'errNodeCode': {}, 'errMsg': 'stript lenght 4102 error',
          'msg': ''}, None),
        ({'isOk': False, 'isFinish': False, 'errCode': 5, 'errNodeCode': {}, 'errMsg': 'stript lenght 259 error',
          'msg': ''}, None),
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_RunActionScript_Test：测试用例有误')
        sys.exit()
    for i in range(len(test_lst)):
        KeytoResult_Func_Test(Cmd_E(*test_lst[i][0:]), test_target_result[i][0], test_target_result[i][1])
        if not test_lst[i][2]:
            Wait_Cmd_Finish(test_lst[i][0])


def Func_WaitActionFinish_Test():
    test_lst = [
        (0, 'E', '1-2It500,100,0|41-42Zz50000'),
        (1, 'E', '1-2Ld1,1000'),
    ]
    test_target_result = [
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0,41:0,42:0'},
         None),
        ({'isOk': False, 'isFinish': False, 'errCode': 7, 'errNodeCode': {1: 22, 2: 1},
          'errMsg': 'runing node error:1:22', 'msg': '1:22,2:1'}, None),
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_WaitActionFinish_Test：测试用例有误')
        sys.exit()
    Sys_Config(True, False, False, False, True, True)
    for i in range(len(test_lst)):
        Cmd_Transmit_wait_ack(*test_lst[i][0:])
        KeytoResult_Func_Test(Wait_Cmd_Finish(test_lst[i][0]), test_target_result[i][0], test_target_result[i][1])
    Sys_Config(True, False, False, False, True, True)
    for i in range(len(test_lst)):
        Cmd_Transmit_wait_ack(*test_lst[i][0:])
        KeytoResult_Func_Test(Wait_Cmd_Finish(test_lst[i][0]), test_target_result[i][0], test_target_result[i][1])


def Func_ReadWriteReg_Test():
    test_lst = [
        (0, 'M0,9600'),
        (0, 'm0,1'),
        (0, '1-2D0X2000,54,20'),
        (0, '1-2d0X2000,54'),
        (0, '1-2Wr54,30'),
        (0, '1-2Rr54'),
        (0, '1-2Wp2,0'),
        (0, '1-2Rp5'),
        (0, 'aabbccdd'),
    ]
    test_target_result = [
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': ''}, []),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '9600'}, [9600]),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': ''}, []),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '20,20'}, [20, 20]),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': ''}, []),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '30,30'}, [30, 30]),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': ''}, []),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1,1'}, [1, 1]),
        ({'isOk': False, 'isFinish': False, 'errCode': 6, 'errNodeCode': {}, 'errMsg': 'no script cmd:aabbccdd',
          'msg': 'None'}, []),
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_ReadWriteReg_Test：测试用例有误')
        sys.exit()
    for i in range(len(test_lst)):
        KeytoResult_Func_Test(RW_Register(*test_lst[i][0:]), test_target_result[i][0],
                              test_target_result[i][1])


def Func_QueryRunStatus_Test():
    test_lst = [
        (0, 'E', '1-2It500,100,0'),
        (0, 'E', '1-2It500,100,0', 1, 2),
        (1, 'E',
         '1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
         '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500'
         '|1It500|1It500|1It500|1It500|1It500|1It500|1It500|1It500|'),
        (0, 'E', '1-2Ld1,1000', 1, 2),
        (0, 'E', '1-2B'),
    ]
    test_target_result = [
        ({'isOk': True, 'isFinish': False, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': 'None'}, {}),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0'},
         {1: 0, 2: 0}),
        ({'isOk': False, 'isFinish': False, 'errCode': 5, 'errNodeCode': {}, 'errMsg': 'stript lenght 259 error',
          'msg': '1:0,2:0'}, {1: 0, 2: 0}),
        ({'isOk': False, 'isFinish': False, 'errCode': 7, 'errNodeCode': {1: 22, 2: 1},
          'errMsg': 'runing node error:1:22', 'msg': '1:22,2:1'}, {1: 22, 2: 1}),
        ({'isOk': False, 'isFinish': False, 'errCode': 7, 'errNodeCode': {}, 'errMsg': 'no script cmd:1-2B', 'msg': ''},
         {}),
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_QueryRunStatus_Test：测试用例有误')
        sys.exit()
    for i in range(len(test_lst)):
        Cmd_Transmit_wait_ack(*test_lst[i][0:3])
        match i:
            case 0:
                KeytoResult_Func_Test(Check_SysStatus(test_lst[i][0]), test_target_result[i][0],
                                      test_target_result[i][1])
                Wait_Cmd_Finish(test_lst[i][0])
            case 1:
                Wait_Cmd_Finish(test_lst[i][0])
                KeytoResult_Func_Test(Check_SysStatus(test_lst[i][0], test_lst[i][3], test_lst[i][4]),
                                      test_target_result[i][0], test_target_result[i][1])
            case 3:
                Wait_Cmd_Finish(test_lst[i][0])
                KeytoResult_Func_Test(Check_SysStatus(test_lst[i][0], test_lst[i][3], test_lst[i][4]),
                                      test_target_result[i][0], test_target_result[i][1])
            case _:
                KeytoResult_Func_Test(Check_SysStatus(test_lst[i][0]), test_target_result[i][0],
                                      test_target_result[i][1])


def Func_GetErrInfo_Test():
    test_lst = [
        (0, 'E', '1-2It500'),
        (0, 'E', '1-2Ld1,1000'),
    ]
    test_target_result = [
        ({'isOk': True, 'isFinish': False, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': ''}, None),
        ({'isOk': True, 'isFinish': False, 'errCode': 7, 'errNodeCode': {}, 'errMsg': 'runing node error:1:22',
          'msg': 'runing node error:1:22'}, None),
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_GetErrInfo_Test：测试用例有误')
        sys.exit()
    for i in range(len(test_lst)):
        Cmd_Transmit_wait_ack(*test_lst[i][0:])
        Wait_Cmd_Finish(test_lst[i][0])
        KeytoResult_Func_Test(Get_Error_Msg(test_lst[i][0]), test_target_result[i][0], test_target_result[i][1])


def Func_EmergencyStop_Test():
    test_lst = [
        (0, 'E', '1-2It500|1-2Ia100000,10'),
        (0, 'E', '1-2Ld1,1000'),
    ]
    test_target_result = [
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '0,0'}, [0, 0]),
        ({'isOk': False, 'isFinish': False, 'errCode': 7, 'errNodeCode': {1: 22}, 'errMsg': 'runing node error:1:22',
          'msg': '1:22,2:0'}, {1: 22, 2: 0}),
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_EmergencyStop_Test：测试用例有误')
        sys.exit()
    for i in range(len(test_lst)):
        Cmd_Transmit_wait_ack(*test_lst[i][0:])
        time.sleep(2)
        Cmd_T()
        match i:
            case 0:
                KeytoResult_Func_Test(RW_Register(test_lst[i][0], '1-2Rr1'), test_target_result[i][0],
                                      test_target_result[i][1])
            case 1:
                KeytoResult_Func_Test(Check_SysStatus(test_lst[i][0]), test_target_result[i][0],
                                      test_target_result[i][1])


def Func_Stop_Test():
    test_lst = [
        (1, 'E', '1-2It500|1-2Ia100000,10'),
        (1, 'E', '1-2It500|1-2Ia100000,10'),
        (1, 'E', '1-2It500|1-2Ia100000,10'),
        (0, 'E', '1-2Ld1,1000'),
    ]
    test_target_result = [
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '0,0'}, [0, 0]),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1,1'}, [1, 1]),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '0,1'}, [0, 1]),
        ({'isOk': False, 'isFinish': False, 'errCode': 7, 'errNodeCode': {1: 22}, 'errMsg': 'runing node error:1:22',
          'msg': '1:22,2:0'}, {1: 22, 2: 0}),
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_Stop_Test：测试用例有误')
        sys.exit()

    for i in range(len(test_lst)):
        Cmd_Transmit_wait_ack(*test_lst[i][0:])
        time.sleep(2)
        match i:
            case 0:
                Cmd_t(test_lst[i][0])
                KeytoResult_Func_Test(RW_Register(test_lst[i][0], '1-2Rr1'), test_target_result[i][0],
                                      test_target_result[i][1])
            case 1:
                Cmd_t(2)
                KeytoResult_Func_Test(RW_Register(test_lst[i][0], '1-2Rr1'), test_target_result[i][0],
                                      test_target_result[i][1])
                Cmd_t(test_lst[i][0])
            case 2:
                Cmd_t(test_lst[i][0], 1)
                KeytoResult_Func_Test(RW_Register(test_lst[i][0], '1-2Rr1'), test_target_result[i][0],
                                      test_target_result[i][1])
                Cmd_t(test_lst[i][0])
            case 3:
                Wait_Cmd_Finish(test_lst[i][0])
                Cmd_t(test_lst[i][0])
                KeytoResult_Func_Test(Check_SysStatus(test_lst[i][0]), test_target_result[i][0],
                                      test_target_result[i][1])


def Func_MultiControl_Test():
    test_lst = [
        (Group_Z_Init, 0, [41, 42], True),
        (Group_Adp_Init, 0, [1, 2], True),
        (Mul_PickUpTip, 0, [1, 2], True, 50, 80, 180),
        (Mul_LldAspirateFollow, 0, [1, 2], True, 200, 200, 180, 0, 20, 0, 30, 80, 78),
        (Mul_DispenseFollow, 0, [1, 2], True, 100, 200, 50, 180, 78),
        (Mul_DispenseEmpty, 0, [1, 2], True, 200, 0, 180),
    ]
    test_target_result = [
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '41:0,42:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '41:0,42:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0,41:0,42:0'},
         None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0,41:0,42:0'},
         None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0,41:0,42:0'},
         None),
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_MultiControl_Test：测试用例有误')
        sys.exit()
    Sys_Config(True, False, False, False, True, False)
    for i in range(len(test_lst)):
        KeytoResult_Func_Test(test_lst[i][0](*test_lst[i][1:]), test_target_result[i][0], test_target_result[i][1])
    Sys_Config(True, False, False, False, True, True)
    for i in range(len(test_lst)):
        KeytoResult_Func_Test(test_lst[i][0](*test_lst[i][1:]), test_target_result[i][0], test_target_result[i][1])


def Func_GroupAdp_Test():
    test_lst = [
        (Group_Adp_Init, 0, [1, 2], True),
        (Group_Adp_Aspirate, 0, [1, 2], True, 100),
        (Group_Adp_Plld, 0, [1, 2], True),
        (Group_Adp_Dispense, 0, [1, 2], True, 50),
        (Group_Adp_Move, 0, [1, 2], True, 500),
        (Group_Adp_PushTip, 0, [1, 2], True),
    ]
    test_target_result = [
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '1:0,2:0'}, None),
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_GroupAdp_Test：测试用例有误')
        sys.exit()
    Sys_Config(True, False, False, False, True, False)
    for i in range(len(test_lst)):
        KeytoResult_Func_Test(test_lst[i][0](*test_lst[i][1:]), test_target_result[i][0], test_target_result[i][1])
    Sys_Config(True, False, False, False, True, True)
    for i in range(len(test_lst)):
        KeytoResult_Func_Test(test_lst[i][0](*test_lst[i][1:]), test_target_result[i][0], test_target_result[i][1])


def Func_GroupZ_Test():
    test_lst = [
        (Group_Z_Init, 0, [41, 42], True),
        (Group_Z_MovePos, 0, [41, 42], True, 50),
        (Group_Z_PickUpTip, 0, [41, 42], True),
        (Group_Z_MoveUp, 0, [41, 42], True, 100),
        (Group_Z_MoveDown, 0, [41, 42], True, 50),
        (Group_Z_Init, 0, [41, 42], True),
    ]
    test_target_result = [
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '41:0,42:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '41:0,42:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '41:0,42:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '41:0,42:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '41:0,42:0'}, None),
        ({'isOk': True, 'isFinish': True, 'errCode': 0, 'errNodeCode': {}, 'errMsg': '', 'msg': '41:0,42:0'}, None),
    ]
    if len(test_lst) != len(test_target_result):
        print('Func_GroupZ_Test：测试用例有误')
        sys.exit()
    Sys_Config(True, False, False, False, True, False)
    for i in range(len(test_lst)):
        KeytoResult_Func_Test(test_lst[i][0](*test_lst[i][1:]), test_target_result[i][0], test_target_result[i][1])
    Sys_Config(True, False, False, False, True, True)
    for i in range(len(test_lst)):
        KeytoResult_Func_Test(test_lst[i][0](*test_lst[i][1:]), test_target_result[i][0], test_target_result[i][1])


if __name__ == '__main__':
    test_fun_lst = [
        Func_StringCommandSendWaitAck_Test,
        Func_RunActionScript_Test,
        Func_WaitActionFinish_Test,
        Func_ReadWriteReg_Test,
        Func_QueryRunStatus_Test,
        Func_GetErrInfo_Test,
        Func_EmergencyStop_Test,
        Func_Stop_Test,
        Func_MultiControl_Test,
        Func_GroupAdp_Test,
        Func_GroupZ_Test
    ]
    print('串口测试开始')
    Func_Serial_Test('com36', 38400)
    Sys_Config(True, False, False, False, True, True)
    for fun in test_fun_lst:
        fun()
    Cmd_Transmit_wait_ack(0, 'E', 'RST123456')
    time.sleep(8)
    print('CAN测试开始')
    Func_CAN_Test(0, 0, 500)
    Sys_Config(True, False, False, False, True, True)
    for fun in test_fun_lst:
        fun()
