import random
import time
from binascii import a2b_hex
from openpyxl import Workbook
import clr
import Com.Port.serialport as sp
from System import Byte, Int32
from System.Collections.Generic import List, Dictionary

clr.AddReference('keytoAPI')
from keytoAPI.MADPV2 import API, Madp_NodeInfo, Madp_Send, Madp_Ack
from keytoAPI.Driver.CAN import CAN_DeviceType

api = API()  # 创建 API 类的实例
# can分析仪类型定义
# *************************************************#
dev_usbcan = CAN_DeviceType.DEV_USBCAN  # 3
dev_usbcan2 = CAN_DeviceType.DEV_USBCAN2  # 4
vci_usbcan_e_u = CAN_DeviceType.VCI_USBCAN_E_U  # 20
vci_usbcan_2e_u = CAN_DeviceType.VCI_USBCAN_2E_U  # 21
# *************************************************#
add = 0  # MADP地址


def Sys_Config(isLog: bool, isHex: bool, noSameLog: bool, isDebug: bool, isParse: bool, mode: bool):
    """
    系统配置
    :param isHex:是否打印HEX格式收发数据 true：打开 false：关闭
    :param isDebug: 调试模式，打开后不会控制所有轴动作，用户通过该模式可以模拟控制开发，无需连接设备 true:打开 false：关闭
    :param isLog: 是否保存日志文件,打开后日志文件自动保存到文件夹ktlog true:打开false：关闭,将通过终端输出
    :param noSameLog: 当isLog=true时，该属性有效true：不打印重复收发数据false：打印所有收发数据
    :param isParse: 是否打开协议解析，用于扩展用户协议是通过该功能关闭MADP的协议解析，避免数据处理出现混乱 true:打开 false：关闭
    :param mode:是否打开动作完成主动上报功能 true:打开 false：关闭
    :return:
    """
    api.isLog = isLog
    api.isHex = isHex
    api.noSameLog = noSameLog
    api.isDebug = isDebug
    api.logDir = r'D:\GitHubRepository\pythonProject\Test\MADPAPITest\ktlog'
    api.isParse = isParse
    api.openActionCompleted = mode


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
            return False, 'OpenSerial'
    print(f"Port opened success")
    return True, 'OpenSerial'


def Sys_Close_Port():
    """
    关闭串口
    :return:
    """
    if api.CloseSerial():
        print(f"Port closed success")
        return True, 'CloseSerial'
    else:
        print(f"Port closed failed")
        return False, 'CloseSerial'


def Sys_CAN_Open(devType, devind=0, canind=0, baudrate=500):
    """
    打开CAN
    :param devType:CAN设备类型
    :param devind:CAN设备号
    :param canind:CAN端口号
    :param baudrate:波特率，单位k
    :return:
    """
    if not api.isCanOpen():
        if api.OpenCan(devType, devind, canind, baudrate):
            pass
        else:
            print(f"CAN open failed")
            return False, 'OpenCan'
    print(f"CAN opened success")
    return True, 'OpenCan'


def Sys_CAN_Close():
    """
    关闭CAN
    :return:
    """
    if api.CloseCan():
        print(f"CAN closed success")
        return True, 'CloseCan'
    else:
        print(f"CAN closed failed")
        return False, 'CloseCan'


def Sys_TCP_Connect(serverIp, port):
    """
    TCP连接
    :param serverIp:
    :param port:
    :return:
    """
    if not api.isTcpConnect():
        if api.TcpConnect(serverIp, port):
            pass
        else:
            print(f"TCP open failed")
            return False, 'TcpConnect'
    print(f"TCP opened success")
    return True, 'TcpConnect'


def Sys_TCP_Disconnect():
    """
    TCP断开
    :return:
    """
    if api.TcpDisconnect():
        print(f"TCP closed success")
        return True, 'TcpDisconnect'
    else:
        print(f"TCP closed failed")
        return False, 'TcpDisconnect'


def KeytoResult_Handle(result, data, func_name: str):
    result_dic = {
        'isOk': bool(result.isOk),
        'isFinish': bool(result.isFinish),
        'errCode': int(result.errCode),
        'errNodeCode': dict(result.errNodeCode),
        'errMsg': str(result.errMsg),
        'msg': str(result.msg)
    }
    return result_dic, data, func_name


def Madp_Ack_Handle(result, func_name: str):
    result_dic = {
        'isRes': bool(result.isRes),
        'add': int(result.add),
        'group': int(result.group),
        'seq': int(result.seq),
        'cmd': str(result.cmd),
        'state': int(result.state),
        'msg': str(result.msg)
    }
    return result_dic, func_name


