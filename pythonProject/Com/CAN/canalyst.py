# python3.8.0 64位（python 32位要用32位的DLL）
import datetime
import time
from ctypes import *
from typing import Any
import Com.CAN.can_directory as ca

CanDLLName = r'D:\code\pythonProject\Com\CAN\ControlCAN.dll'  # 把DLL放到对应的目录下
canDLL = windll.LoadLibrary(r'D:\code\pythonProject\Com\CAN\ControlCAN.dll')


class VCI_BOARD_INFO(Structure):
    _fields_ = [
        ('hw_Version', c_ushort),
        ('fw_Version', c_ushort),
        ('dr_Version', c_ushort),
        ('in_Version', c_ushort),
        ('irq_Num', c_ushort),
        ('can_Num', c_ubyte),
        ('str_Serial_Num', c_byte * 20),
        ('str_hw_Type', c_byte * 40),
        ('Reserved', c_ubyte * 4)
    ]


class VCI_INIT_CONFIG(Structure):
    """
    初始化CAN的配置，结构体将在VCI_InitCan函数中被填充，即初始化之前，要先填好这个结构体变量。\n
    AccCode:验收码。SJA1000的帧过滤验收码。对经过屏蔽码过滤为“有关位”进行匹配，全部匹配成功后，此帧可以被接收。否则不接收。\n
    AccMask:屏蔽码。SJA1000的帧过滤屏蔽码。对接收的CAN帧ID进行过滤，对应位为0的是“有关位”，对应位为1的是“无关位”。
    屏蔽码推荐设置为0xFFFFFFFF，即全部接收。\n
    Reserved:保留。\n
    Filter:滤波方式，=1 表示单滤波，=0 表示双滤波\n
    Timing0:波特率定时器 0（BTR0）。设置值见下表。\n
    CAN波特率 Timing0(BTR0) Timing1(BTR1)\n
    20 Kbps   0x18            0x1C\n
    40 Kbps   0x87            0xFF\n
    50 Kbps   0x09            0x1C\n
    80 Kbps   0x83            0xFF\n
    100 Kbps  0x04            0x1C\n
    125 Kbps  0x03            0x1C\n
    200 Kbps  0x81            0xFA\n
    250 Kbps  0x01            0x1C\n
    400 Kbps  0x80            0xFA\n
    500 Kbps  0x00            0x1C\n
    666 Kbps  0x80            0xB6\n
    800 Kbps  0x00            0x16\n
    1000 Kbps 0x00            0x14\n
    33.33 Kbps 0x09           0x6F\n
    66.66 Kbps 0x04           0x6F\n
    83.33 Kbps 0x03           0x6F\n
    Timing1:波特率定时器 1（BTR1）。设置值见上表。\n
    Mode:模式。=0表示正常模式（相当于正常节点），=1表示只听模式（只接收，不影响总线），=2表示自发自收模式（环回模式）。\n
    """
    _fields_ = [("AccCode", c_uint),
                ("AccMask", c_uint),
                ("Reserved", c_uint),
                ("Filter", c_ubyte),
                ("Timing0", c_ubyte),
                ("Timing1", c_ubyte),
                ("Mode", c_ubyte)
                ]


class VCI_CAN_OBJ(Structure):
    """
    CAN帧结构体，即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中，被用来传送CAN信息帧\n
    ID:帧ID。32位变量，数据格式为靠右对齐。\n
    TimeStamp:设备接收到某一帧的时间标识。时间标示从CAN卡上电开始计时，计时单位为0.1ms。\n
    TimeFlag:是否使用时间标识，为1时TimeStamp有效，TimeFlag和TimeStamp只在此帧为接收帧时有意义。\n
    SendType: 发送帧类型。
                =0时为正常发送（发送失败会自动重发，重发超时时间为1秒，1秒内没有发出则取消）；\n
                =1时为单次发送（只发送一次，发送失败不会自动重发，总线只产生一帧数据波形）；\n
                其它值无效。\n
            注意：
                多节点通信时，务必将SendType强制设为0，否则将会出现发送丢帧的情况。
                如果只是单节点通信，比如，作为一个CAN信号发生器，定时发出信号， SendType可设为0。
    RemoteFlag:是否是远程帧。=0时为数据帧，=1时为远程帧（数据段空）。\n
    ExternFlag:是否是扩展帧。=0时为标准帧（11位ID），=1时为扩展帧（29位ID）。\n
    DataLen:数据长度 DLC (<=8)，即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。\n
    Data[8]:CAN帧的数据。由于CAN规定了最大是8个字节，所以这里预留了8个字节的空间。\n
    受DataLen约束。如DataLen定义为3，即Data[0]、Data[1]、Data[2]是有效的。\n
    Reserved:系统保留。
    """
    _fields_ = [("ID", c_uint),
                ("TimeStamp", c_uint),
                ("TimeFlag", c_ubyte),
                ("SendType", c_ubyte),
                ("RemoteFlag", c_ubyte),
                ("ExternFlag", c_ubyte),
                ("DataLen", c_ubyte),
                ("Data", c_ubyte * 8),
                ("Reserved", c_ubyte * 3)
                ]


