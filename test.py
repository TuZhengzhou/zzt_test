import threading
import time
import os
import psutil

# 全局进程对象
proc = psutil.Process(os.getpid())

def print_resource(prefix: str):
    """打印当前进程的资源使用情况"""
    mem = proc.memory_info()
    rss_mb = mem.rss / 1024 / 1024  # 常驻集（物理内存）
    vms_mb = mem.vms / 1024 / 1024  # 虚拟内存
    num_threads = proc.num_threads()
    print(f"[{prefix}] RSS={rss_mb:.2f} MB, VMS={vms_mb:.2f} MB, threads={num_threads}")

def blocking_io():
    tid = threading.get_ident()
    print(f"Thread {tid}: Start blocking I/O")
    print_resource(f"Thread {tid} start")

    time.sleep(2)  # 阻塞整个线程

    print(f"Thread {tid}: I/O done")
    print_resource(f"Thread {tid} done")

if __name__ == "__main__":
    print("=== Threading version: before threads ===")
    print_resource("before creating threads")

    # 创建多个线程
    threads = []
    for i in range(3):
        t = threading.Thread(target=blocking_io)
        t.start()
        threads.append(t)

    print("=== Threading version: after threads started ===")
    print_resource("after starting threads")

    # 等待所有线程结束
    for t in threads:
        t.join()

    print("=== Threading version: after join ===")
    print_resource("after joining threads")

    print("=== Threading version: program end ===")
    print_resource("program end")
