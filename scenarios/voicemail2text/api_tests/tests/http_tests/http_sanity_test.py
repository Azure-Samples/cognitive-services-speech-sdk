import pytest
import allure
from api_tests.core.config_utils import get_config_value
from api_tests.core.xml_parser_utils import get_values_from_xml_response
from api_tests.core.file_utils import get_audio_text
from api_tests.core.assert_utils import validate
from api_tests.app.http_utils.http_request_sender_to_ic import TranscribeRequestSender
from api_tests.app.http_utils.http_response_fetcher_from_listener import ResponseFetcher
from api_tests.tests.fixtures.http_listener_fixtures import http_listener_fixture
from api_tests.core.constants.http_status_code import HttpStatusCode
from api_tests.core.constants.repo_constant import RepoConstant


@pytest.mark.usefixtures('http_listener_fixture')
@pytest.mark.httpsanity
@pytest.mark.httpregression
def test_sanity(http_sanity_test_data_tc):
    test_data_tc = http_sanity_test_data_tc
    audio_file_name = test_data_tc['input_audio_file']
    audio = get_audio_text(RepoConstant.TESTDATA_AUDIO_REPO_HTTP.value,audio_file_name)
    audio_lang = test_data_tc['input_audio_language']
    content_encoding = test_data_tc['input_content_encoding']
    content_type = test_data_tc['input_content_type']

    # POST --> Send request to transcribe audio
    with allure.step(f"POST transcribe request to server for language {audio_lang}"):
        transcribe = TranscribeRequestSender()
        acknowledgement_response = transcribe.send_transcribe_request(audio_lang, content_encoding, content_type, audio)

        validate(HttpStatusCode.ACCEPTED.value,acknowledgement_response.status_code,'Validate Response status code')
        validate("application/octet-stream",acknowledgement_response.headers.get("content-type"),'Validate response Header Content-Type')

    # Fetch scrid from response header
    with allure.step(f"Fetch scrid from response header"):
        scrid = acknowledgement_response.headers.get("location")
        allure.attach(f"Scrid id is --> {scrid}",name='Scrid',attachment_type=allure.attachment_type.TEXT)

    # GET --> Fetch response received from server from HTTP listener
    with allure.step(f"GET scrid {scrid} response from HTTP Listener"):
        response_fetcher = ResponseFetcher()
        # final_response = response_fetcher.get_converted_response_for_scrid_with_timeout(scrid)
        final_response = response_fetcher.get_converted_response_for_scrid_with_timeout_and_thread(scrid)

        # Validate response status code and header
        validate(HttpStatusCode.OK.value, final_response.status_code, 'Validate Response status code')
        validate("application/xml; charset=utf-8", final_response.headers.get("Content-Type"),'Validate Response Header Content-Type')
        validate(scrid, final_response.headers.get("X-Reference"), 'Validate Response Header X-Reference')

        # Validate response body
        response_body_element_key = ['scrid', 'externalRequestID', 'reasonStatus', 'recognitionStatus', 'convertedText']

        xml_response_dict = get_values_from_xml_response(final_response.text, response_body_element_key)

        validate(scrid,xml_response_dict['scrid'],'Validate Response body scrid')
        validate(get_config_value('httptest','caller_id'),xml_response_dict['externalRequestID'],'Validate Response Body externalRequestID')
        validate(test_data_tc['verify_audio_converted_status'],xml_response_dict['reasonStatus'],'Validate Response Body reasonStatus')
        #validate(test_data_tc['verify_audio_converted_text'], xml_response_dict['convertedText'],'Validate Response Body convertedText')
        validate(test_data_tc['verify_recognition_status'], xml_response_dict['recognitionStatus'],'Validate Response Body recognitionStatus')
