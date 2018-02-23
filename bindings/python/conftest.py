import pytest

def pytest_addoption(parser):
    parser.addoption("--filename")

@pytest.fixture
def filename(request):
    return request.config.getoption("--filename")