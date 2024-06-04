#!/bin/python3
import os
import subprocess

# 编译程序
compile_result = subprocess.run(["make"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

# 如果编译成功，运行测试
if compile_result.returncode == 0:
    print("Compiled successfully.")
    print("Running program...\n")

    # 获取测试文件列表
    test_files = [os.path.join("..", "test", filename) for filename in os.listdir("../test") if os.path.isfile(os.path.join("..", "test", filename))]

    # 确保输出目录存在
    output_dir = os.path.join("..", "easyc")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # 确保日志目录存在
    log_dir = os.path.join("..", "log")
    if not os.path.exists(log_dir):
        os.makedirs(log_dir)

    # 遍历测试文件
    for file in test_files:
        print("Parsing file:", file)

        # 确定输出文件名和日志文件名
        output_filename = os.path.join(output_dir, os.path.splitext(os.path.basename(file))[0] + ".easyc")
        log_filename = os.path.join(log_dir, os.path.splitext(os.path.basename(file))[0] + ".log")

        # 执行程序并重定向标准输出和错误输出
        with open(output_filename, 'w') as output_file, open(log_filename, 'w') as log_file:
            parse_result = subprocess.run(["./main", file], stdout=output_file, stderr=log_file)

        # 检查程序是否成功执行
        if parse_result.returncode == 0:
            print(f"Parsing successful, output saved to {output_filename}")
        else:
            # 读取并打印错误信息
            with open(log_filename, 'r') as log_file:
                error_message = log_file.read()
            print(f"Parsing failed for {file}, error: {error_message.strip()}")

        print()
else:
    print("Compilation failed.")
