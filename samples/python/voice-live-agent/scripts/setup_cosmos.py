import requests
import os

UPDATE_RESULTS_LOGIC_APP_URL = os.environ["UPDATE_RESULTS_LOGIC_APP_URL"]

# insert a few examples into experiments using the previously created logic app
requests.post(UPDATE_RESULTS_LOGIC_APP_URL, json={"id": "1", "author": "John Smith", "results": "Success"})  
requests.post(UPDATE_RESULTS_LOGIC_APP_URL, json={"id": "2", "author": "Jane Doe", "results": "Success"})  
requests.post(UPDATE_RESULTS_LOGIC_APP_URL, json={"id": "3", "author": "James Brown", "results": "Failure"})  
requests.post(UPDATE_RESULTS_LOGIC_APP_URL, json={"id": "4", "author": "Mary Johnson", "results": "Success"})  