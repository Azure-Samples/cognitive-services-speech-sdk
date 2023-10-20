import requests
import allure
from api_tests.core.reporter import reporter


class RestClient:
    def __init__(self, base_url):
        """
        :param url: Set request url
        :param body: Set request body
        """
        self.base_url = base_url
        self.verify = False

    def post(self, service_endpoint, headers, body):
        """
        :param headers:
        :param service_endpoint: post request service endpoint
        :param body: post request body
        :return: response
        """
        complete_url = self.base_url + service_endpoint

        with allure.step(f"POST Request Details"):
            reporter.report("***Request before sending POST request***")
            reporter.report("Request URL",complete_url)
            reporter.report("Request Header",headers)
            # reporter.report("Request Body",body)

        response = requests.post(url=complete_url, headers=headers, json=body, verify=self.verify)

        with allure.step(f"Response Details"):
            reporter.report("***Response after sending POST request***")
            reporter.report("Response Status Code",response.status_code)
            reporter.report("Response Header",response.headers)
            #reporter.report("Response Body",response.json())

        return response

    def get(self,service_endpoint,headers):
        """
        :param headers:
        :param service_endpoint: get request service endpoint
        :return: response
        """
        complete_url = self.base_url + service_endpoint

        with allure.step(f"Request Details"):
            reporter.report("***Request before sending GET request***")
            reporter.report("Request URL",complete_url)
            reporter.report("Request Header",headers)

        response = requests.get(url=complete_url, headers=headers,verify=self.verify)

        with allure.step(f"Response Details"):
            reporter.report("***Response after sending GET request***")
            reporter.report("Response Status Code", response.status_code)
            reporter.report("Response Header",response.headers)

            if response.status_code == 200:
                #reporter.report("Response Body",response.json())
                reporter.report("Response Body", response.text)

        return response
