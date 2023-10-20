from api_tests.core.config_utils import get_config_value
import csv
import os
import base64
import textwrap
from api_tests.core.reporter import reporter

def read_text_from_file(file_path):
    with open(file_path, 'r') as file:
        file_text = file.read()
    return file_text
def file_to_base64(file_path):
    with open(file_path, "rb") as file:
        encoded_file = base64.b64encode(file.read())
    return encoded_file.decode("utf-8")  # Convert bytes to string

def get_audio_text(audio_file_dir,audio_file_name):
    current_dir = os.path.dirname(os.path.abspath(__file__))
    test_data_dir = os.path.join(current_dir, '..')
    audio_path = test_data_dir + get_config_value('common', 'audio_files_repo') + audio_file_dir + audio_file_name
    file_type = audio_file_name.split(".")[-1]

    if file_type == "txt":
        audio_text = read_text_from_file(audio_path)

    else:
        audio_text = file_to_base64(audio_path)

    return audio_text


def read_test_data(file_path):
    with open(file_path, newline='',encoding='utf-8') as csvfile:
        reader = csv.DictReader(csvfile)
        return list(reader)


def break_long_lines(message, max_line_length=100):
    lines = message.split('\n')
    wrapped_lines = []

    for line in lines:
        if len(line) > max_line_length:
            wrapped_lines.extend(textwrap.wrap(line, width=max_line_length))
        else:
            wrapped_lines.append(line)

    return '\n'.join(wrapped_lines)


def comment_file_content(file_path,text):
    with open(file_path, "r") as f:
        lines = f.readlines()
        #print(lines)

    new_lines = []

    for line in lines:
        if text in line.strip():
            if line.strip().startswith("#"):
                new_lines.append(line)
            else:
                # Comment the line
                new_lines.append("#" + line)
        else:
            new_lines.append(line)

    with open(file_path, "w") as f:
        f.writelines(new_lines)

    reporter.report(f"Commented file content: {text} in file: {file_path}")


def uncomment_file_content(file_path,text):
    with open(file_path, "r") as f:
        lines = f.readlines()
        #print(lines)

    new_lines = []

    for line in lines:
        if text in line.strip():
            if line.strip().startswith("#"):
                modified_line = line[1:]
                new_lines.append(modified_line)
            else:
                # UnComment the line
                new_lines.append(line)
        else:
            new_lines.append(line)

    with open(file_path, "w") as f:
        f.writelines(new_lines)

    reporter.report(f"Un-Commented file content: {text} in file: {file_path}")