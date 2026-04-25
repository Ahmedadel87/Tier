import subprocess
import os
import platform
import sys

lexer_failed = 0
lexer_amount_of_files = 0

parser_failed = 0
parser_amount_of_files = 0

compiler = sys.argv[1]

def run_test(input_file, expected_file, *flags):
    result = subprocess.run(
        [compiler, input_file, *flags],
        capture_output=True,
        text=True
    )

    with open(expected_file, "r") as f:
        expected = f.read()
    
    if result.stdout.strip() == expected.strip():
        return True
    else:
        return False

print("\n\x1B[1;4;36mLexer tests:\x1B[0m", end="\n\n")
for file in os.listdir("lexer/"):
    if file.endswith(".tier"):
        lexer_amount_of_files += 1;
        name = file.removesuffix(".tier")
        if(run_test(f"lexer/{name}.tier", f"lexer/{name}.expected", "--dump-tokens", "--debug-no-errors")):
            print(f"\x1B[3;93mlexer/{name}.tier\x1B[0m passed 🟩 {lexer_amount_of_files-lexer_failed}/{lexer_amount_of_files}.")
        else:
            lexer_failed += 1
            print(f"\x1B[3;93mlexer/{name}.tier\x1B[0m failed 🟥 {lexer_amount_of_files-lexer_failed}/{lexer_amount_of_files}.")

print("\n\x1B[1;4;36mParser tests:\x1B[0m", end="\n\n")
for file in os.listdir("parser/check_output/"):
    if file.endswith(".tier"):
        parser_amount_of_files += 1;
        name = file.removesuffix(".tier")
        if(run_test(f"parser/check_output/{name}.tier", f"parser/check_output/{name}.expected", "--dump-ast", "--debug-no-errors")):
            print(f"\x1B[3;93mparser/check_output/{name}.tier\x1B[0m passed 🟩 {parser_amount_of_files-parser_failed}/{parser_amount_of_files}.")
        else:
            parser_failed += 1
            print(f"\x1B[3;93mparser/check_output/{name}.tier\x1B[0m failed 🟥 {parser_amount_of_files-parser_failed}/{parser_amount_of_files}.")

print(f"\nSummary:\n\x1B[1;36mLexer:\x1B[0m {lexer_amount_of_files-lexer_failed}/{lexer_amount_of_files} files passed.", end="")
if(lexer_failed == 0): print("🟩")
else: print("🟥")
print(f"\x1B[1;36mParser:\x1B[0m {parser_amount_of_files-parser_failed}/{parser_amount_of_files} files passed.", end ="")
if(parser_failed == 0): print("🟩")
else: print("🟥")