::�رջ��ԣ�����ʾ����ִ�е����������ִ�еĽ���ȡ�
@echo off
::�л���UTF-8����ҳ������������ʾ����
::chcp 65001
chcp 936
cd /d C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin
STM32_Programmer_CLI.exe -c port=SWD freq=4000 -rdu
STM32_Programmer_CLI.exe -c port=SWD freq=4000 -w D:\keyto\project\PCsoftware\���ʽ���س���\Keyto-SADP16-G4_BL-V1.0.4.hex -v -ob rdp=0xBB

::TIMEOUT /T 30

:: ^���������
if %errorlevel%==0 (
   ECHO ���سɹ�
) else (
   ECHO ����ʧ��
)
pause
