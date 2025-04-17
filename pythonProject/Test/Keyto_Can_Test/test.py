import threading
import time

start_time = time.perf_counter()
time.sleep(2)
end_time = time.perf_counter()
print((end_time-start_time)*1000)

# lock = threading.Lock()
#
#
# def func1():
#     for i in range(5):
#         with lock:
#             print('Thread 1')
#
#
# def func2():
#     for i in range(5):
#         with lock:
#             print('Thread 2')
#
#
# thread1 = threading.Thread(target=func1)
# thread2 = threading.Thread(target=func2)
#
# thread1.start()
# thread2.start()
#
# thread1.join()
# thread2.join()
