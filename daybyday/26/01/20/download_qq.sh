!/bin/bash

set -e

# 获取 QQ 安装包的前 10 MB
curl 	-H "Range: bytes=0-10485759" \
      -H "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36" \
      --output qq_part1 \
      -v "https://dldir1v6.qq.com/qqfile/qq/QQNT/Windows/QQ_9.9.26_260116_x64_01.exe"

# 获取 QQ 安装包的剩余部分
curl 	-H "Range: bytes=10485760-" \
      -H "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36" \
      --output qq_part2  \
      -v "https://dldir1v6.qq.com/qqfile/qq/QQNT/Windows/QQ_9.9.26_260116_x64_01.exe"

# 拼接两部分
cat qq_part1 qq_part2 > QQ_9.9.26_260116_x64_01.exe
