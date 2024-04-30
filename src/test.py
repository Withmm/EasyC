#!/bin/python3
import os
import subprocess

compile_result = subprocess.run(["make"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
if compile_result.returncode == 0:
    print("Compiled successfully.")
    print("Running program...\n")

    test_files = [os.path.join("..", "test", filename) for filename in os.listdir("../test") if os.path.isfile(os.path.join("..", "test", filename))]

    for file in test_files:
        print("Parsing file:", file)

        parse_result = subprocess.run(["./main", file], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if parse_result.returncode == 0:
            print("Parsing successful")
        else:
            print("Parsing failed")

        print()
else:
    print("Compilation failed.")

