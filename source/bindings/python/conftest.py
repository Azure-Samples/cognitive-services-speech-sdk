import pytest

def pytest_addoption(parser):
    parser.addoption("--filename")
    parser.addoption("--subscription")
    parser.addoption("--endpoint")

@pytest.fixture
def filename(request):
    return request.config.getoption("--filename")

@pytest.fixture
def subscription(request):
    return request.config.getoption("--subscription")

@pytest.fixture
def endpoint(request):
    return request.config.getoption("--endpoint")