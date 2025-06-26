import time

import clr
import sys
from System import Byte
from System.Collections.Generic import List, Dictionary

clr.AddReference('keytoAPI')
from keytoAPI.MADP import API, KeytoADP_Type

api = API()  # 创建 API 类的实例


def Adp_Num_Conf(node_list: list[int]):
    """生成adp数量地址列表,默认从一开始"""
    node_id = List[Byte]()
    for value in node_list:
        node_id.Add(value)
    return node_id


def Handle_Result(result, name):
    """返回处理函数"""
    if not result.isOk:
        print(f"{name} failed:")
        print(f"  Error code: {result.errCode}")
        print(f"  Error message: {result.errMsg}")
        if result.errCode == 10:
            return True
        else:
            return False
    return True


def Check_State():
    """
    运动后查询状态直到空闲
    :return:
    """
    err_lst = {}
    action_result = api.RunActionWaitFinish()
    if not Handle_Result(action_result, "Check_State"):
        if action_result.errCode == 30:
            data = action_result.errNodeCode
            for key in data.Keys:
                _value = int(data[Byte(key)])
                err_lst[int(key)] = _value
            return err_lst
        elif action_result.errCode == 10:
            return 10
        else:
            sys.exit()
    else:
        return 0


def Sys_Open_Port(port: str, baudrate=38400):
    """
    打开串口
    :param port: 串口号
    :param baudrate: 波特率
    :return:
    """
    if not api.isOpen():
        if api.OpenPort(port, baudrate):
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
    if api.ClosePort():
        print(f"Port closed success")
    else:
        print(f"Port closed failed")


def Sys_Conf(adp_typ, adp_num=1, s_add=-1, x_add=-1, y_add=-1):
    """
    配置系统参数
    :param adp_typ: ADP类型
    :param adp_num:通道个数
    :param s_add:分距轴地址，<0无效
    :param x_add:X轴地址，<0无效
    :param y_add:Y轴地址，<0无效
    :return:
    """
    result = api.Sys_Conf(adp_typ, adp_num, s_add, x_add, y_add)
    if not Handle_Result(result, "Sys_Conf"):
        sys.exit()


def Sys_StrCmdSendWaitAck(cmd: str, string: str):
    """
    发送命令并等待应答
    :param cmd: 命令字符
    :param string: 指令字符串
    :return:
    """
    result = api.StringCommandSendWaitAck(ord(cmd), string)
    if not result.isRes:
        print(f"{Sys_StrCmdSendWaitAck} failed:")
        sys.exit()


def Sys_Imme_ExeCmd(script: str):
    """
    立即执行一条指令
    :param script: 指令字符串
    :return:
    """
    result = api.ImmediateExeCmd(script)
    if not Handle_Result(result, "Imme_ExeCmd"):
        sys.exit()


def Sys_R_reg(node_id, Reg_ID: int):  # 读节点寄存器
    """
    读寄存器
    :param node_id: 节点地址列表
    :param Reg_ID:寄存器地址
    :return:
    """
    data = Dictionary[Byte, int]()
    conf_data = {}
    result, data = api.ReadRegister(node_id, Byte(Reg_ID), data)
    if Handle_Result(result, "Sys_R_reg"):
        for key in data.Keys:
            _value = int(data[Byte(key)])
            conf_data[int(key)] = _value
            print(f"AdpID: {key} Register:{Reg_ID} Value: {_value}")
        return conf_data
    else:
        sys.exit()


def Sys_W_reg(node_id, Reg_ID: int, Reg_Value: int):  # 写节点寄存器
    """
    写寄存器
    :param node_id: 节点地址列表
    :param Reg_ID: 寄存器地址
    :param Reg_Value:写入数据
    :return:
    """
    result = api.WriteRegister(node_id, Byte(Reg_ID), Reg_Value)
    if not Handle_Result(result, "Sys_W_reg"):
        sys.exit()


