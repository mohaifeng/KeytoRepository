# python3.11 64位（python 32位要用32位的DLL）
import ctypes
import datetime
import time
from ctypes import *
from typing import Any

CanDLLName = r'D:\code\pythonProject\Com\CAN\hcanbus.dll'  # 把DLL放到对应的目录下
canDLL = windll.LoadLibrary(r'D:\code\pythonProject\Com\CAN\hcanbus.dll')


class Can_Config(Structure):
    """
    初始化 CAN 的配置
    1. Baudrate 波特率，直输，无需计算,比如设 500k，baudrate=500000。
    2. Configs 配置信息：0x01 接通内部电阻(仅对含有内部电子开关的设备有效)，0x02 离线唤醒，0x04 自动重传。
    3. Model 工作模式：0 正常模式,1 环回模式,2 静默模式,3 静默环回模式
    """
    _fields_ = [("baudrate", c_uint),
                ("configs", c_uint),
                ("model", c_uint)
                ]


class Can_Msg(Structure):
    """
    帧的数据结构
    1. ID 报文帧 ID 标识。
    2. TimeStamp 帧数据的时间标识，从 CAN 控制器初始化开始计时，单位微秒。也可用于定时发送。
    3. FrameType 帧类型:
        1 0x00 接收帧
        2 0x01 发送帧
        3 0x02 传输失败
        4 0x10 定时发送开始标识
        5 0x20 定时发送结束标识
    4. DataLen 数据长度 DLC(<=8)，即 Data 的长度。
    5. Data CAN 报文的数据。有效字节为 DataLen。
    6. ExternFlag 是否是扩展帧。=0 时为标准帧（11 位帧 ID），=1 时为扩展帧（29 位帧 ID）。
    7. RemoteFlag 是否是远程帧。=0 时为数据帧，=1 时为远程帧。
    8. BusSatus 总线状态,定义见表 2。
    9. ErrSatus 错误状态，定义见表 3。
    10. TECounter 发送错误计数。
    11. RECounter 接收错误计数。
    """
    _fields_ = [("ID", c_uint),
                ("TimeStamp", c_uint),
                ("FrameType", c_ubyte),
                ("DataLen", c_ubyte),
                ("Data", c_ubyte * 8),
                ("ExternFlag", c_ubyte),
                ("RemoteFlag", c_ubyte),
                ("BusSatus", c_ubyte),
                ("ErrSatus", c_ubyte),
                ("TECounter", c_ubyte),
                ("RECounter", c_ubyte)
                ]


class Can_Msg_ARRAY(Structure):
    _fields_ = [('SIZE', ctypes.c_uint16), ('STRUCT_ARRAY', ctypes.POINTER(Can_Msg))]

    def __init__(self, num_of_structs, *args: Any, **kw: Any):
        # 这个括号不能少
        super().__init__(*args, **kw)
        self.STRUCT_ARRAY = ctypes.cast((Can_Msg * num_of_structs)(), ctypes.POINTER(Can_Msg))  # 结构体数组
        self.SIZE = num_of_structs  # 结构体长度
        self.ADDR = self.STRUCT_ARRAY[0]  # 结构体数组地址  byref()转c地址


