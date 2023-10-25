import pytest
import os
import datetime
from api_tests.core.file_utils import read_test_data
from api_tests.core.config_utils import get_config_value

# Set the custom environment variable with the timestamp
timestamp = datetime.datetime.now().strftime("%Y%m%d%H%M%S")
os.environ["TEST_TIMESTAMP"] = timestamp

@pytest.hookimpl(tryfirst=True)
def pytest_configure(config):
    if config.option.html_report:
        timestamp = datetime.datetime.now().strftime("%Y%m%d%H%M%S")
        config.option.htmlpath = f"report_{timestamp}.html"

def pytest_html_results_summary(prefix, summary, postfix):
    timestamp = datetime.datetime.now().strftime("%Y%m%d%H%M%S")
    report_file = f"reports/report_{timestamp}.html"
    summary.append(f"Report: {report_file}")

def pytest_configure(config):
    config.addinivalue_line(
        "markers", "print_assertion_details: Print assertion details in the HTML report"
    )

def pytest_html_results_table_row(report, cells):
    #if 'print_assertion_details' in report.keywords and report.failed:
    #    for entry in report.longrepr.reprcrash.message.split('\n'):
    #        cells.append(entry)
    pass


# Define a custom pytest option for the command-line argument
def pytest_addoption(parser):
    parser.addoption("--setup_local_server", action="store", default="True", help="Setup local server for testing")


def read_test_data_from_csv(test_type,test_data_file_name):
    test_data_repo = None
    current_dir = os.path.dirname(os.path.abspath(__file__))

    if test_type == "http":
        test_data_repo = get_config_value('httptest', 'http_test_data_repo')
    if test_type == "smtp":
        test_data_repo = get_config_value('smtptest', 'smtp_test_data_repo')

    test_data_csv = read_test_data(f"{current_dir}{test_data_repo}{test_data_file_name}")
    return test_data_csv


def pytest_generate_tests(metafunc):
    if 'http_sanity_test_data_tc' in metafunc.fixturenames:
        test_data_file_name = get_config_value('httptest', 'http_sanity_test_data')
        http_sanity_test_data_csv = read_test_data_from_csv("http",test_data_file_name)
        metafunc.parametrize('http_sanity_test_data_tc', http_sanity_test_data_csv)

    if 'http_languages_test_data_tc' in metafunc.fixturenames:
        test_data_file_name = get_config_value('httptest', 'http_languages_test_data')
        http_languages_test_data_csv = read_test_data_from_csv("http",test_data_file_name)
        metafunc.parametrize('http_languages_test_data_tc', http_languages_test_data_csv)

    if 'http_codecs_test_data_tc' in metafunc.fixturenames:
        test_data_file_name = get_config_value('httptest', 'http_codecs_test_data')
        http_codecs_test_data_tc_csv = read_test_data_from_csv("http",test_data_file_name)
        metafunc.parametrize('http_codecs_test_data_tc', http_codecs_test_data_tc_csv)

    if 'smtp_sanity_test_data_tc' in metafunc.fixturenames:
        test_data_file_name = get_config_value('smtptest', 'smtp_sanity_test_data')
        smtp_sanity_test_data_tc_csv = read_test_data_from_csv("smtp",test_data_file_name)
        metafunc.parametrize('smtp_sanity_test_data_tc', smtp_sanity_test_data_tc_csv)

    if 'smtp_languages_test_data_tc' in metafunc.fixturenames:
        test_data_file_name = get_config_value('smtptest', 'smtp_languages_test_data')
        smtp_languages_test_data_tc_csv = read_test_data_from_csv("smtp",test_data_file_name)
        metafunc.parametrize('smtp_languages_test_data_tc', smtp_languages_test_data_tc_csv)