def Sys_R_Obj(node_id, index: int, subIndex: int):  # 读节点寄存器
    """
    读对象字典
    :param node_id: 节点地址列表
    :param index:主索引
    :param subIndex:子索引
    :return:
    """
    data = Dictionary[Byte, int]()
    conf_data = {int: int}
    result, data = api.ReadObjDic(node_id, index, Byte(subIndex), data)
    if Handle_Result(result, "Sys_R_Obj"):
        for key in data.Keys:
            _value = int(data[Byte(key)])
            conf_data[int(key)] = _value
            print(f"AdpID: {key} SubIdex:{subIndex} Value: {_value}")
        return conf_data
    else:
        sys.exit()


def Sys_W_Obj(node_id, index: int, subIndex: int, data: int):  # 写节点寄存器
    """
    写对象字典
    :param node_id: 节点地址列表
    :param index:主索引
    :param subIndex:子索引
    :param data:写入数据
    :return:
    """
    result = api.WriteObjDic(node_id, index, Byte(subIndex), data)
    if not Handle_Result(result, "Sys_W_Obj"):
        sys.exit()


def Sys_Search_Dev():
    """
    搜索连接的节点
    :return:
    """
    conf_data = {int: str}
    data = api.SearchLinkDevice()
    for key in data.Keys:
        _value = str(data[Byte(key)])
        conf_data[int(key)] = _value
        print(f"ID: {key}, Dev Type: {_value}")
    return conf_data


def Sys_R_Status():  # 读取系统状态
    """
    读取系统状态
    :return:
    """
    result = api.ReadSysStatus()
    if not Handle_Result(result, "Sys_R_Status"):
        sys.exit()


def Sys_Stop():
    """
    停止所有轴
    :return:
    """
    result = api.Stop()
    if not Handle_Result(result, "Sys_Stop"):
        sys.exit()


