import allure
import traceback
from api_tests.core.reporter import reporter

def validate(expected,actual,message,contains=False,expected2=None):
    try:
        if contains and expected2 is None:
            assert expected in actual, f"Assertion Failed: '{expected}' not found in '{actual}'"
            with allure.step(f"{message} | Status: PASSED"):
                reporter.report(message,f"Expected - {expected} contains Actual - {actual}")
        elif contains and expected2 is not None:
            assert expected in actual or expected2 in actual, f"Assertion Failed: '{expected}' or '{expected2}' not found in '{actual}'"
            with allure.step(f"{message} | Status: PASSED"):
                reporter.report(message, f"Expected - {expected} || {expected2} contains Actual - {actual}")
        else:
            assert expected == actual, f"Assertion Failed: '{expected}' != '{actual}'"
            with allure.step(f"{message} | Status: PASSED"):
                reporter.report(message, f"Expected - {expected} == Actual - {actual}")
    except AssertionError as ex:
        with allure.step(f"{message} | Status: FAILED"):
            reporter.report(message, f"Expected - {expected} != Actual - {actual}")
        raise AssertionError(f"Expected ({expected}) != Actual ({actual})"
                             f"\nTraceback : {str(traceback.format_exception(etype=type(ex), value=ex, tb=ex.__traceback__))}")