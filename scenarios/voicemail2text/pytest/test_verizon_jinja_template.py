import csv
import os
from jinja2 import Environment, FileSystemLoader
##Run pytest with specific method on terminal as pytest -k method_name
##Run pytest with specific file on terminal as pytest file_name.py

verizon_response_test_data = 'verizon_response_test_data.csv'
template_dir = 'etc/templates/verizon/'

def read_test_data(file_path):
    with open(file_path, newline='', encoding='utf-8') as csvfile:
        reader = csv.DictReader(csvfile)
        return list(reader)
"""
def get_output_of_jinja_template(template_name='response.j2', data= None):
    environment = Environment(loader=FileSystemLoader("etc/templates/verizon/"))
    template = environment.get_template(template_name)
    output = template.render({"response": data})
    return output
"""
def get_output_of_jinja_template(template_name='response.j2', data= None):
    current_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.dirname(current_dir)
    environment = Environment(loader=FileSystemLoader(f"{parent_dir}/{template_dir}"))
    template = environment.get_template(template_name)
    output = template.render({"response": data})
    return output

def get_test_data_from_csv_file():
    current_dir = os.path.dirname(os.path.abspath(__file__))
    test_data = read_test_data(f"{current_dir}/{verizon_response_test_data}")
    return test_data


def test_jinja_success_response_template():
    test_data = get_test_data_from_csv_file()
    for row in test_data:
        data = {
        "x_voice_writer": row['x_voice_writer'],
        "subject": row['subject'],
        "x_cns_language": row['x_cns_language']
        }
        actual_output = get_output_of_jinja_template('response.j2', data).strip()
        expected_output = row["response_message"]
        print(f"Actual output: {actual_output}")
        print(f"Expected output: {expected_output}")
    assert actual_output == expected_output, f"Test failed: expected {expected_output}, but got {actual_output}"


def test_jinja_failed_response_template():
    data = {
    "voice_writer": 'false'
    }
    actual_output = get_output_of_jinja_template('response.j2', data).strip()
    expected_output = "An error has occurred."
    print(f"Actual output: {actual_output}")
    print(f"Expected output: {expected_output}")
    assert actual_output == expected_output, f"Test failed: expected {expected_output}, but got {actual_output}"

#asyncio.run(test_jinja_response_template())
