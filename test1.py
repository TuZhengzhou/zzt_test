import asyncio
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

async def async_io(task_id: int):
    print(f"Task {task_id}: Start async I/O")
    print_resource(f"Task {task_id} start")
    await asyncio.sleep(2)  # 不阻塞线程，只让出控制权
    print(f"Task {task_id}: Async I/O done")
    print_resource(f"Task {task_id} done")

async def main():
    print("=== Asyncio version: before tasks ===")
    print_resource("before asyncio.run(main)")

    # 创建多个协程，在同一个线程中并发执行
    tasks = [async_io(i) for i in range(3)]
    await asyncio.gather(*tasks)  # 所有协程共用同一个线程

    print("=== Asyncio version: after tasks ===")
    print_resource("after asyncio.run(main)")

if __name__ == "__main__":
    asyncio.run(main())
    # 程序结尾再打印一次
    print("=== Asyncio version: program end ===")
    print_resource("program end")
