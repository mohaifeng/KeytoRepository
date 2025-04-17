import datetime
import random
import time


import cv2
import numpy as np


def Take_Photo():
    cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
    flag = cap.isOpened()
    index = 1
    while flag:
        ret, frame = cap.read()
        cv2.imshow("Capture_photo", frame)
        k = cv2.waitKey(10) & 0xFF
        if k == ord('s'):  # 按下s键，进入下面的保存图片操作
            cv2.imwrite("D:/code/python_Auto_Test_Project/screen/" + str(index) + ".jpg", frame)
            print("save" + str(index) + ".jpg successfully!")
            print("-------------------------")
            index += 1
        elif k == ord('q'):  # 按下q键，程序退出
            break
    cap.release()  # 释放摄像头
    cv2.destroyAllWindows()  # 释放并销毁窗口


if __name__ == '__main__':
    Take_Photo()




