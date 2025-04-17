import Excel.file_handle as fa
import Adp.pipette as pi
import openpyxl
from tkinter import *


def EXECUTE_COMMAND():
    text_console.insert('end', adp.AdpSend(str(lb_command.get(lb_command.curselection()))[:]))
    text_console.insert('end', '\n')
    rec_datas = adp.AdpReceive(500)
    if rec_datas != 0:
        text_console.insert('end', rec_datas)
        text_console.insert('end', '\n')
    else:
        text_console.insert('end', 'Receive_dt_data:No Receive Data!')
        text_console.insert('end', '\n')
    text_console.see(END)


fina_observe_result_lst = []


def PASS_COMMAND():
    observe_result_tmp = observe_result_lines[ts].split(']Yes')
    print(observe_result_tmp)
    observe_result_tmp_str = observe_result_tmp[0] + '√]Yes' + observe_result_tmp[1]
    observe_result_tmp_lst = observe_result_tmp_str.split('？')

    for fina_lines in observe_result_tmp_lst:
        fina_observe_result_lst.append(fina_lines)
    fina_str = '\n'.join(fina_observe_result_lst)
    sheet = test_workbook.active
    sheet[observe_result[r]] = fina_str


def NO_PASS_COMMAND():
    pass


def REPORTING():
    test_workbook.save('SADP16TEST_REPORT.xlsx')  # 新建测试报告文档


if __name__ == '__main__':
    workbook = openpyxl.load_workbook('DOC2111001SADP16TEST_CASE.xlsx')  # 返回一个workbook数据类型的值
    adp = pi.ADP()
    workbook.save('SADP16TEST_REPORT.xlsx')  # 新建测试报告文档
    test_workbook = openpyxl.load_workbook('SADP16TEST_REPORT.xlsx')  # 返回一个workbook数据类型的值
    test_report = fa.EXCEL(r'D:\code\python_Auto_Test_Project', 'SADP16TEST_REPORT.xlsx')
    test_report.SHEET_ATTRIBUTE('KT-OEM指令测试')
    test_step = []
    func_type = []
    expected_results = []
    observe_result = []
    for i in range(2, test_report.row + 1):
        tsr = 'F' + str(i)
        test_step.append(tsr)
        ft = 'B' + str(i)
        func_type.append(ft)
        er = 'G' + str(i)
        expected_results.append(er)
        ore = 'H' + str(i)
        observe_result.append(ore)
    root = Tk()  # 生成一个主窗口对象
    root.title('KT-OEM指令测试')
    root.geometry('1500x800')
    label_func_type_title = Label(root, text='功能需求', width=20, height=1, relief='sunken',
                                  justify=LEFT)
    label_cell_str_title = Label(root, text='测试步骤', width=130, height=1, relief='sunken', justify=LEFT)
    label_step_str_lines_title = Label(root, text='执行步骤', width=20, height=10, relief='sunken', justify=LEFT)
    label_command_title = Label(root, text='指令', width=20, height=1, relief='sunken', justify=LEFT)
    label_console_title = Label(root, text='控制台', width=100, height=1, relief='sunken', justify=LEFT)
    label_result_title = Label(root, text='期望结果', width=55, height=1, relief='sunken', justify=LEFT)
    text_console = Text(root, font=16, width=100, height=10)  # 控制台发送接收显示
    text_result = Text(root, font=16, width=50, height=10)  # 期望结果显示
    text_step_str_lines = Text(root, font=16, width=100, height=10)  # 单步步骤显示
    # text_result.configure(font=16)
    button1 = Button(root, text="下一步", width=10, height=5, command=root.quit)  # 创建按钮下一步
    button2 = Button(root, text="执行", width=10, height=2, command=EXECUTE_COMMAND)  # 创建按钮执行
    button3 = Button(root, text="通过", width=10, height=2, justify=RIGHT, command=PASS_COMMAND)
    button4 = Button(root, text="不通过", width=10, height=2, justify=LEFT, command=NO_PASS_COMMAND)
    button5 = Button(root, text="生成报告", width=10, height=2, justify=LEFT, command=REPORTING)
    text_step_str_lines.grid(row=3, column=1)
    button1.grid(row=3, column=2)  # 将按钮1添加到窗口中
    button2.grid(row=6, column=0)
    button3.grid(row=6, column=2)
    button4.grid(row=6, column=3)
    button5.grid(row=1, column=2)
    label_result_title.grid(row=4, column=2, columnspan=3)
    label_func_type_title.grid(row=0, column=0)
    label_cell_str_title.grid(row=0, column=1)
    label_step_str_lines_title.grid(row=3, column=0)
    label_command_title.grid(row=4, column=0)
    label_console_title.grid(row=4, column=1)
    text_console.grid(row=5, column=1)
    text_result.grid(row=5, column=2, columnspan=3)

    text_console.delete('1.0', END)
    open_message = adp.adp_ser.OpenPort()
    # text_console.insert('end', open_message)
    # text_console.insert('end', '\n')  # 实现换行
    for r in range(0, len(test_step)):
        func_type_str = test_report.READ_CELL(func_type[r])  # 功能需求

        observe_result_str = test_report.READ_CELL(observe_result[r])
        observe_result_lines = test_report.READ_CELL_LINES()  # 观察结果

        expect_result_str = test_report.READ_CELL(expected_results[r])
        expect_result_lines = test_report.READ_CELL_LINES()  # 期望结果

        test_step_str = test_report.READ_CELL(test_step[r])  # 测试步骤
        # print('test_step_str', test_step_str)
        test_step_str_lines = test_report.READ_CELL_LINES()
        # print('test_step_str_lines', test_step_str_lines)
        command_str = test_report.EXTRACT_SEND_COMMAND()
        # 功能需求显示
        label_func_type = Label(root, text=func_type_str, width=20, height=20, relief='sunken', justify=CENTER)
        # 测试步骤显示
        label_cell_str = Label(root, text=test_step_str, anchor='w', bg='white', width=130, height=20, relief='sunken',
                               wraplength=800, justify=LEFT)
        label_func_type.grid(row=1, column=0)
        label_cell_str.grid(row=1, column=1)  # 将标签添加到窗口中

        for ts in range(1, len(test_step_str_lines) + 1):
            # 单步执行步骤显示
            text_step_str_lines.delete('1.0', END)
            text_step_str_lines.insert('end', test_step_str_lines[ts])
            text_result.delete('1.0', END)
            if ts in expect_result_lines.keys():
                text_result.insert('end', expect_result_lines[ts])
                text_result.insert('end', '\n')  # 实现换行

            lb_command = Listbox(root)
            if '重复执行' in test_step_str_lines[ts]:
                repeat_num = int(test_step_str_lines[ts].split("步骤")[1].split("重复执行")[0])  # 提取重复步骤序号
                for rm in command_str[repeat_num]:
                    lb_command.insert(END, rm)
            if ts in command_str.keys():
                for lb in command_str[ts]:
                    lb_command.insert(END, lb)
            lb_command.grid(row=5, column=0)

            # button2 = Button(root, text="否")  # 创建按钮2
            # button2.grid(row=3, column=3)  # 将按钮2添加到窗口中
            root.mainloop()  # 进入消息循环
