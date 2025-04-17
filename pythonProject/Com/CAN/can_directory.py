import Com.Protocol.protocol as pr

frame_id_type = {
    'response': '0000',
    'write': '0001',
    'read': '0002',
    'process': '0003',
    'heart': '0004',
    'warning': '0080'
}


class ADP_CAN_DIC:
    def __init__(self):
        self.addr = 1
        self.can_prot = pr.keyto_can_dic(self.addr)
        self.fix_par = 123456
        self.adp16_index = {
            '置零': '4000',
            '吸液': '4001',
            '排液': '4002',
            '绝对位移': '4002',
            '相对上移': '4004',
            '相对下移': '4005',
            '退TIP': '4006',
            '液面探测': '4007',
            '排空': '4009',
            '停止': '4008',
            '抗悬滴': '4010',
            '吸液跟随': '4011',
            '排液跟随': '4012',
            '滤芯检测': '4020'
        }
        self.sys_can_index = {
            '查询状态': ['2000', '1'],
            '设备类型': ['9F00', '0'],
            '急停': ['9F00', '1'],
            '心跳间隔时间': ['9F00', '2'],
            '重启节点': ['9F00', '3'],
            '软件版本': ['9F00', '4'],
            '主动上报': ['9F00', '4'],
            '掉电保存': ['9F10', '0'],
            '恢复出厂设置': ['9F10', '1'],
        }  # 通用字典，格式[主索引，子索引]

    def Contr_Cmd_Conf(self, cmd: str, *args: int):
        par_lst = []
        if len(args) > 0:
            for x in range(0, len(args)):
                tmp_dat = self.can_prot.Frame_Cmd(cmd, hex(x).replace('0x', ''), args[x])
                par_lst.append(tmp_dat)
            par_lst = par_lst[1:] + par_lst[:1]
        return par_lst

    def Sys_Cmd_Conf(self, cmd: list, dat: int):
        return [self.can_prot.Frame_Cmd(cmd[0], cmd[1], dat)]

    def Can_It_Cmd(self, speed=500, power=100, enable=0):
        return self.Contr_Cmd_Conf(self.adp16_index['置零'], speed, power, enable)

    def Can_Ia_Cmd(self, aspir_vol, start_speed=500, stop_speed=10, compensate=0):
        return self.Contr_Cmd_Conf(self.adp16_index['吸液'], aspir_vol, start_speed, stop_speed, compensate)

    def Can_Da_Cmd(self, dis_vol, re_aspir=0, start_speed=500, stop_speed=10):
        return self.Contr_Cmd_Conf(self.adp16_index['排液'], dis_vol, re_aspir, start_speed, stop_speed)

    def Can_Dt_Cmd(self, speed=500, enable=0):
        return self.Contr_Cmd_Conf(self.adp16_index['退TIP'], speed, enable)

    def Can_Ld_Cmd(self, repo_mode=1, timeout=10000, plld_mode=1):
        return self.Contr_Cmd_Conf(self.adp16_index['液面探测'], repo_mode, timeout, plld_mode)

    def Can_Mp_Cmd(self, absol_pos, start_speed=128000, stop_speed=32000):
        return self.Contr_Cmd_Conf(self.adp16_index['绝对位移'], absol_pos, start_speed, stop_speed)

    def Can_Up_Cmd(self, rela_pos, start_speed=128000, stop_speed=32000):
        return self.Contr_Cmd_Conf(self.adp16_index['相对上移'], rela_pos, start_speed, stop_speed)

    def Can_Dp_Cmd(self, rela_pos, start_speed=128000, stop_speed=32000, re_aspiration=0):
        return self.Contr_Cmd_Conf(self.adp16_index['相对下移'], rela_pos, start_speed, stop_speed, re_aspiration)

    def Can_Pc_Cmd(self, pc_mode=0, speed=200, max_position=50):
        return self.Contr_Cmd_Conf(self.adp16_index['抗悬滴'], pc_mode, speed, max_position)

    def Can_Iz_Cmd(self, aspir_vol, start_speed=100, surface_area=78, z_position=0):
        return self.Contr_Cmd_Conf(self.adp16_index['吸液跟随'], aspir_vol, start_speed, surface_area, z_position)

    def Can_Dz_Cmd(self, disc_vol, start_speed=100, surf_ar=78):
        return self.Contr_Cmd_Conf(self.adp16_index['排液跟随'], disc_vol, start_speed, surf_ar)

    def Can_De_Cmd(self, start_speed=500):
        return self.Contr_Cmd_Conf(self.adp16_index['排空'], start_speed)

    def Can_Dc_Cmd(self, dat=0):
        return self.Contr_Cmd_Conf(self.adp16_index['滤芯检测'], dat)

    def Can_Stop_Cmd(self, dat=0):
        return self.Contr_Cmd_Conf(self.adp16_index['停止'], dat)

    def Can_Check_State_Cmd(self, dat=0):
        return self.Sys_Cmd_Conf(self.sys_can_index['查询状态'], dat)

    def Can_Dev_Type_Cmd(self, dat=0):
        return self.Sys_Cmd_Conf(self.sys_can_index['设备类型'], dat)

    def Can_Sys_Stop_Cmd(self, dat=0):
        return self.Sys_Cmd_Conf(self.sys_can_index['急停'], dat)

    def Can_Heart_Cmd(self, time=0):
        return self.Sys_Cmd_Conf(self.sys_can_index['心跳间隔时间'], time)

    def Can_Restart_Cmd(self, dat=0):
        return self.Sys_Cmd_Conf(self.sys_can_index['重启节点'], dat)

    def Can_Version_Cmd(self, dat=0):
        return self.Sys_Cmd_Conf(self.sys_can_index['软件版本'], dat)

    def Can_Report_Cmd(self, mode=0):
        return self.Sys_Cmd_Conf(self.sys_can_index['主动上报'], mode)

    def Can_Save_Cmd(self, dat=0):
        return self.Sys_Cmd_Conf(self.sys_can_index['掉电保存'], dat)

    def Can_Restore_Cmd(self, dat=123456):
        return self.Sys_Cmd_Conf(self.sys_can_index['恢复出厂设置'], dat)


if __name__ == '__main__':
    adp_can_dic = ADP_CAN_DIC()
    print(adp_can_dic.Can_Check_State_Cmd())