def Cmd_Transmit_wait_ack(group: int, cmd: str, string: str, timeout=500):
    """

    :param group:启动的组ID，范围0～15
    :param cmd:命令
    :param string:字符串
    :param timeout:等待超时时间,ms
    :return:
    """
    global add
    result = api.StringCommandSendWaitAck(Byte(add), Byte(group), ord(cmd), string, timeout)
    return Madp_Ack_Handle(result, 'StringCommandSendWaitAck')


def Cmd_E(group: int, string: str, waitFlag: bool, timeout=30000):
    """

    :param group:启动的组ID，范围0～15
    :param string:脚本内容
    :param waitFlag:是否等待动作完成 true：阻塞等待动作完成后返回数据 false：启动后立即返回是否启动成功
    :param timeout:
    :return:等待动作完成超时时间，单位ms
    """
    global add
    result = api.RunActionScript(Byte(add), Byte(group), string, waitFlag, timeout)
    return KeytoResult_Handle(result, None, 'RunActionScript')


def Wait_Cmd_Finish(group: int, timeout=30000):
    """

    :param group:等待的组ID，范围0～15
    :param timeout:超时时间,ms
    :return:
    """
    global add
    result = api.WaitActionFinish(Byte(add), Byte(group), timeout)
    return KeytoResult_Handle(result, None, 'WaitActionFinish')


def RW_Register(group: int, string: str, timeout=3000):
    """

    :param group:组ID，范围0～15
    :param string:读写的脚本
    :param timeout:超时时间
    :return:
    """
    global add
    data_lst = List[Int32]()
    da_lst = []
    result, data_lst = api.ReadWriteReg(Byte(add), Byte(group), string, data_lst, timeout)
    for i in data_lst:
        da_lst.append(int(i))
    return KeytoResult_Handle(result, da_lst, 'ReadWriteReg')


def Check_SysStatus(group: int, *args):
    """

    :param group:组ID，范围0～15
    :param args:空：查询当前脚本所有的节点状态 非空：查询的目标节点地址
    :return:
    """
    global add
    data = Dictionary[Byte, int]()
    node_id = List[Byte]()
    if args:
        for i in args:
            node_id.Add(i)
    result, data = api.QueryRunStatus(Byte(add), Byte(group), data, node_id)
    return KeytoResult_Handle(result, dict(data), 'QueryRunStatus')


def Get_Error_Msg(group: int):
    """

    :param group: 组ID，范围0～15
    :return:
    """
    global add
    result = api.GetErrInfo(Byte(add), Byte(group))
    return KeytoResult_Handle(result, None, 'GetErrInfo')


def Search_Single_Dev(node_id: int):
    """

    :param node_id: 组ID，范围0～15
    :return:
    """
    node_info = Madp_NodeInfo()
    result, node_info = api.SearchLinkNode(Byte(node_id), node_info)
    info_dic = {
        'add': int(node_info.add),
        'type': int(node_info.type),
        'version': str(node_info.version),
        'typeName': str(node_info.typeName),
    }
    return KeytoResult_Handle(result, info_dic, 'SearchLinkNode')


def Search_Mult_Dev(node_id: list):
    """

    :param node_id: 组ID，范围0～15
    :return:
    """
    id_lst = List[Byte]()
    info_lst = []
    node_info = List[Madp_NodeInfo]()
    for dev in node_id:
        id_lst.Add(dev)
    result, node_info = api.SearchLinkNode(id_lst, node_info)
    for i in node_info:
        info_dic = {
            'add': int(i.add),
            'type': int(i.type),
            'version': str(i.version),
            'typeName': str(i.typeName),
        }
        info_lst.append(info_dic)
    return KeytoResult_Handle(result, info_lst, 'SearchLinkNode')


def Cmd_T():
    global add
    result = api.EmergencyStop(Byte(add))
    return KeytoResult_Handle(result, None, 'EmergencyStop')


def Cmd_t(group: int, *args):
    """

    :param group:组ID，范围0～15
    :param args:空：停止脚本中所有地址 非空：停止指定的地址
    :return:
    """
    global add
    string = ''
    for arg in args:
        string += str(arg) + ','
    string = string[:-1]
    result = api.Stop(Byte(add), group, string, 2000)
    return KeytoResult_Handle(result, None, 'Stop')