# 结构体数组类
class VCI_CAN_OBJ_ARRAY(Structure):
    _fields_ = [('SIZE', c_uint16), ('STRUCT_ARRAY', POINTER(VCI_CAN_OBJ))]

    def __init__(self, num_of_structs, *args: Any, **kw: Any):
        super().__init__(*args, **kw)
        self.STRUCT_ARRAY = cast((VCI_CAN_OBJ * num_of_structs)(), POINTER(VCI_CAN_OBJ))  # 结构体数组
        self.SIZE = num_of_structs  # 结构体长度
        self.ADDR = self.STRUCT_ARRAY[0]  # 结构体数组地址  byref()转c地址


class CANALYST:
    def __init__(self):
        """
        CAN通讯类
        """
        self.devtype = 21  # 设备类型
        self.devindex = 0  # 设备索引,只有一个USB-CAN为0，再插入一个则为1
        self.canindex = 0  # CAN通道:0
        self.can_dev_num = 0  # 当前设备can通道总数
        self.status_ok = 1
        self.Reserved = 0  # 保留参数，一般为0
        self.acccode = 0x00000000  # 验收码
        self.accmask = 0xFFFFFFFF  # 屏蔽码
        self.can_filter = 1  # 滤波方式 1:接收所有类型；2：只接收标准帧；3：只接收扩展帧
        self.baudrate = 500  # 波特率
        self.mode = 0  # 模式。0:正常模式（相当于正常节点），1:只听模式（只接收，不影响总线），2:自发自收模式（环回模式）。
        self.timestamp = 0  # 设备接收到某一帧的时间标识
        self.timeflag = 0  # 是否使用时间标识
        self.sendtype = 0  # 发送帧类型:0:正常发送；1:单次发送
        self.remoteflag = 0  # 是否是远程帧。0:数据帧，1:远程帧（数据段空）
        self.externflag = 1  # 是否是扩展帧。0:标准帧（11位ID），1:扩展帧（29位ID）
        self.send_frame_num = 1
        self.can_send_data = []  # 发送单帧数据列表变量
        self.can_recv_data = []  # 接收单帧数据列表变量
        self.baud_dic = {
            20: [0x1600B3, 0x18, 0x1C],
            50: [0x1C002C, 0x09, 0x1C],
            100: [0x160023, 0x04, 0x1C],
            125: [0x1C0011, 0x03, 0x1C],
            250: [0x1C0008, 0x01, 0x1C],
            500: [0x060007, 0x00, 0x1C],
            800: [0x060004, 0x00, 0x16],
            1000: [0x060003, 0x00, 0x14]
        }

    def Dev_Scanf(self):
        dev_info = VCI_BOARD_INFO()
        if canDLL.VCI_ReadBoardInfo(self.devtype, self.devindex, byref(dev_info)) == 1:
            return dev_info.can_Num
        else:
            return 0

    # 打开设备
    def Dev_Open(self):
        """
        打开CAN设备
        :return: True：打开成功；False：打开失败
        """
        ov = canDLL.VCI_OpenDevice(self.devtype, self.devindex, self.Reserved)
        if ov == self.status_ok:
            print('CAN: 设备开启成功')
            return True
        else:
            print('CAN: 设备开启失败')
            return False

    # 关闭设备
    def Dev_Close(self):
        """
        关闭设备
        :return: True：关闭成功；False：关闭失败
        """
        ret = canDLL.VCI_CloseDevice(self.devtype, self.devindex)
        if ret == self.status_ok:
            print('CAN: 设备关闭成功')
            return True
        else:
            print('CAN: 设备关闭失败')
            return False

    def Init_Can_Config(self):
        """
        CAN通道初始化配置，之前需先调用VCI_SetReference设置波特率
        :return: True：设置成功；False：设置失败
        """
        vci_initconfig = VCI_INIT_CONFIG(self.acccode, self.accmask, self.Reserved, self.can_filter,
                                         self.baud_dic[self.baudrate][1], self.baud_dic[self.baudrate][2], self.mode)
        ic = canDLL.VCI_InitCAN(self.devtype, self.devindex, self.canindex, byref(vci_initconfig))
        if ic == self.status_ok:
            return True
        else:
            return False

    def Can_Init(self):
        """
        CAN通道初始化
        :return:True：设置成功；False：设置失败
        """
        # 波特率设置
        ret1 = canDLL.VCI_SetReference(self.devtype, self.devindex, self.canindex, 0,
                                       pointer(c_int(self.baud_dic[self.baudrate][0])))
        ret2 = self.Init_Can_Config()  # 初始化通道
        if ret1 and ret2:
            print('CAN通道' + str(self.canindex + 1) + '初始化成功')
            return True
        else:
            print('CAN通道' + str(self.canindex + 1) + '初始化失败')
            return False

    def Can_Open(self):
        """
        打开CAN通道：之前需设备初始化+通道初始化
        :return: True：打开成功；False：打开失败
        """
        sc = canDLL.VCI_StartCAN(self.devtype, self.devindex, self.canindex)
        if sc == self.status_ok:
            print('CAN通道' + str(self.canindex + 1) + '打开成功')
            return True
        else:
            print('CAN通道' + str(self.canindex + 1) + '打开失败')
            return False

    def Can_Send(self, can_id, can_data):
        """
        CAN发送数据，单帧发送
        :param can_id: 帧ID:str
        :param can_data: 帧数据:str
        :return: True：发送成功；False：发送失败
        """
        reser = (c_ubyte * 3)(0, 0, 0)  # reserved[3]
        self.can_send_data = [can_id, can_data]
        data_len = len(can_data) // 2
        vco = VCI_CAN_OBJ(int(can_id, 16), self.timestamp, self.timeflag, self.sendtype, self.remoteflag,
                          self.externflag, data_len,
                          self.Data_Send_Config(can_data), reser)
        # 单次发送
        ret = canDLL.VCI_Transmit(self.devtype, self.devindex, self.canindex, byref(vco), self.send_frame_num)
        print(datetime.datetime.now(), end=':')
        if ret == self.send_frame_num:
            print('Send:', 'ID:', '0x' + can_id, 'len:', data_len, 'Data:', '0x' + can_data)
            return True
        else:
            print('通道 ' + str(self.canindex + 1) + ' 发送数据失败')
            return False

    @staticmethod
    def Data_Send_Config(data: str):
        """
        将16进制字符串转成16进制C数组格式
        :param data:16进制字符串
        :return: ubyte_array:ubyte_array
        """
        data = data.replace('0x', '').replace(' ', '')
        data_config = [int(x, 16) for x in [data[ig:ig + 2] for ig in range(0, len(data), 2)]]
        ubyte_array = (c_ubyte * len(data_config))()
        for ic in range(len(data_config)):
            ubyte_array[ic] = data_config[ic]
        return ubyte_array

    # 通道接收数据
    def Can_Receive(self, timeout=500):
        """
        CAN通道接收数据
        :param timeout: 超时时间单位ms;0：不检测超时
        :return: 0:无返回，else：接收到帧数，数据存放在类变量can_recv_data[],数据格式[帧ID，帧数据]
        """
        rx_vci_can_obj = VCI_CAN_OBJ_ARRAY(self.send_frame_num)  # 结构体数组
        if timeout == 0:
            while True:
                rece = canDLL.VCI_Receive(self.devtype, self.devindex, self.canindex, byref(rx_vci_can_obj.ADDR),
                                          self.send_frame_num)
                if rece > 0:  # 接收到一帧数据
                    break
        else:
            start_time = time.perf_counter()
            while True:
                end_time = time.perf_counter()
                if (end_time - start_time) * 1000 < timeout:
                    # 接收函数,返回接收帧数据个数
                    rece = canDLL.VCI_Receive(self.devtype, self.devindex, self.canindex, byref(rx_vci_can_obj.ADDR),
                                              self.send_frame_num)
                    if rece > 0:  # 接收到一帧数据
                        break
                else:
                    print(datetime.datetime.now(), end=':')
                    print('CAN:接收超时')
                    return 0
        print(datetime.datetime.now(), end=':')
        can_id = hex(rx_vci_can_obj.STRUCT_ARRAY[0].ID).replace('0x', '').zfill(8).upper()
        can_datalen = int(rx_vci_can_obj.STRUCT_ARRAY[0].DataLen)
        candata = list(rx_vci_can_obj.STRUCT_ARRAY[0].Data)
        # 将数据转化为字符串格式
        candata_hex = ''.join(list(map(lambda x: hex(x).replace('0x', '').zfill(2), candata))).upper()
        self.can_recv_data = [can_id, candata_hex]
        print('Rece:', 'ID:', '0x' + can_id, 'len:', can_datalen, 'Data:', '0x' + candata_hex)
        return rece


