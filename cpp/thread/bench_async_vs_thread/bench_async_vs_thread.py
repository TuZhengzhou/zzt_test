#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import asyncio
import os
import threading
import time

import psutil

proc = psutil.Process(os.getpid())


def print_resource(prefix: str):
    """打印当前进程的资源使用情况"""
    mem = proc.memory_info()
    rss_mb = mem.rss / 1024 / 1024  # 物理内存
    vms_mb = mem.vms / 1024 / 1024  # 虚拟内存
    num_threads = proc.num_threads()
    print(f"[{prefix}] RSS={rss_mb:.2f} MB, VMS={vms_mb:.2f} MB, threads={num_threads}")


# ===================== 多线程基准 =====================

def thread_worker(sleep_sec: float):
    """阻塞 I/O 模拟：简单 sleep"""
    time.sleep(sleep_sec)


def run_threading_bench(num_threads: int, sleep_sec: float):
    print("\n=== Threading benchmark ===")
    print_resource("threading: before creating threads")

    threads = []
    for i in range(num_threads):
        t = threading.Thread(target=thread_worker, args=(sleep_sec,))
        t.start()
        threads.append(t)

    print_resource("threading: after starting threads")

    for t in threads:
        t.join()

    print_resource("threading: after joining threads")


# ===================== asyncio 基准 =====================

async def async_worker(sleep_sec: float):
    """异步 I/O 模拟：asyncio.sleep"""
    await asyncio.sleep(sleep_sec)


async def run_asyncio_bench_inner(num_tasks: int, sleep_sec: float):
    print("\n=== Asyncio benchmark ===")
    print_resource("asyncio: before creating tasks")

    tasks = [asyncio.create_task(async_worker(sleep_sec)) for _ in range(num_tasks)]
    print_resource("asyncio: after creating tasks")

    await asyncio.gather(*tasks)

    print_resource("asyncio: after awaiting all tasks")


def run_asyncio_bench(num_tasks: int, sleep_sec: float):
    asyncio.run(run_asyncio_bench_inner(num_tasks, sleep_sec))


# ===================== main =====================

def main():
    parser = argparse.ArgumentParser(
        description="Compare threading vs asyncio: memory & thread usage"
    )
    parser.add_argument(
        "--mode",
        choices=["thread", "async", "both"],
        default="both",
        help="选择测试模式：thread / async / both（默认）",
    )
    parser.add_argument(
        "-n",
        "--num",
        type=int,
        default=100,
        help="线程数量或协程数量（默认 100）",
    )
    parser.add_argument(
        "--sleep",
        type=float,
        default=2.0,
        help="每个线程/协程的休眠时间，默认 2 秒",
    )

    args = parser.parse_args()

    print("=== Baseline: program start ===")
    print_resource("baseline")

    if args.mode in ("thread", "both"):
        run_threading_bench(args.num, args.sleep)

    if args.mode in ("async", "both"):
        run_asyncio_bench(args.num, args.sleep)

    print("\n=== Program end ===")
    print_resource("end")


if __name__ == "__main__":
    main()
