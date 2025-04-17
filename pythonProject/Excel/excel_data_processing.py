import openpyxl  # 引入openpyxl库
import string

wb = r'D:/code/python_Auto_Test_Project/abc.xlsx'  # 设置路径找到已有的Excel文件457.xlsx
xj = openpyxl.load_workbook(wb)  # 打开Excel文件
gzb = xj['Sheet3']  # 把名为9月一日的表赋值给变量gzb

word = []

A = string.ascii_uppercase
for i in A:
    word.append(i)
print(word)

j = 0
k = 0
num = []
for i in range(0, 168):
    if k == 0:
        row1 = word[j]
        num.append(row1)
        j += 1
        if row1 == 'Z':
            j = 0
            k += 1
    else:
        row1 = word[j]
        row2 = word[k - 1] + row1
        j += 1
        num.append(row2)
        if row1 == 'Z':
            j = 0
            k += 1
count = ''
print(num)
for c in range(1, 27):
    for i in num:
        cell = i + str(c)
        dyg = str(gzb[cell].value) + '|'  # 找到A3单元格中的值，复制给dyg
        count += dyg
print(count)  # 打印单元格的值