class ADP_CAN:
    def __init__(self):
        self.adp_can = canalyst
        self.host_addr = 0
        self.sub_addr = 1
        self.state = 0
        self.report_mode = 0
        self.can_idex = ''
        self.adp_can_dic = ca.ADP_CAN_DIC()
        self.state_dic = {
            0: '空闲', 1: 'ADP忙', 2: '执行成功', 3: '探测到液面', 4: 'Z轴忙', 5: 'Z轴忙，ADP忙', 10: '参数超限',
            11: '参数错误', 12: '语法错误', 13: '无效指令', 14: '地址错误', 15: '禁止写', 16: '禁止读',
            17: 'ADP未初始化', 18: 'Z轴未初始化', 19: 'Z轴未连接', 20: '未检测到TIP', 21: '顶出TIP失败',
            22: '超时报警', 23: '吸堵检测报警', 24: '气泡检测报警', 25: '吸空检测报警', 26: '排气凝块报警',
            27: '排气气泡报警', 28: '抗悬滴范围超限', 50: '电机堵转', 51: '驱动器故障', 52: '光耦1错误',
            53: '光耦2错误', 54: '传感器错误', 55: 'EEPROM错误', 56: '电源欠压', 57: '电源过压', 58: '电机短路',
            59: '电机开路', 80: 'Z轴电机堵转', 81: 'Z轴电机驱动器故障', 82: 'Z轴电机光耦错误'
        }

    def Can_Send_Rece(self, data_lst: list, mode='W', timeout=500):
        adp_id_hex = self.Send_Frame_ID_Config(mode)  # 写对象字典
        rec_lst = []
        for acs in data_lst:
            self.can_idex = int(acs[0:2], 16)  # 获取发送帧索引
            self.adp_can.Can_Send(adp_id_hex, acs)
            while True:
                if self.adp_can.Can_Receive(timeout):
                    rec = self.adp_can.can_recv_data
                    re_frame_id_type = rec[0][0:4]  # 获取返回帧类型
                    re_idex = int(rec[1][0:2], 16)  # 获取接收帧索引
                    if re_frame_id_type == ca.frame_id_type['response'] and re_idex == self.can_idex:
                        rec_lst.append(rec)
                        break
                else:
                    rec_lst.append([])
                    break
        return rec_lst

    def Can_Rec_Check(self, data_lst: list):
        true_flag = 0
        for tmp in data_lst:
            sta = int(tmp[1][8:], 16)
            if sta == 2:
                true_flag += 1
            else:
                self.state = sta
                print('error state:', sta)
        if true_flag == len(data_lst):
            return True
        return False

    @staticmethod
    def Can_Get_Data(data_lst: list):
        return int(data_lst[0][1][-8:], 16)

    def Send_Frame_ID_Config(self, mode):
        """
        发送帧ID格式化
        :param mode: 模式：'W':写字典；'R':读字典
        :return:str：帧ID16进制字符串:'xxxxxxxx'
        """
        # 帧ID格式化
        can_id_l = (hex(self.host_addr).removeprefix('0x').zfill(2) + hex(self.sub_addr).removeprefix('0x').zfill(
            2)).upper()
        if mode == 'W':
            id_hex = ca.frame_id_type['write'] + can_id_l  # 写字典帧ID格式
        else:
            id_hex = ca.frame_id_type['read'] + can_id_l  # 读字典帧ID格式
        return id_hex

    def Can_Check_State(self):
        while True:
            rec = self.Can_Send_Rece(self.adp_can_dic.Can_Check_State_Cmd(), 'R')
            if len(rec):
                rec_stat = int(rec[0][1][8:], 16)
                if rec_stat == 1:
                    time.sleep(0.005)
                    continue
                else:
                    return rec_stat


