import datetime
import sys
import time


# 生成日志
class LOGGER(object):
    def __init__(self, filename="Default.txt"):
        self.terminal = sys.stdout
        self.log = open(filename, "a")

    def write(self, message):
        self.terminal.write(message)
        self.log.write(message)

    def flush(self):
        pass


def Start_Log():
    now_date = datetime.date.today()
    log_path = r'D:\code\pythonProject\Com\Log' + '\\' + str(now_date) + '.txt'
    sys.stdout = LOGGER(log_path)


if __name__ == '__main__':
    Start_Log()
    for i in range(100):
        print(i)
        time.sleep(1)
