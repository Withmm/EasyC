import os
import subprocess
import sys

def run_make():
    # 检查当前目录是否存在Makefile
    if os.path.isfile("Makefile"):
        # 调用make命令进行编译
        try:
            subprocess.run(["make"], check=True)
            print("Make 完成编译")
        except subprocess.CalledProcessError:
            print("编译失败，请检查Makefile")
    else:
        print("当前目录下找不到 Makefile")

def run_single_test(test_file):
    if os.path.isfile(test_file):
        print(f"开始测试: {test_file}")
        # 这里可以根据需要调用测试脚本或执行测试程序等
        # 示例：假设执行测试的命令是 ./test_program {test_file}
        subprocess.run(["./test_program", test_file])
    else:
        print(f"找不到测试文件: {test_file}")

if __name__ == "__main__":
    # 首先编译项目
    run_make()

    # 检查是否提供了测试文件名作为参数
    if len(sys.argv) != 2:
        print("请提供一个测试文件名作为参数")
        sys.exit(1)

    # 获取测试文件名
    test_file = sys.argv[1]
    # 拼接测试文件路径
    test_file_path = os.path.join("../test", test_file)

    # 运行单个测试文件
    run_single_test(test_file_path)