def Group_Adp_Init(group: int, ID_lst: list, waitFlag: bool, speed=500, mode=2):
    """

    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param speed:ADP置零速度,单位ul/s
    :param mode:初始化ADP模式0：无论是否检测到TIP，都将顶出TIP；1：检测到TIP，将顶出TIP；2：不顶出TIP
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ADP_Init(Byte(group), id_lst, waitFlag, speed, mode)
    return KeytoResult_Handle(result, None, 'ADP_Init')


def Group_Adp_Aspirate(group: int, ID_lst: list, waitFlag: bool, vol: float, speed=500, cutoffSpeed=10):
    """

    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param vol:吸液体积，单位ul
    :param speed:吸液速度，单位ul/s
    :param cutoffSpeed:截流速度，单位ul/s
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ADP_Aspirate(Byte(group), id_lst, waitFlag, vol, speed, cutoffSpeed)
    return KeytoResult_Handle(result, None, 'ADP_Aspirate')


def Group_Adp_Dispense(group: int, ID_lst: list, waitFlag: bool, vol: float, backVol=0.0, speed=500, cutoffSpeed=10):
    """

    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param vol:吸液体积，单位ul
    :param backVol:回吸体积，单位ul
    :param speed:吸液速度，单位ul/s
    :param cutoffSpeed:截流速度，单位ul/s
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ADP_Dispense(Byte(group), id_lst, waitFlag, vol, backVol, speed, cutoffSpeed)
    return KeytoResult_Handle(result, None, 'ADP_Dispense')


def Group_Adp_Move(group: int, ID_lst: list, waitFlag: bool, pos: float, speed=500, stopSpeed=10):
    """

    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param pos:柱塞位置(ul)
    :param speed:移动速度(ul/s)
    :param stopSpeed:停止速度(ul/s)
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ADP_Move(Byte(group), id_lst, waitFlag, pos, speed, stopSpeed)
    return KeytoResult_Handle(result, None, 'ADP_Move')


def Group_Adp_PushTip(group: int, ID_lst: list, waitFlag: bool, speed=500):
    """

    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param speed:ADP置零速度,单位ul/s
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ADP_PushTip(Byte(group), id_lst, waitFlag, speed)
    return KeytoResult_Handle(result, None, 'ADP_PushTip')


def Group_Adp_Plld(group: int, ID_lst: list, waitFlag: bool, timeout=10000, mode=0):
    """

    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param timeout:超时时间
    :param mode:液面探测模式
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ADP_LiqDet(Byte(group), id_lst, waitFlag, timeout, mode)
    return KeytoResult_Handle(result, None, 'ADP_LiqDet')


def Group_Z_Init(group: int, ID_lst: list, waitFlag: bool, speed=50.0):
    """

    :param group:组ID，范围0～15
    :param ID_lst:Z轴地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param speed:初始化Z轴速度，单位mm/s
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ZAxis_Init(Byte(group), id_lst, waitFlag, speed)
    return KeytoResult_Handle(result, None, 'ZAxis_Init')


def Group_Z_MovePos(group: int, ID_lst: list, waitFlag: bool, pos: float, speed=100.0):
    """

    :param group:组ID，范围0～15
    :param ID_lst:Z轴地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param pos:目标位置，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ZAxis_MovePos(Byte(group), id_lst, waitFlag, pos, speed)
    return KeytoResult_Handle(result, None, 'ZAxis_MovePos')


def Group_Z_MoveUp(group: int, ID_lst: list, waitFlag: bool, pos: float, speed=100.0):
    """

    :param group:组ID，范围0～15
    :param ID_lst:Z轴地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param pos:目标距离，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ZAxis_MoveUp(Byte(group), id_lst, waitFlag, pos, speed)
    return KeytoResult_Handle(result, None, 'ZAxis_MoveUp')


def Group_Z_MoveDown(group: int, ID_lst: list, waitFlag: bool, pos: float, speed=100.0):
    """

    :param group:组ID，范围0～15
    :param ID_lst:Z轴地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param pos:目标距离，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ZAxis_MoveDown(Byte(group), id_lst, waitFlag, pos, speed)
    return KeytoResult_Handle(result, None, 'ZAxis_MoveDown')


