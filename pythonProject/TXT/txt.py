import tkinter as tk


def cp():
    print("Copy")


#
# def item_click(lb):
#     idex = lb.curselection()
#     print(idex[0] + 1)
#
#
# def CB_Click():
#     print([var1.get(), var2.get(), var3.get(), var4.get(), var5.get()])


#
class App(tk.Tk):
    def __init__(self):
        tk.Tk.__init__(self)
        self.geometry("400x400")
        self.title("程序下载测试")
        # self.iconbitmap("icon.ico")
        self.var_9600 = tk.BooleanVar()
        self.var_19200 = tk.BooleanVar()
        self.var_38400 = tk.BooleanVar()
        self.var_57600 = tk.BooleanVar()
        self.var_115200 = tk.BooleanVar()
        self.dev_type = ["SP16", "SP18", "STEPCG", "STEPIA02", "STEPVLG"]
        self.dev = ''

    def BaudRate_Select(self):
        # 创建复选按钮组件
        tk.Checkbutton(self.master, text="9600", variable=self.var_9600, command=self.CB_Click).grid(row=0, column=1,
                                                                                                     columnspan=2)
        tk.Checkbutton(self.master, text="19200", variable=self.var_19200, command=self.CB_Click).grid(row=1, column=1,
                                                                                                       columnspan=2)
        tk.Checkbutton(self.master, text="38400", variable=self.var_38400, command=self.CB_Click).grid(row=2, column=1,
                                                                                                       columnspan=2)
        tk.Checkbutton(self.master, text="57600", variable=self.var_57600, command=self.CB_Click).grid(row=3, column=1,
                                                                                                       columnspan=2)
        tk.Checkbutton(self.master, text="115200", variable=self.var_115200, command=self.CB_Click).grid(row=4,
                                                                                                         column=1,
                                                                                                         columnspan=2)

    def Dev_Type_Select(self):
        dev_type_box = tk.Listbox(self.master)
        dev_type_box.grid(row=0, column=0, rowspan=5, columnspan=1)
        for item in self.dev_type:
            dev_type_box.insert(tk.END, item)
        dev_type_box.bind('<<ListboxSelect>>', lambda event: self.Box_click(dev_type_box))

    def Bt_Select(self):
        tk.Button(self.master, text="开始测试", command=self.Bt_Click).grid(row=0, column=3, rowspan=5)

    def CB_Click(self):
        print([self.var_9600.get(), self.var_19200.get(), self.var_38400.get(), self.var_57600.get(),
               self.var_115200.get()])

    def Box_click(self, lb: tk.Listbox):
        self.dev = lb.curselection()[0] + 1  # 返回选择的索引：1:SP16;2:SP18;3:STEPCG;4:STEPIA02;5:STEPVLG
        print(self.dev)

    def Bt_Click(self):
        print(1)


if __name__ == '__main__':
    app = App()
    app.BaudRate_Select()
    app.Dev_Type_Select()
    app.Bt_Select()
    app.mainloop()
    # dev_type = ["SP16", "SP18", "STEPCG", "STEPIA02", "STEPVLG"]
    #
    # # 创建窗口对象
    # window = tk.Tk()
    # window.title('程序下载测试')
    # window.geometry('500x300')
    # # 创建布尔变量
    # var1 = tk.BooleanVar()
    # var2 = tk.BooleanVar()
    # var3 = tk.BooleanVar()
    # var4 = tk.BooleanVar()
    # var5 = tk.BooleanVar()
    #
    # # 创建复选按钮组件
    # cb_9600 = tk.Checkbutton(window, text="9600", variable=var1, command=CB_Click)
    # cb_9600.grid(row=0, column=1, columnspan=2)
    # cb_19200 = tk.Checkbutton(window, text="19200", variable=var2, command=CB_Click)
    # cb_19200.grid(row=1, column=1, columnspan=2)
    # cb_38400 = tk.Checkbutton(window, text="38400", variable=var3, command=CB_Click)
    # cb_38400.grid(row=2, column=1, columnspan=2)
    # cb_57600 = tk.Checkbutton(window, text="57600", variable=var4, command=CB_Click)
    # cb_57600.grid(row=3, column=1, columnspan=2)
    # cb_115200 = tk.Checkbutton(window, text="115200", variable=var5, command=CB_Click)
    # cb_115200.grid(row=4, column=1, columnspan=2)
    # # 创建列表对象
    # dev_type_box = tk.Listbox(window)
    # dev_type_box.grid(row=0, column=0, rowspan=5, columnspan=1)
    # for item in dev_type:
    #     dev_type_box.insert(tk.END, item)
    # dev_type_box.bind('<<ListboxSelect>>', lambda event: item_click(dev_type_box))
    # but1 = tk.Button(window, text="开始测试", command=window.quit)
    # but1.grid(row=0, column=3, rowspan=5)
    # # 运行主循环
    # window.mainloop()

    # import tkinter as tk
    #
    # # 创建窗口对象
    # window = tk.Tk()
    #
    # # 创建框架1
    # frame1 = tk.Frame(window, bg="red", width=200, height=100)
    # frame1.pack(fill="both", expand=True)
    #
    # # 在框架1中添加标签
    # label = tk.Label(frame1, text="Hello, Tkinter!")
    # label.pack()
    #
    # # 创建框架2
    # frame2 = tk.Frame(window, bg="blue", width=200, height=100)
    # frame2.pack(fill="both", expand=True)
    #
    # # 在框架2中添加按钮
    # button = tk.Button(frame2, text="Click me!")
    # button.pack()
    #
    # # 运行主循环
    # window.mainloop()
