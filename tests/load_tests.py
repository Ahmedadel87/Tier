import subprocess
import os
import platform
import sys

failed = 0
amount_of_files = 0
compiler = sys.argv[1]

def run_test(input_file, expected_file):
    global failed
    result = subprocess.run(
        [compiler, input_file],
        capture_output=True,
        text=True
    )

    with open(expected_file, "r") as f:
        expected = f.read()
    
    if result.stdout.strip() == expected.strip():
        print(f"\x1B[3;93m{input_file}\x1B[0m passed 🟩 {amount_of_files-failed}/{amount_of_files}.")
    else:
        failed += 1;
        print(f"\x1B[3;93m{input_file}\x1B[0m failed 🟥 {amount_of_files-failed}/{amount_of_files}.")

for file in os.listdir("lexer/"):
    if file.endswith(".tier"):
        amount_of_files += 1;
        name = file.removesuffix(".tier")
        run_test(f"lexer/{name}.tier", f"lexer/{name}.expected")

print(f"{amount_of_files-failed}/{amount_of_files} files passed.")