import pytest
import sys,os
sys.path.append(os.path.join(os.path.abspath(os.curdir)))
import v2ticlib.string_utils as string_utils

@pytest.mark.parametrize("is_blank_string, expected_result", [
    ("", True),
    (" ", True),
    (None, True),
    ("abc", False),
])

def test_is_blank(is_blank_string, expected_result):
    assert expected_result == string_utils.is_blank(is_blank_string)

@pytest.mark.parametrize("is_not_blank_string, expected_result", [
    ("", False),
    (" ", False),
    (None, False),
    ("abc", True),
])

def test_is_not_blank(is_not_blank_string, expected_result):
    assert expected_result == string_utils.is_not_blank(is_not_blank_string)

@pytest.mark.parametrize("default_if_empty_string, default, expected_result", [
    ("", "default", "default"),
    ("   ", "default", "default"),
    (None, "default", "default"),
    ("abc", "default", "abc"),
])

def test_default_if_blank(default_if_empty_string, default, expected_result):
    assert expected_result == string_utils.default_if_empty(default_if_empty_string, default)