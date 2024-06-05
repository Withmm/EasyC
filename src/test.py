#!/bin/python3
import os
import subprocess

compile_result = subprocess.run(["make"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

if compile_result.returncode == 0:
    print("Compiled successfully.")
    print("Running program...\n")

    test_files = [os.path.join("..", "test", filename) for filename in os.listdir("../test") if os.path.isfile(os.path.join("..", "test", filename))]

    output_dir = os.path.join("..", "easyc")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    log_dir = os.path.join("..", "log")
    if not os.path.exists(log_dir):
        os.makedirs(log_dir)

    for file in test_files:
        print("Parsing file:", file)

        output_filename = os.path.join(output_dir, os.path.splitext(os.path.basename(file))[0] + ".easyc")
        log_filename = os.path.join(log_dir, os.path.splitext(os.path.basename(file))[0] + ".log")

        with open(output_filename, 'w') as output_file, open(log_filename, 'w') as log_file:
            parse_result = subprocess.run(["./main", file], stdout=output_file, stderr=log_file)

        if parse_result.returncode == 0:
            print(f"Parsing successful, output saved to {output_filename}")
        else:
            with open(log_filename, 'r') as log_file:
                error_message = log_file.read()
            print(f"Parsing failed for {file}, error: {error_message.strip()}")

        print()
else:
    print("Compilation failed.")