def Group_Z_PickUpTip(group: int, ID_lst: list, waitFlag: bool, speed=50.0, power=80, endpos=180.0):
    """

    :param group:组ID，范围0～15
    :param ID_lst:Z轴地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param speed:取TIP头速度，单位mm/s
    :param power:取TIP头功率百分比，单位%
    :param endpos:取TIP头最大位置限制，单位mm。0：不限制>0：最大位置限制
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.ZAxis_PickUp(Byte(group), id_lst, waitFlag, speed, power, endpos)
    return KeytoResult_Handle(result, None, 'ZAxis_PickUp')


def Group_Space_Init(group: int, id_lst: int, waitFlag: bool, speed: float):
    """

    :param group:组ID，范围0～15
    :param id_lst:变距轴地址
    :param waitFlag:是否阻塞等待动作完成
    :param speed:初始化分距轴速度,单位mm/s
    :return:
    """
    result = api.Space_Init(Byte(group), Byte(id_lst), waitFlag, speed)
    return KeytoResult_Handle(result, None, 'Space_Init')


def Group_Space_Move(group: int, id_lst: int, waitFlag: bool, value: float, speed=50):
    """

    :param group:组ID，范围0～15
    :param id_lst:变距轴地址
    :param waitFlag:是否阻塞等待动作完成
    :param value:目标间距，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    result = api.Space_Move(Byte(group), Byte(id_lst), waitFlag, value, speed)
    return KeytoResult_Handle(result, None, 'Space_Move')


def Group_ArmAxis_Init(group: int, id_lst: int, waitFlag: bool, speed=50):
    """

    :param group:组ID，范围0～15
    :param id_lst:移液臂轴地址
    :param waitFlag:是否阻塞等待动作完成
    :param speed:初始化X轴速度,单位mm/s
    :return:
    """
    result = api.ArmAxis_Init(Byte(group), Byte(id_lst), waitFlag, speed)
    return KeytoResult_Handle(result, None, 'ArmAxis_Init')


def Group_ArmAxis_MovePos(group: int, id_lst: int, waitFlag: bool, pos: float, speed=50):
    """

    :param group:组ID，范围0～15
    :param id_lst:移液臂轴地址
    :param waitFlag:是否阻塞等待动作完成
    :param pos:目标位置，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    result = api.ArmAxis_MovePos(Byte(group), Byte(id_lst), waitFlag, pos, speed)
    return KeytoResult_Handle(result, None, 'ArmAxis_MovePos')


def Group_ArmAxis_MoveDir(group: int, id_lst: int, waitFlag: bool, pos: float, speed=50):
    """

    :param group:组ID，范围0～15
    :param id_lst:移液臂轴地址
    :param waitFlag:是否阻塞等待动作完成
    :param pos:相对距离，单位mm>0：正向<0：反向
    :param speed:移动速度，单位mm/s
    :return:
    """
    result = api.ArmAxis_MoveDir(Byte(group), Byte(id_lst), waitFlag, pos, speed)
    return KeytoResult_Handle(result, None, 'ArmAxis_MoveDir')


def Mul_PickUpTip(group: int, ID_lst: list, waitFlag: bool, startPos: float, endPos=180.0, moveSpeed=100.0,
                  pickupSpeed=50.0, power=80):
    """

    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param startPos:取TIP头开始位置，单位mm，该位置推荐在TIP头上方1cm处
    :param endPos:取TIP头结束位置，单位mm
    :param moveSpeed:移动到startPos位置过程中速度，单位mm/s
    :param pickupSpeed:取TIP头速度，单位mm/s
    :param power:取TIP头功率，单位%
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.MultiAction_PickupTip(Byte(group), id_lst, waitFlag, startPos, endPos, moveSpeed, pickupSpeed, power)
    return KeytoResult_Handle(result, None, 'MultiAction_PickupTip')