canalyst = CANALYST()

if __name__ == "__main__":
    can=CANALYST()
    can.Dev_Open()
    can.Can_Init()
    can.Can_Open()
    can.Can_Send('00010000','0050010000000000')
    can.Can_Receive()
    # 初始化CAN指令
    # adp1_can = ADP_CAN()
    # # # 配置帧ID
    # adp1_can.adp_can.Dev_Open()
    # adp1_can.adp_can.Can_Init()
    # adp1_can.adp_can.Can_Open()
    # adp1_can.
    # if adp1_can.Can_Rec_Check(adp1_can.Can_Send_Rece(adp1_can.adp_can_dic.Can_It_Cmd(), 'W')):
    #     print('done')
    # adp1_can.Can_Check_State()
    # adp1_can.Can_Check_State()
    # adp1_can.Can_Send_Rece(adp_write_id_hex, adp1_can.adp_can_dic.Can_Ia_Cmd(100000, 1000, 10))
    # adp1_can.Can_Check_State()
    # adp1_can.Can_Send_Rece(adp_write_id_hex, adp1_can.adp_can_dic.Can_Da_Cmd(100000, 0, 1000, 10))
    # adp1_can.Can_Check_State()
    # adp1_can.adp_can.Dev_Close()
    # can1 = CANALYST()
    # can1.Dev_Open()
    # print(can1.Dev_Scanf())
