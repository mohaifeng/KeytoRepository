::关闭回显，不显示正在执行的批处理命令及执行的结果等。
@echo off
::切换到UTF-8代码页，否则中文显示乱码
::chcp 65001
chcp 936
cd /d C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin
STM32_Programmer_CLI.exe -c port=SWD freq=4000 -rdu
STM32_Programmer_CLI.exe -c port=SWD freq=4000 -w D:\keyto\project\PCsoftware\命令方式下载程序\Keyto-SADP16-G4_BL-V1.0.4.hex -v -ob rdp=0xBB

::TIMEOUT /T 30

:: ^允许命令换行
if %errorlevel%==0 (
   ECHO 下载成功
) else (
   ECHO 下载失败
)
pause
