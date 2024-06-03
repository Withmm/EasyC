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

        output_filename = os.path.join("..", "easyc", os.path.splitext(os.path.basename(file))[0] + ".easyc")
        with open(output_filename, 'w') as output_file:
            parse_result = subprocess.run(["./main", file], stdout=output_file, stderr=subprocess.PIPE)
        
        if parse_result.returncode == 0:
            print(f"Parsing successful, output saved to {output_filename}")
        else:
            print(f"Parsing failed for {file}, error: {parse_result.stderr.decode('utf-8')}")

        print()
else:
    print("Compilation failed.")

