import csv

import pytest
import sys
from madp_dll_repackage import *
import json

test_idex = 0
txt_path = r"test_result.csv"


def Test_Idex_Count(func):
    def wrapper(*args, **kwargs):
        global test_idex
        test_idex += 1
        with open(txt_path, "a", encoding="utf-8") as file:
            if test_idex == 1:
                file.write('\n')
                file.write('序号' + ' ' * 16)
                file.write('测试对象' + ' ' * 16)
                file.write('是否通过' + ' ' * 16)
                file.write('返回字典' + ' ' * 16 + '返回数据' + ' ' * 16)
                file.write('期望字典' + ' ' * 16 + '期望数据')
            file.write('\n' + ' ' * (len(str(test_idex)) % 4) + str(test_idex))  # indent 缩进
        return func(*args, **kwargs)

    return wrapper


@Test_Idex_Count
def KeytoResult_Func_Test(func_result: tuple, target_result, target_data):
    with open(txt_path, "a", encoding="utf-8") as file:
        file.write(' ' * 4 + json.dumps(func_result[2], indent=None, ensure_ascii=False))
        if (func_result[0] == target_result) & (func_result[1] == target_data):
            file.write(' ' * 4 + 'Pass')
        else:
            file.write(' ' * 4 + 'No Pass')
        file.write(' ' * 4 + json.dumps(func_result[0], indent=None, ensure_ascii=False))
        file.write(' ' * 4 + json.dumps(func_result[1], indent=None, ensure_ascii=False))
        file.write(' ' * 4 + json.dumps(target_result, indent=None, ensure_ascii=False))
        file.write(' ' * 4 + json.dumps(target_data, indent=None, ensure_ascii=False))


if __name__ == '__main__':
    test_dic={"isRes": True, "add": 170, "group": 0, "seq": 0, "cmd": "k", "state": 0,
                                   "msg": "9600,38400,500,250718100"}
    with open(txt_path, "a", encoding="utf-8",newline='') as file:
        writer = csv.writer(file)
        writer.writerow(str(test_dic))
