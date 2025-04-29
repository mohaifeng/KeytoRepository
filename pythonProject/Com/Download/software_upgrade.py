import struct
import threading
import time
import Com.Port.check as ck
import Com.Port.serialport as sp
import os
import Com.CAN.canalyst as canalyst
import Com.CAN.can_bus as canbus
from Configure.downloadpath import Get_Latest_File

file_bin_name_lst = []  # 下载与回退文件路径列表
file_get_flag = 0  # 获取到列表标志
rec_flag = 0  # 下载过程中收到重发帧标志
done_flag = 0  # 下载完成标志
lock = threading.Lock()
# 定义数字对应设备类型
type_dic = {
    1: 'ADP16',
    2: 'ADP18',
    3: '旋转阀',
    4: '旋转阀-编码器',
    5: '旋转阀-I2C',
    6: '旋转阀-VICI',
    7: 'DRL-计量泵',
    8: 'DRL-柱塞泵',
    9: 'DRS-柱塞泵',
    10: 'VLG-柱塞泵',
    11: 'VLG-PUSI-柱塞泵',
}
dev_support_baudrate = {
    'ADP16': [9600, 19200, 38400, 115200],
    'ADP18': [9600, 19200, 38400, 115200],
    '旋转阀': [9600, 19200, 38400, 57600, 115200],
    '旋转阀-编码器': [9600, 19200, 38400, 57600, 115200],
    '旋转阀-I2C': [9600, 19200, 38400, 57600, 115200],
    '旋转阀-VICI': [4800, 9600, 19200, 38400, 57600, 115200],
    'DRL-计量泵': [9600, 19200, 38400, 57600, 115200],
    'DRL-柱塞泵': [9600, 19200, 38400, 57600, 115200],
    'DRS-柱塞泵': [9600, 19200, 38400, 57600, 115200],
    'VLG-柱塞泵': [9600, 19200, 38400, 57600, 115200],
    'VLG-PUSI-柱塞泵': [9600],
}


def Run():
    """
    Bootloader线程运行函数，检测是否接受到返回数据，有则将全局变量rec_flag置1
    退出线程需将done_flag置1
    """
    global rec_flag
    global done_flag
    th1_ser = sp.ser
    th1_ser.PortClean()
    while True:
        if th1_ser.open_com.in_waiting:
            lock.acquire()
            rec_flag = 1
            lock.release()
        if done_flag == 1:
            done_flag = 0
            break


class MyThread(threading.Thread):

    def __init__(self, obj):
        super().__init__()
        self.obj = obj

    def run(self, ):
        Run()


