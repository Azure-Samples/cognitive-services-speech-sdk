import time
import threading
from api_tests.core.config_utils import get_config_value
from api_tests.app.http_utils.http_listener_client import HTTPListenerClient
from api_tests.core.rest_client.rest_client import RestClient


class ResponseFetcher:
    def __init__(self):
        self.listener_url = f"https://{HTTPListenerClient.listener_host}"
        self.base_url = f"{self.listener_url}:{HTTPListenerClient.listener_port}"
        self.response = None
        self.done_event = threading.Event()

    def fetch_response(self, scrid):
        rest_client = RestClient(base_url=self.base_url)
        while not self.done_event.is_set():
            response = rest_client.get(service_endpoint=f"/response/scrid/{scrid}", headers=None)
            if response.status_code == 200:
                self.response = response
                self.done_event.set()
            time.sleep(1)

    def get_converted_response_for_scrid_with_timeout_and_thread(self, scrid, timeout=int(get_config_value('common', 'fetch_scrid_from_listener_timeout'))):
        response_fetcher = threading.Thread(target=self.fetch_response, args=(scrid,))
        response_fetcher.start()
        response_fetcher.join(timeout)
        if self.response is None:
            raise TimeoutError("Timeout reached. Unable to get 200 response.")
        return self.response

    def get_converted_response_for_scrid_with_timeout(self, scrid, timeout=int(get_config_value('common', 'fetch_scrid_from_listener_timeout'))):
        rest_client = RestClient(base_url=self.base_url)

        start_time = time.time()
        while True:
            response = rest_client.get(service_endpoint=f"/response/scrid/{scrid}",headers=None)
            if response.status_code == 200:
                break
            elif time.time() - start_time >= timeout:
                raise TimeoutError("Timeout reached. Unable to get 200 response.")

            time.sleep(1)

        return response