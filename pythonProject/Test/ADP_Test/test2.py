import threading
import queue
import time


def worker(thread_id, task_queue):
    while True:
        task = task_queue.get()
        if task == "STOP":
            print(f"线程 {thread_id} 结束")
            task_queue.task_done()
            break

        if task == thread_id:
            print(f"线程 {thread_id} 执行任务")
            time.sleep(1)
            print(f"线程 {thread_id} 任务完成")
        else:
            print(f"线程 {thread_id} 收到非自身任务，跳过")

        task_queue.task_done()


# 创建任务队列
task_queue = queue.Queue()

# 创建3个线程
threads = []
for i in range(3):
    t = threading.Thread(target=worker, args=(i, task_queue))
    t.start()
    threads.append(t)

# 只让线程0执行任务
task_queue.put(0)

# 添加停止信号
for _ in range(3):
    task_queue.put("STOP")

# 等待所有任务完成
task_queue.join()

for t in threads:
    t.join()

print("特定线程执行完成")