class DOWNLOAD_PLUS:
    def __init__(self, bin_nam):
        self.bin_name = bin_nam
        self.dlp = sp.ser
        self.download_start_str = '1FF11FF11FF11FF1'
        self.frame_start = '1F'
        self.frame_end = 'FE'
        self.Status_Ok = '00'
        self.data_address = 0
        self.download_len = 240
        self.download_cmd = {'擦除': '01', '写数据': '02', '校验': '03', }
        self.file = open(self.bin_name, 'rb')
        self.file_size = os.path.getsize(self.bin_name)
        self.send_data_size = self.Get_File_Size()
        self.send_data = ''
        self.receive_data = ''

    def Get_File_Size(self):
        """
        计算bin文件字节个数，若个数不是8的倍数，则自动加4个字节
        :return: bin文件字节个数（必然是8的倍数）
        """
        if self.file_size % 8 == 0:
            return self.file_size
        else:
            return ((self.file_size // 8) + 1) * 8

    def Erase_Program_Config(self, num: int):
        """
        配置擦除指令
        :param num: 需要擦除的字节个数
        :return:擦除指令：str
        """
        len_erase = hex(num).replace('0x', '').zfill(8)
        conf_data = self.frame_start + self.download_cmd['擦除'] + '0'.zfill(8) + '04' + len_erase
        check_sum_16 = ck.Uchar_Checksum_16_Bit(conf_data)
        return (conf_data + check_sum_16 + self.frame_end).upper()

    def Write_Program_Config(self, sd_data: str):
        """
        配置写入指令
        :param sd_data: 写入字节字符串
        :return:擦除指令：str
        """
        data_len = hex(len(sd_data) // 2).replace('0x', '').zfill(2)
        download_addr = hex(self.data_address).replace('0x', '').zfill(8)
        conf_data = self.frame_start + self.download_cmd['写数据'] + download_addr + data_len + sd_data
        check_sum_16 = ck.Uchar_Checksum_16_Bit(conf_data)
        return (conf_data + check_sum_16 + self.frame_end).upper()

    def Check_Program_Config(self, checksum: str):
        """
        配置校验指令
        :param checksum: 校验码字符串
        :return: 校验指令：str
        """
        checksum_hex = checksum.replace('0x', '').zfill(8)
        conf_data = self.frame_start + self.download_cmd['校验'] + '0'.zfill(8) + '04' + checksum_hex
        check_sum_16 = ck.Uchar_Checksum_16_Bit(conf_data)
        return (conf_data + check_sum_16 + self.frame_end).upper()

    def DownLoad_Send(self, dat):
        """
        发送下载数据
        :param dat: 数据字符串
        :return: True：成功；False：失败
        """
        self.send_data = dat
        return self.dlp.PortSend(bytes.fromhex(dat))

    def DownLoad_Rec(self, timeout=2000):
        """
        获取下载模式下返回数据
        :param timeout: 超时时间，单位ms
        :return: 0：无返回；else：返回数据字节个数
        """
        self.receive_data = ''
        while True:
            if self.dlp.PortReceive_Data(1, timeout):
                self.receive_data += self.dlp.receive_buf
                if self.dlp.receive_buf == self.frame_end:
                    return len(self.receive_data) // 2
            else:
                return 0

    def Rece_Check(self):
        """
        判断返回状态
        :return: True：成功；False：失败
        """
        if self.DownLoad_Rec():
            if self.receive_data[2:4] == self.Status_Ok:
                return True
        return False

    def Bin_Get_Data(self):
        """
        以列表形式返回bin文件数据
        :return: bin文件数据列表
        """
        self.file.seek(0)  # 将文件指针移动到固定地址偏移量处
        fin_data_lst = []
        for _ in range(0, self.file_size // 4):
            fin_data = ''
            data = self.file.read(4)
            value = hex(struct.unpack('I', data)[0]).replace('0x', '').zfill(8).upper()
            for va in range(0, len(value) + 1, 2):
                tmp = value[va:va + 2]
                fin_data = tmp + fin_data
            fin_data_lst.append(fin_data)
        if self.file_size % 8 != 0:
            fin_data_lst.append('FFFFFFFF')
        return fin_data_lst

    def Get_Send_Data(self):
        """
        格式化数据列表，以设定每次发送字节数生成发送列表
        :return:发送数据列表
        """
        send_str_lst = []
        bin_data = self.Bin_Get_Data()
        bin_data_len = len(bin_data) * 4  # 85384
        idex = 0
        full_cyc = bin_data_len // self.download_len  # 355
        rest_cyc_len = bin_data_len - full_cyc * 240  # 184
        for count in range(0, full_cyc):  # 355
            send_str = ''
            for j in range(idex, idex + self.download_len // 4):
                send_str += bin_data[j]
            send_str_lst.append(send_str)
            idex += (self.download_len // 4)
            # self.data_address = idex
        send_str = ''
        for k in range(idex, idex + rest_cyc_len // 4):
            send_str += bin_data[k]
        send_str_lst.append(send_str)
        return send_str_lst


class BOOTLOADER:
    def __init__(self, file_name: str, com_type: int):
        """
        bootloader升级类
        :param file_name:升级文件绝对路径
        :param com_type: 通讯类型：0：串口；1：can
        """
        self.btl = sp.ser
        self.com_type = com_type  # 通讯类型：0：串口；1：can
        self.can_type = 0  # 0：can_alyst；1：can_bus；-1：无连接
        self.can_alyst = canalyst.canalyst
        self.can_bus = canbus.canbus
        self.bin_name = file_name
        self.file = open(self.bin_name, 'rb')
        self.file_size = os.path.getsize(self.bin_name)
        self.frame_start = 'A4'  # 帧头
        self.bl_comm_frame = 'A5'  # 表示bootloader串口通讯数据
        self.bl_can_frame = '05'  # 表示bootloader can通讯数据
        self.cmd_data = 'FFFFFF'  # 表示指令数据
        self.dev_type_cmd = '01000000'  # 升级设备型号指令
        self.frame_type_cmd = '02000000'  # 升级数据帧数指令
        self.check_type_cmd = '03000000'  # 校验数据帧数指令
        self.download_dev = self.bin_name.split('0X')[1][0:8]  # 升级设备类型（默认adp16）
        self.upgrade_num = 0  # 升级数据的帧数
        self.send_data_size = self.Get_File_Size()
        self.data_address = 0
        self.send_data = ''
        self.receive_data = ''

    def Check_Can_Type(self):
        if self.can_alyst.Dev_Open():
            self.can_alyst.Can_Init()
            self.can_alyst.Can_Open()
            self.can_type = 0
        elif self.can_bus.Can_Dev_Scanf():
            self.can_bus.Can_Open()
            self.can_bus.Can_Init()
            self.can_type = 1
        else:
            print('CAN无连接')
            self.can_type = -1

    def Get_File_Size(self):
        """
        获取发送bin文件长度（以8字节字节为单位）若不能整除则取整后加1
        :return:
        """
        if self.file_size % 8 == 0:
            return self.file_size // 8
        else:
            return (self.file_size // 8) + 1

    def Bootloader_Enter_Cmd(self):
        """
        生成格式化发送进入下载模式命令字符串
        :return: 进入下载模式命令字符串
        """
        enter_cmd = self.frame_start + self.bl_comm_frame + self.cmd_data + self.dev_type_cmd + self.download_dev
        crc_8_str = ck.CRC_8(enter_cmd)
        return enter_cmd + crc_8_str

    def Bootloader_DataLen_cmd(self):
        """
        生成清除发送数据长度内存命令字符串
        :return: 发送数据长度命令字符串
        """
        send_len = hex(self.send_data_size).replace('0x', '').zfill(8).upper()
        datalen_cmd = self.frame_start + self.bl_comm_frame + self.cmd_data + self.frame_type_cmd + send_len
        crc_8_str = ck.CRC_8(datalen_cmd)
        return datalen_cmd + crc_8_str

    def Bootloader_Write_Cmd(self):
        """
        生成格式化发送数据命令字符串列表
        :return: 发送数据命令字符串列表
        """
        send_data_lst = []
        data_lst = self.Get_Send_Data()
        idex = 0
        for dat_8 in data_lst:
            send_cmd = self.frame_start + self.bl_comm_frame + hex(idex).replace('0x', '').zfill(6) + dat_8
            crc_8 = ck.CRC_8(send_cmd)
            send_data_lst.append(send_cmd + crc_8)
            idex += 1
        return send_data_lst

    def Bootloader_Check_Cmd(self):
        """
        生成格式化发送数据CRC32校验数据命令字符串
        :return: 数据CRC32校验数据命令字符串
        """
        send_lst = self.Get_Send_Data()
        fin_str = ''
        for tmp in send_lst:
            fin_str += tmp
        check_crc32 = ck.CRC_32(fin_str)
        send_data_lst = self.frame_start + self.bl_comm_frame + self.cmd_data + self.check_type_cmd + check_crc32
        check_str = ck.CRC_8(send_data_lst)
        return send_data_lst + check_str

    # def Bootloader_Can_Write_cmd(self):
    #     send_data_lst = []
    #     data_lst = self.Get_Send_Data()
    #     idex = 0
    #     for dat_8 in data_lst:
    #         send_cmd = [self.bl_can_frame + hex(idex).replace('0x', '').zfill(6), dat_8]
    #         send_data_lst.append(send_cmd)
    #         idex += 1
    #     return send_data_lst

    def Ser_To_Can_Cmd(self, data_str: str):
        """
        串口数据转can数据
        :param data_str: 串口数据：str
        :return: can数据：list[帧id,data]
        """
        can_id = self.bl_can_frame + data_str[4:10]
        can_data = data_str[10:-2]
        return [can_id, can_data]

    def Can_To_Ser_Cmd(self, can_data: list):
        """
        can数据转串口数据
        :param can_data: can数据：list[帧id,data]
        :return: 串口数据：str
        """
        ser_str = self.frame_start + can_data[0] + can_data[1]
        crc8_str = ck.CRC_8(ser_str)
        return ser_str + crc8_str

    def Bootloader_Send(self, dat: str):
        """
        通过串口发送数据
        :param dat: 数据字符串
        :return: True:发送成功；False：发送失败
        """
        self.send_data = dat
        if self.com_type == 0:
            return self.btl.PortSend(bytes.fromhex(dat))
        can_data = self.Ser_To_Can_Cmd(dat)
        if self.can_type == 0:
            return self.can_alyst.Can_Send(can_data[0], can_data[1])
        else:
            return self.can_bus.Can_Send(can_data[0], can_data[1])

    def Bootloader_Rec(self, timeout=2000):
        """
        获取下载模式下返回数据
        :param timeout: 超时时间，单位ms
        :return: 0：无返回；else：返回数据字节个数
        """
        if self.com_type == 0:
            self.receive_data = ''
            while True:
                _rec_flag = self.btl.PortReceive_Data(1, timeout)
                if _rec_flag:
                    if ck.CRC_8(self.receive_data) == self.btl.receive_buf:
                        self.receive_data += self.btl.receive_buf
                        return True
                    else:
                        self.receive_data += self.btl.receive_buf
                        continue
                else:
                    return False
        if self.can_type == 0:
            while True:
                if self.can_alyst.Can_Receive(timeout):
                    self.receive_data = self.Can_To_Ser_Cmd(self.can_alyst.can_recv_data)
                    if self.receive_data[4:10] == self.send_data[4:10]:
                        return True
                else:
                    return False
        else:
            while True:
                if self.can_bus.Can_Receive(timeout):
                    self.receive_data = self.Can_To_Ser_Cmd(self.can_bus.can_recv_data)
                    if self.receive_data[4:10] == self.send_data[4:10]:
                        return True
                else:
                    return False

    def Rec_Check(self, timeout=1000):
        """
        处理下载过程中返回数据
        :return: -1：失败；-2：成功；else：已接收完成的数据索引（在数据所以不连续才会返回该数据）
        """
        rec = self.Bootloader_Rec(timeout)
        if rec:
            answer_cmd = self.receive_data[10:10 + 8]
            state = int(self.receive_data[18:18 + 8], 16)
            if answer_cmd == '00000001':
                if state == 1:
                    return -2
            elif answer_cmd == '00000002':
                if state == 1:
                    return -2
                elif state == 2:
                    print('擦除大小超范围')
                else:
                    print('擦除失败')
            elif answer_cmd == '00000003':
                return state
            elif answer_cmd == '00000004':
                if state == 2:
                    return -2
        return -1

    def Bin_Get_Data(self):
        """
        获取bin文件十六进制数据并输出为列表模式，并且长度能够整除8
        :return: 列表：四个字节为一个字符串
        """
        self.file.seek(0)  # 将文件指针移动到固定地址偏移量处
        fin_data_lst = []
        for _ in range(0, self.file_size // 4):
            fin_data = ''
            data = self.file.read(4)
            value = hex(struct.unpack('I', data)[0]).replace('0x', '').zfill(8).upper()
            for va in range(0, len(value) + 1, 2):
                tmp = value[va:va + 2]
                fin_data = tmp + fin_data
            fin_data_lst.append(fin_data)
        if self.file_size % 8 != 0:
            fin_data_lst.append('FFFFFFFF')
        return fin_data_lst

    def Get_Send_Data(self):
        """
        以发送数据区格式获取发送数据列表
        :return: 列表：每个元素8个字节
        """
        send_str_lst = []
        bin_data = self.Bin_Get_Data()
        for count in range(0, len(bin_data), 2):  # 10673
            send_str = bin_data[count] + bin_data[count + 1]
            send_str_lst.append(send_str)
        return send_str_lst


def Get_File_Names(folder_path):
    file_names = []
    for item in os.listdir(folder_path):
        item_path = os.path.join(folder_path, item)
        if os.path.isfile(item_path):  # 只处理文件，忽略文件夹
            file_names.append(item)  # 将文件名添加到列表中
    return file_names[0]


def DownLoadPlus(dev_type: int, mode: int):
    """
    使用Download协议下载bin文件
    :param dev_type:
    1: 'ADP16',\n
    2: 'ADP18',\n
    3: '旋转阀',\n
    4: '旋转阀-编码器',\n
    5: '旋转阀-I2C',\n
    6: '旋转阀-VICI',\n
    7: 'DRL-计量泵',\n
    8: 'DRL-柱塞泵',\n
    9: 'DRS-柱塞泵',\n
    10: 'VLG-柱塞泵',\n
    11: 'VLG-PUSI-柱塞泵',\n
    :param mode: 0:下载测试程序；1：回退最近正式版
    :return:True：下载成功；False：下载失败
    """
    # 按设定获取文件绝对路径
    global file_bin_name_lst, file_get_flag
    dev_name = type_dic[dev_type]
    if file_get_flag == 0:
        file_bin_name_lst = Get_Latest_File(dev_name)
        file_get_flag = 1
    dl = DOWNLOAD_PLUS(file_bin_name_lst[mode])  # 定义download下载类变量
    send_data_lst = dl.Get_Send_Data()  # 获取需要发送的数据列表
    # 进入下载模式
    download_count = 0
    start_time = time.perf_counter()
    while True:
        dl.DownLoad_Send(dl.download_start_str)
        if dl.Rece_Check():
            break
        time.sleep(0.1)
        download_count += 1
        if download_count > 10:
            print('进入下载模式失败')
            return False
    time.sleep(1)
    # 擦除程序
    dl.DownLoad_Send(dl.Erase_Program_Config(dl.send_data_size))
    if not dl.Rece_Check():
        print('清除内存失败')
        return False
    print('清除内存成功')
    time.sleep(1)
    # 开始下载
    for num in range(0, len(send_data_lst)):
        dl.DownLoad_Send(dl.Write_Program_Config(send_data_lst[num]))
        if not dl.Rece_Check():
            print('下载失败')
            return False
        print(end="\r")
        print('下载进度：{0}%'.format(num * 100 // len(send_data_lst)), end='')
        dl.data_address += (len(send_data_lst[num]) // 2)  # 下载数据地址累加
        time.sleep(0.01)
    # 求32位和校验
    send_data_str = ''
    for sd_da_str in send_data_lst:
        send_data_str += sd_da_str
    check_sum_32 = ck.Uchar_Checksum_32_Bit(send_data_str)
    # 发送校验数据
    dl.DownLoad_Send(dl.Check_Program_Config(check_sum_32))
    if not dl.Rece_Check():
        print('校验码失败:', check_sum_32)
        return False
    end_time = time.perf_counter()
    print(end="\r")
    print('下载进度：100%')
    print('下载程序完成，用时:{0}秒'.format(round(end_time - start_time, 2)))
    dl.dlp.ClosePort()
    return True


def Bootloader(dev_type: int, mode: int, interface=0):
    """
    Bootloader协议升级
    :param dev_type: 1:SP16;2:SP18;3:STEPCG;4:STEPIA02;5:STEPVLG
    :param mode: 0:下载测试程序；1：回退最近正式版
    :param interface:0:串口升级；1：can升级
    :return: True：升级成功；False：升级失败
    """
    # 按设定获取文件绝对路径
    dev_name = type_dic[dev_type]
    file_bin_name_lst = Get_Latest_File(dev_name)
    btl = BOOTLOADER(file_bin_name_lst[mode], interface)  # 定义Bootloader类变量
    if interface == 1:
        btl.Check_Can_Type()
    send_data_lst = btl.Bootloader_Write_Cmd()  # 生成发送数据列表
    # 生成发送数据字典{ 索引：帧数据}
    send_data_dic = {}
    for w_cmd_num in range(0, len(send_data_lst)):
        send_data_dic[w_cmd_num] = send_data_lst[w_cmd_num]
    # 生成crc32校验码
    crc32 = btl.Bootloader_Check_Cmd()
    # 进入下载模式
    download_count = 0
    success_count = 0
    start_time = time.perf_counter()
    while True:
        btl.Bootloader_Send(btl.Bootloader_Enter_Cmd())
        if btl.Rec_Check() == -2:
            success_count += 1
            if success_count == 8:
                print('进入下载模式成功')
                break
        else:
            download_count += 1
        if download_count > 10:
            print('进入下载模式失败')
            return False
        time.sleep(0.5)
    time.sleep(1)
    # 发送升级数据长度，板卡擦除相应长度内存
    btl.Bootloader_Send(btl.Bootloader_DataLen_cmd())
    if btl.Rec_Check() == -1:
        print('清除内存失败')
        return False
    print('清除内存成功')
    time.sleep(1)
    # 开始下载程序
    idex = 0
    global rec_flag
    global done_flag
    rec_flag = 0
    done_flag = 0
    th1 = MyThread(btl)
    if interface == 0:
        th1.start()
    while True:
        finish_rate = idex * 100 // len(send_data_lst)
        print(end="\r")
        print('下载进度：{0}%'.format(finish_rate), end='')
        btl.Bootloader_Send(send_data_dic[idex])
        if rec_flag == 1:
            check_idex = btl.Rec_Check(1000)
            if check_idex > -1:
                idex = check_idex
                print('重发索引：', idex)
                rec_flag = 0
                continue
        if interface == 0:
            time.sleep(0.02)  # 延时20ms
        idex += 1
        if idex == len(send_data_lst):
            done_flag = 1
            break
        time.sleep(0.001)
    if interface == 0:
        th1.join()
    time.sleep(1)
    # 发送校验数据
    check_count = 0
    while True:
        btl.Bootloader_Send(crc32)
        if btl.Rec_Check() == -2:
            break
        check_count += 1
        if check_count == 10:
            print('校验失败')
            return False
        time.sleep(0.5)
    end_time = time.perf_counter()
    print(end="\r")
    print('下载进度：100%')
    print('数据校验成功')
    print('下载程序完成，用时:{0}秒'.format(round(end_time - start_time, 2)))
    btl.btl.ClosePort()
    return True


if __name__ == '__main__':
    sp.Reset_Ser_Baud(0,'com36', 9600)  # 设置串口
    canalyst.canalyst.baudrate = 500  # 设置波特率
    DownLoadPlus(7, 0)