class CAN_BUS:

    def __init__(self):
        """
        CAN通讯类
        """
        self.dev_type = 21  # can设备类型
        self.DevIndex = 0  # 设备索引号
        self.can_index = 0  # can通道
        self.can_dev_num = 0  # 当前设备can通道总数
        self.status_ok = 0
        self.rec_num = 1
        self.send_frame_flag = 0x01  # 发送帧标识
        self.can_baudrate = 500  # CAN波特率，单位K
        self.can_configs = 0x01  # 配置信息：0x01 接通内部电阻(仅对含有内部电子开关的设备有效)，0x02 离线唤醒，0x04 自动重传。
        self.can_model = 0  # 工作模式：0 正常模式,1 环回模式,2 静默模式,3 静默环回模式。
        self.identifier_type = 1  # 是否是扩展帧。=0时为标准帧（11位帧ID），=1时为扩展帧（29位帧ID）。
        self.can_remoteflag = 0  # 是否是远程帧。=0时为数据帧，=1时为远程帧。
        self.can_send_data = []
        self.can_recv_data = []

    def Can_Dev_Scanf(self):
        """
        扫描CAN通道数
        :return: 0：无通道；else:CAN通道数
        """
        self.can_dev_num = canDLL.CAN_ScanDevice()
        if self.can_dev_num > 0:
            print('扫描到通道数:', self.can_dev_num)
            return self.can_dev_num
        else:
            print('当前无CAN通道')
            self.can_dev_num = 0
            return 0

    def Can_Init(self):
        """
        CAN通道初始化
        :return:True:成功；False：失败
        """
        can_initconfig = Can_Config(self.can_baudrate * 1000, self.can_configs, self.can_model)
        can_ret = canDLL.CAN_Init(self.can_index, byref(can_initconfig))
        if can_ret == self.status_ok:
            print('初始化通道' + str(self.can_index + 1) + '成功')
            return True
        else:
            print('初始化通道' + str(self.can_index + 1) + '失败')
            return False

    def Can_Open(self):
        """
        打开CAN通道
        :return:True:成功；False：失败
        """
        open_ret = canDLL.CAN_OpenDevice(self.can_index)  # 打开通道0
        if open_ret == self.status_ok:
            print('CAN通道' + str(self.can_index + 1) + '打开成功')
            return True
        else:
            print('CAN通道' + str(self.can_index + 1) + '打开失败')
            return False

    def Can_Close(self):
        """
        关闭CAN通道
        :return:True:成功；False：失败
        """
        close_ret = canDLL.CAN_CloseDevice(self.can_index)  # 打开通道0
        if close_ret == self.status_ok:
            print('CAN通道' + str(self.can_index + 1) + '关闭成功')
            return True
        else:
            print('CAN通道' + str(self.can_index + 1) + '关闭失败')
            return False

    def Can_Reset(self):
        """
        复位CAN通道
        :return:True:成功；False：失败
        """
        reset_ret = canDLL.CAN_Reset(self.can_index)  # 打开通道0
        if reset_ret == self.status_ok:
            print('VCI_StartCAN: 通道 ' + str(self.can_index + 1) + ' 复位成功')
            return True
        else:
            print('VCI_StartCAN: 通道 ' + str(self.can_index + 1) + ' 复位失败')
            return False

    def Can_SetFilter(self, namber, ftype, ftID, ftMask, enable):
        """
        设置 CAN 过滤器
        :param namber:过滤器编号，范围 0-13
        :param ftype:过滤器类型，0：2 个 16 位过滤器，1：一个 32 位过滤器。
        :param ftID:标识符
        :param ftMask:屏蔽位
        :param enable:是否使能， 0：非使能，1：使能
        :return:True:成功；False：失败
        """
        can_ret = canDLL.CAN_SetFilter(self.can_index, namber, ftype, ftID, ftMask, enable)
        if can_ret == self.status_ok:
            print('CAN过滤器设置成功')
            return True
        else:
            print('CAN过滤器设置失败')
            return False

    def Can_Send(self, can_dat_ID, _can_data):
        """
        发送数据
        :param can_dat_ID:帧 ID->str
        :param _can_data:CAN can数据->str
        :return:True:成功；False：失败
        """
        self.can_send_data = [can_dat_ID, _can_data]
        # 将传入str转化为int型列表
        can_hex_id = int(can_dat_ID, 16)
        # 将16进制字符串转成16进制C数组格式
        dat_lst = [int(x, len(_can_data)) for x in [_can_data[ig:ig + 2] for ig in range(0, len(_can_data), 2)]]
        arr = (c_ubyte * len(dat_lst))(*dat_lst)
        can_datalen = len(_can_data) // 2
        send_canmsg = Can_Msg(can_hex_id, 0, self.send_frame_flag, can_datalen, arr, self.identifier_type,
                              self.can_remoteflag, 0, 0, 0, 0)
        if canDLL.CAN_Transmit(self.can_index, byref(send_canmsg), 1, 0) == 1:
            print(datetime.datetime.now(), end=':')
            print('Send:', 'ID:', '0x' + can_dat_ID, 'len:', can_datalen, 'Data:', '0x' + _can_data)
            return True
        else:
            print(datetime.datetime.now(), end=':')
            print('Send error')
            return False

    def Can_Receive(self, overtime=1000):
        """
        can接收数据
        :param overtime: 接收超时时间，默认100ms
        :return: 0:无返回；else：接收到帧数,数据存放在类变量can_recv_data[],数据格式[帧ID，帧数据]
        """
        receive_canmsg = Can_Msg_ARRAY(self.rec_num)  # 结构体数组
        if overtime == 0:
            while True:
                rec = canDLL.CAN_Receive(self.can_index, byref(receive_canmsg.ADDR), self.rec_num, 0)
                if rec > 0:  # 接收到一帧数据
                    break
        else:
            start_time = time.perf_counter()
            while True:
                end_time = time.perf_counter()
                if (end_time - start_time) * 1000 <= overtime:
                    rec = int(canDLL.CAN_Receive(self.can_index, byref(receive_canmsg.ADDR), self.rec_num, 0))
                    if rec > 0:  # 接收到一帧数据
                        break
                else:
                    print(datetime.datetime.now(), end=':')
                    print('CAN:接收超时')
                    return 0
        print(datetime.datetime.now(), end=':')
        can_id = hex(receive_canmsg.STRUCT_ARRAY[0].ID).replace('0x', '').zfill(8)
        can_data_len = int(receive_canmsg.STRUCT_ARRAY[0].DataLen)
        can_dat = list(receive_canmsg.STRUCT_ARRAY[0].Data)
        # 将数据转化为字符串格式
        candata_hex = ''.join(list(map(lambda x: hex(x).replace('0x', '').zfill(2), can_dat))).upper()
        print('Rece:', 'ID:', '0x' + can_id, 'len:', can_data_len, 'Data:', '0x' + candata_hex)
        self.can_recv_data = [can_id, candata_hex]
        return rec


canbus = CAN_BUS()

if __name__ == '__main__':
    CAN_INDEX = 0  # CAN通道
    CANBAUDRATE = 500  # 500K

    canbus.can_index = CAN_INDEX
    canbus.can_baudrate = CANBAUDRATE
    frame_id = '00010001'  # 指令构成：CAN指令（2byte）+主控地址（1byte）+目标地址（1byte）
    can_data = '00400000000001F4'  # 指令构成：序列号（1byte）+主索引（2byte）+子索引（1byte）+数据（4byte）

    canbus.Can_Dev_Scanf()  # 扫描CAN通道
    canbus.Can_Open()  # 打开指定can通道
    canbus.Can_Init()  # can通道初始化
    # canbus.Can_Send(frame_id, can_data)  # 发送数据
    while True:
        canbus.Can_Receive()  # 接收数据
    # canbus.Can_Close()

# canDLL.CAN_CloseDevice(0)
# canDLL.CAN_CloseDevice(1)
