import pytest

def pytest_addoption(parser):
    parser.addoption("--filename")
    parser.addoption("--subscription")

@pytest.fixture
def filename(request):
    return request.config.getoption("--filename")

@pytest.fixture
def subscription(request):
    return request.config.getoption("--subscription")