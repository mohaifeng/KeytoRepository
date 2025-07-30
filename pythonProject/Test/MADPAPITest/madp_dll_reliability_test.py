import sys
import threading

from madp_dll_repackage import *

test_idex = 0
group0 = 0
group1 = 1
group2 = 2
group3 = 3


def Config_Result(result):
    if not result:
        sys.exit()


# 0,1为ADP1,2/2，3为Z轴41，42

def Thread0_Task():
    global test_idex, group0
    match test_idex:
        case 0:
            Config_Result(Cmd_E(group0, '1It500,100,0', True))
        case 1:
            Config_Result(Cmd_Transmit_wait_ack(group0, 'E', '1It500,100,0'))
            Config_Result(Wait_Cmd_Finish(group0))
        case 2:
            Config_Result(RW_Register(group0, '1Rr90'))
            Config_Result(RW_Register(group0, '1Wr54,20'))
        case 3:
            Config_Result(Check_SysStatus(group0))
        case 4:
            Config_Result(Cmd_Transmit_wait_ack(group0, 'E', '41It500,100,0'))
            Config_Result(Get_Error_Msg(group0))
        case 5:
            Config_Result(Cmd_t(group0))
        case 6:
            Config_Result(Group_Adp_Init(group0, [1], True))
        case 7:
            Config_Result(Group_Adp_Aspirate(group0, [1], True, 200))
        case 8:
            Config_Result(Group_Adp_Dispense(group0, [1], True, 200))
        case 9:
            Config_Result(Group_Adp_Move(group0, [1], True, 200))
        case 10:
            Config_Result(Group_Adp_PushTip(group0, [1], True))


def Thread1_Task():
    global test_idex, group1
    match test_idex:
        case 0:
            Config_Result(Cmd_E(group1, '2It500,100,0', True))
        case 1:
            Config_Result(Cmd_Transmit_wait_ack(group1, 'E', '2It500,100,0'))
            Config_Result(Wait_Cmd_Finish(group1))
        case 2:
            Config_Result(RW_Register(group1, '2Rr90'))
            Config_Result(RW_Register(group1, '2Wr54,20'))
        case 3:
            Config_Result(Check_SysStatus(group1))
        case 4:
            Config_Result(Cmd_Transmit_wait_ack(group1, 'E', '41It500,100,0'))
            Config_Result(Get_Error_Msg(group1))
        case 5:
            Config_Result(Cmd_t(group1))
        case 6:
            Config_Result(Group_Adp_Init(group1, [2], True))
        case 7:
            Config_Result(Group_Adp_Aspirate(group1, [2], True, 200))
        case 8:
            Config_Result(Group_Adp_Dispense(group1, [2], True, 200))
        case 9:
            Config_Result(Group_Adp_Move(group1, [2], True, 200))
        case 10:
            Config_Result(Group_Adp_PushTip(group1, [2], True))


def Thread2_Task():
    global test_idex, group2
    match test_idex:
        case 0:
            Config_Result(Cmd_E(group2, '41Zz50000', True))
        case 1:
            Config_Result(Cmd_Transmit_wait_ack(group2, 'E', '41Zz50000'))
            Config_Result(Wait_Cmd_Finish(group2))
        case 2:
            Config_Result(RW_Register(group2, '41Rr101'))
            Config_Result(RW_Register(group2, '41Wr107,0'))
        case 3:
            Config_Result(Check_SysStatus(group2))
        case 4:
            Config_Result(Cmd_Transmit_wait_ack(group2, 'E', '41It500,100,0'))
            Config_Result(Get_Error_Msg(group2))
        case 5:
            Config_Result(Cmd_t(group2))
        case 6:
            Config_Result(Group_Z_Init(group2, [41], True))
        case 7:
            Config_Result(Group_Z_MovePos(group2, [41], True, 30))
        case 8:
            Config_Result(Group_Z_MoveUp(group2, [41], True, 30))
        case 9:
            Config_Result(Group_Z_MoveDown(group2, [41], True, 30))
        case 10:
            Config_Result(Group_Z_PickUpTip(group2, [41], True))


def Thread3_Task():
    global test_idex, group3
    match test_idex:
        case 0:
            Config_Result(Cmd_E(group3, '42Zz50000', True))
        case 1:
            Config_Result(Cmd_Transmit_wait_ack(group3, 'E', '42Zz50000'))
            Config_Result(Wait_Cmd_Finish(group3))
        case 2:
            Config_Result(RW_Register(group3, '42Rr101'))
            Config_Result(RW_Register(group3, '42Wr107,0'))
        case 3:
            Config_Result(Check_SysStatus(group3))
        case 4:
            Config_Result(Cmd_Transmit_wait_ack(group3, 'E', '41It500,100,0'))
            Config_Result(Get_Error_Msg(group3))
        case 5:
            Config_Result(Cmd_t(group3))
        case 6:
            Config_Result(Group_Z_Init(group3, [42], True))
        case 7:
            Config_Result(Group_Z_MovePos(group3, [42], True, 30))
        case 8:
            Config_Result(Group_Z_MoveUp(group3, [42], True, 30))
        case 9:
            Config_Result(Group_Z_MoveDown(group3, [42], True, 30))
        case 10:
            Config_Result(Group_Z_PickUpTip(group3, [42], True))


def worker1(num):
    """线程执行的任务"""
    match num:
        case 0:
            Thread0_Task()
        case 1:
            Thread1_Task()
        case 2:
            Thread2_Task()
        case 3:
            Thread3_Task()


if __name__ == '__main__':
    if not Sys_Open_Port('com36', 38400):
        sys.exit()
    Sys_Config(True, False, False, False, True, True)
    # 创建线程
    threads = []
    while True:
        for i in range(0, 4):
            t = threading.Thread(target=worker1, args=(i,))
            threads.append(t)
            t.start()
        for thread in threads:
            thread.join()
        test_idex += 1
        if test_idex > 10:
            test_idex = 0