def Mul_LldAspirateFollow(group: int, ID_lst: list, waitFlag: bool, imbVol: float, imbSpeed: int, zSpeed=100.0,
                          lldMode=0, lldSpeed=20.0, imbDetection=0, imbBottleTopPos=0.0, imbBottleBottomPos=0.0,
                          imbBottleArea=0.0):
    """

    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param imbVol:吸液体积，单位ul
    :param imbSpeed:吸液速度，单位ul/s
    :param zSpeed:Z轴移动速度，单位mm/s
    :param lldMode:液面探测模式
    :param lldSpeed:液面探测过程中Z轴下降速度，单位mm/s
    :param imbDetection:压力异常检测使能
    :param imbBottleTopPos:容器顶部位置，液面探测将在容器顶部上方1cm位置开始
    :param imbBottleBottomPos:容器底部位置，液面探测和吸液过程中，Z轴下降的最低位置
    :param imbBottleArea:容器截面积，吸液过程中将根据容器截面积计算Z轴下降速度
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.MultiAction_LldAspirateFollow(Byte(group), id_lst, waitFlag, imbVol, imbSpeed, zSpeed, Byte(lldMode),
                                               lldSpeed, Byte(imbDetection), imbBottleTopPos, imbBottleBottomPos,
                                               imbBottleArea)
    return KeytoResult_Handle(result, None, 'MultiAction_LldAspirateFollow')


def Mul_DispenseFollow(group: int, ID_lst: list, waitFlag: bool, disVol: float, disSpeed: int, disZAxisPos: float,
                       zSpeed=100.0, disBottleArea=0.0):
    """
    排液跟随
    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param disVol:排液体积，单位ul
    :param disSpeed:排液速度，单位ul/s
    :param disZAxisPos:排液Z轴位置，单位mm
    :param zSpeed:Z轴移动速度，单位mm/s
    :param disBottleArea:容器截面积mm2，吸液过程中将根据容器截面积计算Z轴上升速度
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.MultiAction_DispenseFollow(Byte(group), id_lst, waitFlag, disVol, disSpeed, disZAxisPos, zSpeed,
                                            disBottleArea)
    return KeytoResult_Handle(result, None, 'MultiAction_DispenseFollow')


def Mul_DispenseEmpty(group: int, ID_lst: list, waitFlag: bool, disSpeed: int, disZAxisPos: float, zSpeed=100.0):
    """

    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param disSpeed:排液速度，单位ul/s
    :param disZAxisPos:排液Z轴位置，单位mm
    :param zSpeed:Z轴移动速度，单位mm/s
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.MultiAction_DispenseEmpt(Byte(group), id_lst, waitFlag, disSpeed, disZAxisPos, zSpeed)
    return KeytoResult_Handle(result, None, 'MultiAction_DispenseEmpt')


def Mul_PushTip(group: int, ID_lst: list, waitFlag: bool, zAxisPos: float, zSpeed=100.0):
    """

    :param group:组ID，范围0～15
    :param ID_lst:ADP地址列表
    :param waitFlag:是否阻塞等待动作完成
    :param zAxisPos:退TIP头Z轴位置，单位mm
    :param zSpeed:Z轴移动速度，单位mm/s
    :return:
    """
    id_lst = List[Byte]()
    for dev in ID_lst:
        id_lst.Add(dev)
    result = api.MultiAction_PushTip(Byte(group), id_lst, waitFlag, zAxisPos, zSpeed)
    return KeytoResult_Handle(result, None, 'MultiAction_PushTip')


def Extend_Serial_Transmit(hex_str: str):
    csharp_code = []
    for i in range(0, len(hex_str), 2):
        csharp_code.append(Byte(int(hex_str[i:i + 2], 16)))
    return api.SerialWrite(csharp_code)


def GetPos():
    sp.ser.PortSend(bytes.fromhex('02 2D 00 00 00 00 0D 0A'))
    data = sp.ser.Wait_Rx_Finish()
    datalen = int(data[2:4], 16)
    dataint = int(a2b_hex(data[4:datalen * 2 + 4]).decode())
    print('dataint:', dataint)
    return dataint


if __name__ == '__main__':
    # 创建一个工作簿
    wb = Workbook()
    # 获取活动工作表
    ws = wb.active
    # 数据
    data = []
    target_pos = 31
    sp.Reset_Ser_Baud(0, 'com5', 115200)
    sp.ser.PortSend(bytes.fromhex('02 2A 00 00 00 01 0D 0A'))
    time.sleep(1)
    Sys_CAN_Open(dev_usbcan2, 0, 0, 500)
    Sys_Config(False, False, False, False, True, True)
    Group_ArmAxis_Init(0, 10, True, 50)
    Group_ArmAxis_Init(0, 11, True, 50)
    start_pos = GetPos()
    # data.append(start_pos)
    # Group_ArmAxis_MovePos(0, 10, True, 10, 50)
    for i in range(0, 11):
        real_data = []
        for j in range(0, 5):
            Group_ArmAxis_MovePos(0, 11, True, target_pos * i, 200)
            time.sleep(1)
            end_pos = GetPos()
            real_data.append(end_pos)
            Group_ArmAxis_Init(0, 11, True, 50)
            # Group_ArmAxis_MovePos(0, 10, True, random.randint(0, 430), 200)
        data.append(real_data)

    for k in data:
        ws.append(k)
    wb.save("示例数据.xlsx")
    print("数据已写入Excel文件")
