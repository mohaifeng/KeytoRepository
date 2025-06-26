import time

import clr
import sys
from System import Byte, Int32
from System.Collections.Generic import List, Dictionary

clr.AddReference('keytoAPI')
from keytoAPI.MADPV2 import API, Madp_NodeInfo
from keytoAPI.Driver.CAN import CAN_DeviceType

api = API()  # 创建 API 类的实例


def Sys_Open_Port(port: str, baudrate=38400):
    """
    打开串口
    :param port: 串口号
    :param baudrate: 波特率
    :return:
    """
    if not api.isSerialOpen():
        if api.OpenSerial(port, baudrate):
            pass
        else:
            print(f"Port open failed")
            sys.exit()
    print(f"Port opened success")


def Sys_Close_Port():
    """
    关闭串口
    :return:
    """
    if api.CloseSerial():
        print(f"Port closed success")
    else:
        print(f"Port closed failed")


def Cmd_Transmit_wait_ack(add: int, group: int, cmd: str, string: str, timeout=500):
    result = api.StringCommandSendWaitAck(Byte(add), Byte(group), ord(cmd), string, timeout)
    if result.isRes:
        print('add:', result.add)
        print('group:', result.group)
        print('seq:', result.seq)
        print('cmd:', result.cmd)
        print('state:', result.state)
        print('msg:', result.msg)


def Cmd_E(add: int, group: int, string: str, waitFlag: bool, timeout=30000):
    result = api.RunActionScript(Byte(add), Byte(group), string, waitFlag, timeout)
    if waitFlag:
        if result.isFinish:
            print('isOk:', result.isOk)
            print('errCode:', result.errCode)
            print('errMsg:', result.errMsg)
            print('errNodeCode:', result.errNodeCode)
            print('msg:', result.msg)
    else:
        if result.isOk:
            print('isOk:', result.isOk)
            print('errCode:', result.errCode)
            print('errMsg:', result.errMsg)
            print('errNodeCode:', result.errNodeCode)
            print('msg:', result.msg)


def Wait_Cmd_Finish(add: int, group: int, timeout=30000):
    result = api.WaitActionFinish(Byte(add), Byte(group), timeout)
    if result.isFinish:
        print('isOk:', result.isOk)
        print('errCode:', result.errCode)
        print('errMsg:', result.errMsg)
        print('errNodeCode:', result.errNodeCode)
        print('msg:', result.msg)


def RW_Register(add: int, group: int, string: str, timeout=30000):
    data_lst = List[Int32]()
    result, data_lst = api.ReadWriteReg(Byte(add), Byte(group), string, data_lst, timeout)
    for i in data_lst:
        print(i)
    if result.isOk:
        print(data_lst)
        print('isOk:', result.isOk)
        print('errCode:', result.errCode)
        print('errMsg:', result.errMsg)
        print('errNodeCode:', result.errNodeCode)
        print('msg:', result.msg)


def Check_SysStatus(add: int, group: int, *args):
    data = Dictionary[Byte, int]()
    node_id = List[Byte]()
    if args:
        for i in args:
            node_id.Add(i)
    result, data = api.QueryRunStatus(Byte(add), Byte(group), data, node_id)
    if result.isOk:
        print(data)
        print('isOk:', result.isOk)
        print('errCode:', result.errCode)
        print('errMsg:', result.errMsg)
        print('errNodeCode:', result.errNodeCode)
        print('msg:', result.msg)


def Get_Error_Msg(add: int, group: int):
    result = api.GetErrInfo(Byte(add), Byte(group))
    if result.isOk:
        print('isOk:', result.isOk)
        print('errCode:', result.errCode)
        print('errMsg:', result.errMsg)
        print('errNodeCode:', result.errNodeCode)
        print('msg:', result.msg)


def Search_Single_Dev(node_id: int):
    node_info = Madp_NodeInfo()
    result, node_info = api.SearchLinkNode(Byte(node_id), node_info)
    if result.isOk:
        print('add:', node_info.add)
        print('type:', node_info.type)
        print('version:', node_info.version)
        print('typeName:', node_info.typeName)


def Cmd_T(add):
    result=api.EmergencyStop(Byte(add))
    if result.isOk:
        print('isOk:', result.isOk)
        print('errCode:', result.errCode)
        print('errMsg:', result.errMsg)
        print('errNodeCode:', result.errNodeCode)
        print('msg:', result.msg)
def Search_Mult_Dev(node_id: list):
    id_lst = List[Byte]()
    node_info = List[Madp_NodeInfo]()
    for dev in node_id:
        id_lst.Add(dev)
    result, node_info = api.SearchLinkNode(id_lst, node_info)
    if result.isOk:
        for info in node_info:
            print('add:', info.add)
            print('type:', info.type)
            print('version:', info.version)
            print('typeName:', info.typeName)


if __name__ == '__main__':
    api.isDebug = False  # 调试模式，打开后不会控制所有轴动作，用户通过该模式可以模拟控制开发，无需连接设备 true:打开 false：关闭
    api.isLog = False  # 是否保存日志文件,打开后日志文件自动保存到文件夹ktlog true:打开false：关闭,将通过终端输出
    # api.noSameLog = False  # 当isLog=true时，该属性有效true：不打印重复收发数据false：打印所有收发数据

    Sys_Open_Port('COM60', 9600)
    # for i in range(0, 20):
    #     Cmd_Transmit_wait_ack(0, 0, 'e', '1It500')
    #     time.sleep(2)
    # Cmd_E(0, 0, '1It500', True)
    # Cmd_E(0, 0, '1Ld1,1000', True)
    # # Wait_Cmd_Finish(0, 0)
    # # RW_Register(0, 0, 'm0,4')
    # # Check_SysStatus(0,0,1,41)
    # Get_Error_Msg(0, 0)
    # for i in range(1, 49):
    #     Search_Single_Dev(i)
    Search_Mult_Dev([1, 41])
    Cmd_T(0)
    Sys_Close_Port()
