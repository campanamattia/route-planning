import os
import subprocess

def compile_and_run(c_program, input_file, expected_output, output_file, test_number):

    # Esegue il programma con l'input da un file e salva l'output in un file
    run_cmd = f"./{c_program.split('.')[0]} < {input_file} > {output_file}"
    os.system(run_cmd)

    # Ottieni il return code dell'esecuzione del programma C
    process = subprocess.run(f"./{c_program.split('.')[0]}", shell=True)
    return_code = process.returncode

    if return_code != 0:
        print("Il programma ha lanciato un'eccezione.")
        return

    # Confronta l'output con il file di output atteso e salva le differenze nel file di resoconto
    with open(output_file, "r") as output_file:
        with open(expected_output, "r") as expected_file:
            output_lines = output_file.readlines()
            expected_lines = expected_file.readlines()

    with open("resoconto.txt", "a") as report_file:
        report_file.write(f"\nTest {test_number} - {input_file}:\n")
        for i, (output_line, expected_line) in enumerate(zip(output_lines, expected_lines)):
            if output_line.strip() != expected_line.strip():
                report_file.write(f"Riga {i + 1}: {output_line.strip()}\n")

if __name__ == "__main__":
    c_program = "main.c"
    # Compila il programma C
    compile_cmd = f"gcc -o {c_program.split('.')[0]} {c_program} -std=c11"
    os.system(compile_cmd)

    for i in range(1, 112):
        print("doing round "+ str(i));
        input_file = f"test/open_{i}.txt"
        expected_output = f"test/open_{i}.output.txt"
        output_file = f"out/my_{i}.txt"
        compile_and_run(c_program, input_file, expected_output, output_file, i)
