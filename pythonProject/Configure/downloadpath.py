import os

dec_bin_type_dic = {
    'ADP16': r'\项目\ADP\SADP16\客户升级',
    'ADP18': r'\项目\ADP\SADP18\客户升级',
    '旋转阀': r'\项目\旋切阀\RotaryValveG431-TMC5160\客户升级',
    '旋转阀-编码器': r'\项目\旋切阀\EncoderRotaryValveG431-TMC5160\客户升级',
    '旋转阀-I2C': r'\项目\旋切阀\STEPIA\客户升级',
    '旋转阀-VICI': r'\项目\旋切阀\EncoderRotaryValve-VICI\客户升级',
    'DRL-计量泵': r'\项目\阀泵集成\stepdrl-计量泵\客户升级',
    'DRL-柱塞泵': r'\项目\阀泵集成\stepdrl\客户升级',
    'DRS-柱塞泵': r'\项目\阀泵集成\stepdrs\客户升级',
    'VLG-柱塞泵': r'\项目\阀泵集成\stepvl-G431\客户升级',
    'VLG-PUSI-柱塞泵': r'\项目\阀泵集成\stepvl-G431-pusi\客户升级',
}
dec_hex_type_dic = {
    'ADP16': r'\项目\ADP\SADP16\生产下载',
    'ADP18': r'\项目\ADP\SADP18\生产下载',
    '旋转阀': r'\项目\旋切阀\RotaryValveG431-TMC5160\生产出厂',
    '旋转阀-编码器': r'\项目\旋切阀\EncoderRotaryValveG431-TMC5160\生产出厂',
    '旋转阀-I2C': r'\项目\旋切阀\STEPIA\生产下载',
    '旋转阀-VICI': r'\项目\旋切阀\EncoderRotaryValve-VICI\生产下载',
    'DRL-计量泵': r'\项目\阀泵集成\stepdrl-计量泵\生产出厂',
    'DRL-柱塞泵': r'\项目\阀泵集成\stepdrl\生产出厂',
    'DRS-柱塞泵': r'\项目\阀泵集成\stepdrs\生产下载',
    'VLG-柱塞泵': r'\项目\阀泵集成\stepvl-G431\生产出厂',
    'VLG-PUSI-柱塞泵': r'\项目\阀泵集成\stepvl-G431-pusi\生产出厂',
}
confirm_flag = 0


def Get_Latest_Hex_File(dev_type):
    """
    确认下载版本与回退版本
    :param dev_type:
    ADP16',\n
    ADP18',\n
    旋转阀',\n
    旋转阀-编码器',\n
    旋转阀-I2C',\n
    旋转阀-VICI',\n
    DRL-计量泵',\n
    DRL-柱塞泵',\n
    DRS-柱塞泵',\n
    VLG-柱塞泵',\n
    VLG-PUSI-柱塞泵',\n
    :return: 列表：[下载版本路径，回退版本路径]
    """
    with open(r'D:\code\pythonProject\Configure\svn_path.txt', 'r', encoding='utf-8') as file:
        content = file.read().split('=')[1]
    folder_path = content + dec_hex_type_dic[dev_type]
    # 获取文件夹内所有文件（排除子目录）
    files = [f for f in os.listdir(folder_path) if os.path.isfile(os.path.join(folder_path, f))]
    if not files:
        return None  # 如果文件夹为空
    # 按修改时间排序（降序，最新文件在前）
    files.sort(key=lambda f: os.path.getmtime(os.path.join(folder_path, f)), reverse=True)
    # 返回最新文件的完整路径
    confirm_count = False
    test_file = ''
    pre_file = ''
    for file in files:
        if not confirm_count:
            is_ok = input('下载hex文件为：' + file + '\n' + '是否确认？(Y/N):')
            if is_ok == 'Y':
                test_file = file
                confirm_count = True
                continue
        else:
            if 'beta' not in file and 'dz' not in file:
                is_ok = input('上个hex正式版为：' + file + '\n' + '是否确认？(Y/N):')
                if is_ok == 'Y':
                    pre_file = file
                    break
    return [os.path.join(folder_path, test_file), os.path.join(folder_path, pre_file)]


def Get_Latest_Bin_File(dev_type):
    """
    确认下载版本与回退版本
    :param dev_type:
    ADP16',\n
    ADP18',\n
    旋转阀',\n
    旋转阀-编码器',\n
    旋转阀-I2C',\n
    旋转阀-VICI',\n
    DRL-计量泵',\n
    DRL-柱塞泵',\n
    DRS-柱塞泵',\n
    VLG-柱塞泵',\n
    VLG-PUSI-柱塞泵',\n
    :return: 列表：[下载版本路径，回退版本路径]
    """
    with open(r'D:\code\pythonProject\Configure\svn_path.txt', 'r', encoding='utf-8') as file:
        content = file.read().split('=')[1]
    folder_path = content + dec_bin_type_dic[dev_type]
    # 获取文件夹内所有文件（排除子目录）
    files = [f for f in os.listdir(folder_path) if os.path.isfile(os.path.join(folder_path, f))]
    if not files:
        return None  # 如果文件夹为空
    # 按修改时间排序（降序，最新文件在前）
    files.sort(key=lambda f: os.path.getmtime(os.path.join(folder_path, f)), reverse=True)
    # 返回最新文件的完整路径
    confirm_count = True
    test_file = ''
    pre_file = ''
    for file in files:
        if confirm_count:
            is_ok = input('下载bin文件为：' + file + '\n' + '是否确认？(Y/N):')
            if is_ok == 'Y':
                test_file = file
                confirm_count = False
                continue
        else:
            if 'beta' not in file and 'dz' not in file:
                is_ok = input('上个bin正式版为：' + file + '\n' + '是否确认？(Y/N):')
                if is_ok == 'Y':
                    pre_file = file
                    break
    return [os.path.join(folder_path, test_file), os.path.join(folder_path, pre_file)]


if __name__ == '__main__':
    print(Get_Latest_Hex_File('ADP16'))
