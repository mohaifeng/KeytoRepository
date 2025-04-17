import datetime
import sys
import time
from ctypes import *
import ctypes
from typing import Any
import threading

CanDLLName = './madp_dll.dll'  # 把DLL放到对应的目录下
canDLL = windll.LoadLibrary('./madp_dll.dll')

# DevType:设备类型。对应不同的产品型号详见：适配器设备类型定义。
# DevIndex:设备索引，比如当只有一个USB-CAN适配器时，索引号为0，这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1，以此类推。
# CANIndex:CAN通道索引。第几路 CAN。即对应卡的CAN通道号，CAN1为0，CAN2为1。
DevType = 0  # 设备类型
DevIndex = 0  # 设备索引
CANIndex = 0  # CAN通道:0


class VCI_INIT_CONFIG(Structure):
    _fields_ = [("AccCode", c_uint),
                ("AccMask", c_uint),
                ("Reserved", c_uint),
                ("Filter", c_ubyte),
                ("Timing0", c_ubyte),
                ("Timing1", c_ubyte),
                ("Mode", c_ubyte)
                ]


def Check_State(_id: int):
    while True:
        state = canDLL.ktcan_readActionFinish(_id)
        if state == 1:
            continue
        elif state == 0:
            return 0
        return state


class myThread(threading.Thread):
    def __init__(self, threadID):
        super(myThread, self).__init__()
        self.threadID = threadID

    def run(self):
        print('Starting ' + 'check state')
        if Check_State(adp_id) == 0:
            print('Exiting ' + 'state idle')
        else:
            print('Exiting ' + 'state error')
            flag = input('是否继续？0：停止；1：继续')
            if flag == 0:
                sys.exit()


if __name__ == "__main__":
    th1 = myThread('1')
    canDLL.openDebug(0)
    adp_id = 1
    canDLL.ktcan_link(DevType, DevIndex, CANIndex, 500)
    if not canDLL.ktcan_isOpen():
        print('CAN未连接')
        sys.exit()
    canDLL.adp_init(adp_id, 500, 100, 0)
    th1.start()
    th1.join()