def Group_Init(adpSpeed=500, zSpeed=50, sSpeed=50, xSpeed=50, ySpeed=50):  # 设备初始化
    """
    设备初始化
    :param adpSpeed: ADP置零速度
    :param zSpeed:Z轴初始化速度，单位mm/s
    :param sSpeed:分距轴初始化速度，单位mm/s
    :param xSpeed:X轴初始化速度，单位mm/s
    :param ySpeed:Y轴初始化速度，单位mm/s
    :return:
    """
    group_result = api.Group_Init(adpSpeed, zSpeed, sSpeed, xSpeed, ySpeed)
    if Handle_Result(group_result, "Group_Init"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Group_GetTIPs(node_id, startPos, downSpeed=100, getSpeed=50, power=80):  # 吸取Tip
    """
    组合取TIP
    :param node_id: ADP地址列表
    :param startPos:取TIP头开始位置，该位置推荐在TIP头上方1cm处
    :param downSpeed:移动到startPos位置过程中速度，单位mm/s
    :param getSpeed:取TIP头速度，单位mm/s
    :param power:取TIP头功率，单位%
    :return:
    """
    action_result = api.Group_GetTipOneByOne(node_id, startPos, downSpeed, getSpeed, power)
    if Handle_Result(action_result, "Group_GetTIPs"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Group_Plld(node_id, start_pos, limit_pos, move_speed=100, detect_speed=20, time_out=10000, mode=1):  # 液面探测
    """
    组合液面探测
    :param node_id: ADP地址列表
    :param start_pos:开始位置，单位mm Z轴移动到该位置后开始液面探测，推荐瓶口上方1cm，
    :param limit_pos:最大位置限制，单位mm 液面探测过程中Z轴下降的最低位置，推荐容器底部1mm上方
    :param move_speed:Z轴移动到开始位置过程中的速度，单位mm
    :param detect_speed:液面探测过程中Z轴下降速度，单位mm
    :param time_out:液面探测超时时间，单位ms
    :param mode:液面探测模式，推荐模式1
    :return:
    """
    action_result = api.Group_LLD(node_id, start_pos, limit_pos, move_speed, detect_speed, time_out, mode)
    if Handle_Result(action_result, "Group_Plld"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                data = [x for x in err_data.keys()]
                Z_Move(Adp_Num_Conf(data), _50_tip_lld_z_pos, speed_z_max)
                Adp_Init(Adp_Num_Conf(data), 500, 100, 2)
                Adp_Asp(Adp_Num_Conf(data), as_air, 500, 10, 0)
    else:
        sys.exit()


def Group_Asfollow(node_id, vol=0, velocity=500, area=78, maxpos=0):  # 液面跟随吸液
    """
    吸液跟随
    :param node_id: ADP地址列表
    :param vol:吸液体积，单位ul
    :param velocity:吸液速度，单位ul/s
    :param area:截面积，单位mm2
    :param maxpos:最大位置限制 0：不做限制 其他值：限制最大位置，当前位置小于最大位置时吸液会上升，这里需要注意
    :return:
    """
    action_result = api.Group_AspirateFollow(node_id, vol, velocity, area, maxpos)
    if Handle_Result(action_result, "Group_Asfollow"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Group_Disfollow(node_id, vol=0, velocity=100, area=78):  # 液面跟随排液
    """
    排液跟随
    :param node_id: ADP地址列表
    :param vol:排液体积，单位ul
    :param velocity:排液速度，单位ul/s
    :param area:截面积,单位mm2
    :return:
    """
    action_result = api.Group_DispenseFollow(node_id, vol, velocity, area)
    if Handle_Result(action_result, "Group_Disfollow"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Group_Mix(node_id, vol: int, velocity: int, minpos=0.0, maxpos=0.0, area=0, cycle=3):  # 混匀
    """
    混匀
    :param node_id: ADP地址列表
    :param vol:混匀体积体积，单位ul
    :param velocity:混匀过程中吸排液速度，单位ul/s
    :param minpos:最小位置限制 0：Z轴不做上下移动
    :param maxpos:最大位置限制 0：Z轴不做上下移动
    :param area:截面积,单位mm2，该参数将影响Z轴上下速度 0：Z轴不做上下移动
    :param cycle:混匀次数
    :return:
    """
    action_result = api.Group_MixFollow(node_id, vol, velocity, minpos, maxpos, area, cycle)
    if Handle_Result(action_result, "Group_Mix"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Group_XYmove(x_pos, y_pos, s_gap, safetyzpos=0, xsp=50, ysp=50, ssp=20):  # XY轴运动
    """
    XY分距轴同时移动指定位置
    :param x_pos:X轴目标位置，单位mm 当参数小于0或者无参数时不移动X轴
    :param y_pos:Y轴目标位置，单位mm 当参数小于0或者无参数时不移动Y轴
    :param s_gap:分距轴目标间距，单位mm 当参数小于0或者无参数时不移动分距轴
    :param safetyzpos:Z轴安全高度，当系统所有Z轴高度大于该值时，不允许移动X、Y、分距
    :param xsp:X轴移动速度，单位mm/s
    :param ysp:Y轴移动速度，单位mm/s
    :param ssp:分距移动速度，单位mm/s
    :return:
    """
    action_result = api.Group_MoveXYS(x_pos, y_pos, s_gap, safetyzpos, xsp, ysp, ssp)
    if Handle_Result(action_result, "Group_XYmove"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Z_Init(node_id, speed=50):  # Z轴初始化
    """
    Z轴初始化
    :param node_id: Z轴对应ADP通道地址列表
    :param speed:初始化Z轴速度mm/s
    :return:
    """
    action_result = api.ZAxis_Init(node_id, speed)
    if Handle_Result(action_result, "Z_Init"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Z_Move(node_id, pos, speed=100):  # Z轴运动
    """
    指定位置移动Z轴
    :param node_id:ADP地址列表
    :param pos:目标位置，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    action_result = api.ZAxis_MovePos(node_id, pos, speed)
    if Handle_Result(action_result, "Z_Move"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Z_MoveUp(node_id, pos, speed=100):  # Z轴运动
    """
    相对上移Z轴
    :param node_id:ADP地址列表
    :param pos:目标位置，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    action_result = api.ZAxis_MoveUp(node_id, pos, speed)
    if Handle_Result(action_result, "Z_MoveUp"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Z_MoveDp(node_id, pos, speed=100):  # Z轴运动
    """
    相对下移Z轴
    :param node_id:ADP地址列表
    :param pos:目标位置，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    action_result = api.ZAxis_MoveDown(node_id, pos, speed)
    if Handle_Result(action_result, "Z_MoveDp"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Z_Get_Tip(node_id, speed=50, power=80):
    """
    Z轴同时取TIP
    :param node_id: ADP地址列表
    :param speed:取TIP头速度，单位mm/s
    :param power:取TIP头功率百分比，单位%
    :return:
    """
    action_result = api.ZAxis_GetTipSync(node_id, speed, power)
    if Handle_Result(action_result, "Z_Get_Tip"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Adp_Init(node_id, speed=500, power=100, initADPMode=0):
    """
    adp初始化
    :param node_id:ADP地址列表
    :param speed:初始化ADP速度，单位ul/s
    :param power:初始化ADP功率
    :param initADPMode:初始化ADP模式 0：无论是否检测到TIP，都将顶出TIP；1：检测到TIP，将顶出TIP；2：不顶出TIP。
    :return:
    """
    action_result = api.ADP_Init(node_id, speed, power, initADPMode)
    if Handle_Result(action_result, "Adp_Init"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Adp_Asp(node_id, vol, speed=500, cutoffSpeed=10, offset=0):
    """
    adp吸液
    :param node_id: ADP地址列表
    :param vol:吸液体积，单位ul
    :param speed:吸液速度，单位ul/s
    :param cutoffSpeed:截流速度，单位ul/s
    :param offset:TIP类型精度补偿：0：无补偿；1：1000ul TIP补偿；2：50ul TIP补偿。
    :return:
    """
    action_result = api.ADP_Aspirate(node_id, vol, speed, cutoffSpeed, offset)
    if Handle_Result(action_result, "Adp_Asp"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Adp_Dis(node_id, vol, backVol=0, speed=500, stopSpeed=10):
    """
    adp排液
    :param node_id: ADP地址列表
    :param vol:吸液体积，单位ul
    :param backVol:回吸体积，单位ul
    :param speed:吸液速度，单位ul/s
    :param stopSpeed:截流速度，单位ul/s
    :return:
    """
    action_result = api.ADP_Dispense(node_id, vol, backVol, speed, stopSpeed)
    if Handle_Result(action_result, "Adp_Dis"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Adp_Push_Tip(node_id, speed=500):  # ADP退Tip
    """
    adp退tip
    :param node_id: ADP地址列表
    :param speed:退TIP头速度，单位ul/s
    :return:
    """
    action_result = api.ADP_PushTip(node_id, speed)
    if Handle_Result(action_result, "Adp_Push_Tip"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Space_Init(speed=10):
    """
    分距初始化
    :param speed: 初始化分距轴速度,单位mm/s
    :return:
    """
    action_result = api.Space_Init(speed)
    if Handle_Result(action_result, "Space_Init"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Space_Move(val=0, speed=10):  # 分距控制
    """
    指定间距移动分距
    :param val:目标间距，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    action_result = api.Space_Move(val, speed)
    if Handle_Result(action_result, "Space_Move"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def X_Init(speed=10):
    """
    X轴初始化
    :param speed: 初始化X轴速度,单位mm/s
    :return:
    """
    action_result = api.XAxis_Init(speed)
    if Handle_Result(action_result, "X_Init"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def X_Move(val=0, speed=50):
    """
    X轴移动指定位置
    :param val:目标位置，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    action_result = api.XAxis_MovePos(val, speed)
    if Handle_Result(action_result, "X_Move"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Y_Init(speed=10):
    """
    Y轴初始化
    :param speed: 初始化Y轴速度,单位mm/s
    :return:
    """
    action_result = api.YAxis_Init(speed)
    if Handle_Result(action_result, "Y_Init"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


def Y_Move(val=0, speed=50):
    """
    X轴移动指定位置
    :param val:目标位置，单位mm
    :param speed:移动速度，单位mm/s
    :return:
    """
    action_result = api.YAxis_MovePos(val, speed)
    if Handle_Result(action_result, "Y_Move"):
        err_data = Check_State()
        if err_data:
            if err_data == 10:
                time.sleep(3)
            else:
                for _tmp in err_data.keys():
                    print(_tmp, ':', err_data[_tmp])
                sys.exit()
    else:
        sys.exit()


if __name__ == '__main__':
    api.isDebug = False  # 调试模式，打开后不会控制所有轴动作，用户通过该模式可以模拟控制开发，无需连接设备 true:打开 false：关闭
    api.isLog = True  # 是否保存日志文件,打开后日志文件自动保存到文件夹ktlog true:打开false：关闭,将通过终端输出
    api.isMonitor = False  # 运动过程中监控节点数据 true：打开 false：关闭系统在运动过程中实时查询节点数据
    api.noSameLog = False  # 当isLog=true时，该属性有效true：不打印重复收发数据false：打印所有收发数据
    run_flag = api.isRuning  # 运行脚本过程中，只读 true：运行中 false：无脚本运行

    adp_num = 8  # adp数量
    adp_id = Adp_Num_Conf([x for x in range(1, adp_num + 1)])  # 生成adp地址列表
    z_id = Adp_Num_Conf([x + 40 for x in range(1, adp_num + 1)])  # 生成Z轴地址列表
    space_addr = 10  # 分距地址
    x_addr = 11  # X轴地址
    y_addr = 0  # Y轴地址

    x_max_pos = 195  # X轴最大位置，单位mm
    y_max_pos = 270  # Y轴最大位置，单位mm
    speed_z_max = 180  # Z轴运行最大速度
    pos_z_max = 180  # Z轴运动最大位置

    tip_num = 11  # 取tip最大排数
    get_tip_y_start_pos = 5.923  # 取tip第一排Y轴位置
    get_tip_x_start_pos = 98.55  # 取tip第一排X轴位置
    get_tip_z_pos = 150  # 取tipZ轴开始位置
    get_tip_y_cur_pos = get_tip_y_start_pos  # 取tip第一排Y轴位置
    get_tip_x_cur_pos = get_tip_x_start_pos  # 取tip第一排X轴位置

    tube_x_pos = 145.158  # plldX轴位置
    tube_y_pos = 131.194  # plldY轴位置
    tube_z_max_pos = 150  # plldZ轴下限位置
    _1000_tip_lld_z_pos = 60  # plld 1000uL tip开始位置
    _50_tip_lld_z_pos = _1000_tip_lld_z_pos + 30  # plld 50/200uL tip开始位置
    plld_z_speed = 20  # plldZ轴速度，单位mm/s
    timeout = 10000  # 超时时间单位ms

    as_air = 200  # 吸空量
    as_vol = 100  # 吸液量
    mix_vol = 50  # 混匀体积
    as_speed = 500  # 吸排液速度
    tube_area = int(3.14 * ((9 / 2) ** 2))  # 试管表面积

    Sys_Open_Port('com52', 38400)  # 串口配置
    adp_type = KeytoADP_Type.KEYTO_TYPE_ADP16  # 设备类型
    Sys_Conf(adp_type, 1)  # 系统配置
    Adp_Init(adp_id,2000)
    # Sys_Conf(adp_type, 8, space_addr, x_addr, y_addr)  # 系统配置

    _cycle = 0  # 记录循环次数
    gettip_count = 1  # 取tip当前拍数，默认从第一排开始
    Group_Init(500, 50)  # 设备初始化
    # istip = Sys_R_reg(adp_id, 3)  # 检查设备是否有tip，有的话则到指定位置退tip
    # for num in istip.keys():
    #     if istip[num] == 1:
    #         Group_XYmove(x_max_pos, y_max_pos, 9)  # XY轴移动到取tip位置
    #         Adp_Push_Tip(adp_id)
    #         break
    # plld_z_pos = {}
    # first_plld = 1
    # 老化开始
    # while True:
    #     Group_XYmove(get_tip_x_cur_pos, get_tip_y_cur_pos, 9)  # XY轴移动到取tip位置
    #     print('get tip pos:', get_tip_x_cur_pos, get_tip_y_cur_pos)
    #     Sys_W_reg(adp_id, 60, 0)  # 关闭异常检测
    #     Adp_Asp(adp_id, as_air)  # adp吸空
    #     Group_GetTIPs(adp_id, get_tip_z_pos, 180, 30, 80)  # Z轴下降取tip
    #     # 取完之后读取是否取成功，失败再取一次
    #     istip = Sys_R_reg(adp_id, 3)
    #     re_get = []  # 获取重新取tip的adp地址
    #     for num in istip.keys():
    #         if istip[num] == 0:
    #             re_get.append(num)
    #     if len(re_get) > 0:  # 重新取tip
    #         while True:
    #             Group_GetTIPs(Adp_Num_Conf(re_get), get_tip_z_pos, 180, 30, 100)
    #             re_get.clear()
    #             istip = Sys_R_reg(adp_id, 3)
    #             for num in istip.keys():
    #                 if istip[num] == 0:
    #                     print('channel:', num, 'No tip')
    #                     re_get.append(num)
    #             if len(re_get) == 0:
    #                 break
    #     Z_Move(adp_id, 0, 180)  # Z轴回到零位
    #     Adp_Dis(adp_id, 200, 0, 1000, 10)  # 吹气排tip内液膜
    #     Group_XYmove(tube_x_pos, tube_y_pos, 18)  # XY轴移动到plld位置
    #     Adp_Asp(adp_id, as_air)  # adp吸空
    #     Z_Move(adp_id, _50_tip_lld_z_pos, 180)  # Z轴回到零位
        # Group_Plld(adp_id, _50_tip_lld_z_pos, tube_z_max_pos, speed_z_max, plld_z_speed, timeout, 1)  # 液面探测
        # if first_plld:
        #     plld_z_pos = Sys_R_reg(z_id, 101)  # 记录第一次液面探测Z轴位置
        #     first_plld = 0
        # plld_z_cur_pos = Sys_R_reg(z_id, 101)
        # re_plld = []  # 重新液面探测adp地址列表
        # for z_num in plld_z_pos.keys():
        #     if abs(plld_z_pos[z_num] - plld_z_cur_pos[z_num]) > 10000:
        #         re_plld.append(z_num - 40)
        # if len(re_plld) > 0:
        #     Group_Plld(Adp_Num_Conf(re_plld), _50_tip_lld_z_pos, tube_z_max_pos, speed_z_max, plld_z_speed, timeout,
        #                0)
        #     re_plld_z_pos = Sys_R_reg(Adp_Num_Conf([x + 40 for x in re_plld]), 101)  # 重新获取探测后Z轴位置
        #     for tmp in re_plld_z_pos.keys():
        #         plld_z_cur_pos[tmp] = re_plld_z_pos[tmp]
        # plld_z_pos = plld_z_cur_pos
        # Group_Mix(adp_id, mix_vol, 500, _50_tip_lld_z_pos, pos_z_max, tube_area, 3)  # 混匀
        # Z_MoveDp(adp_id, 2, 20)  # 下降2mm
        # Group_Asfollow(adp_id, as_vol, as_speed, pos_z_max)  # 吸液跟随
        # Z_MoveUp(adp_id, 10, speed_z_max)  # Z轴相对上移10mm
        # Group_Disfollow(adp_id, as_air + as_vol, as_speed, tube_area)  # 排液跟随
        # Z_Move(adp_id, 0, speed_z_max)  # Z轴运动到零位
        # Group_XYmove(get_tip_x_cur_pos, get_tip_y_cur_pos, 9)  # XY轴移动到退tip位置
        # print('push tip pos:', get_tip_x_cur_pos, get_tip_y_cur_pos)
        # Z_Move(adp_id, get_tip_z_pos, speed_z_max)  # Z轴下降到退tip位置
        # Adp_Push_Tip(adp_id)  # adp退tip
        # Z_Move(adp_id, 0, speed_z_max)  # Z轴回到零位
        #
        # get_tip_y_cur_pos += 9  # Y轴位置加9mm，准备取下一排tip
        # gettip_count += 1  # 取tip排数加1
        # if gettip_count > tip_num:
        #     gettip_count = 1
        #     get_tip_y_cur_pos = get_tip_y_start_pos  # Y轴位置回到第一排位置
        # _cycle += 1  # 循环次数加1
        # print('cycle:', _cycle)
        # if _cycle % 100 == 0:
        #     Group_Init(500, 50, 20, 20, 20)  # 设备初始化
