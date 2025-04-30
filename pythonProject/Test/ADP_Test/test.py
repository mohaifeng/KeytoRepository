import subprocess
import os
from Configure.downloadpath import Get_Latest_Hex_File


def program_stm32_swd(hex_file_path, stm32_programmer_path=None):
    """
    使用STM32_Programmer_CLI通过SWD接口烧录HEX文件

    参数:
        hex_file_path: HEX文件的完整路径
        stm32_programmer_path: STM32_Programmer_CLI.exe的路径(如果不在系统PATH中)
    """
    # 默认路径 - 根据你的安装位置调整
    if stm32_programmer_path is None:
        stm32_programmer_path = r"D:\Download\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"

    if not os.path.exists(hex_file_path):
        raise FileNotFoundError(f"HEX文件未找到: {hex_file_path}")

    if not os.path.exists(stm32_programmer_path):
        raise FileNotFoundError(f"STM32_Programmer_CLI未找到: {stm32_programmer_path}")

    # 构建命令
    command = [
        stm32_programmer_path,
        "-c", "port=SWD",  # 使用SWD接口
        "--erase", "all",  # 完全擦除Flash
        "-w", hex_file_path,  # 要写入的HEX文件
        "0x08000000",  # 起始地址(Flash起始地址)
        "-v",  # 验证写入
        "-s",  # 开始执行程序
        "-rst"  # 下载后硬件复位
    ]

    try:
        # 执行命令
        result = subprocess.run(command, check=True, capture_output=True, text=True, encoding='utf-8',errors='ignore')
        print("烧录成功!")
        print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print("烧录失败!")
        print("错误输出:")
        print(e.stderr)
        raise


# 使用示例
if __name__ == "__main__":
    hex_file = Get_Latest_Hex_File('旋转阀')[0]
    program_stm32_swd(hex_file)
