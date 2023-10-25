import pytest
import re
from api_tests.app.smtp_utils.smtp_email_sender_to_ic import send_email_with_audio
from api_tests.core.smtp_utils.smtp_email_parser import EmailResponseParser
from api_tests.tests.fixtures.smtp_listener_fixtures import smtp_listener_fixture,get_smtp_default_headers
from api_tests.core.file_utils import get_audio_text,break_long_lines
from api_tests.core.constants.repo_constant import RepoConstant
from api_tests.core.config_utils import get_config_value
from api_tests.core.assert_utils import validate
from api_tests.core.custom_exception import CustomBaseException

@pytest.mark.smtplanguages
@pytest.mark.smtpregression
@pytest.mark.asyncio
async def test_send_email(smtp_listener_fixture,smtp_languages_test_data_tc):
    handler = smtp_listener_fixture

    # Test Data
    test_data_tc = smtp_languages_test_data_tc
    sender_email = test_data_tc['input_sender_email']
    receiver_email = test_data_tc['input_receiver_email']
    subject = test_data_tc['input_email_subject']
    headers = get_smtp_default_headers()
    audio_language = test_data_tc['input_audio_language']
    content = get_audio_text(RepoConstant.TESTDATA_AUDIO_REPO_SMTP.value, test_data_tc['input_audio_file'])
    formatted_message = break_long_lines(content)
    expected_email_sent_message = f"Email sent to client on smtp host: {get_config_value('smtptest', 'smtp_server_url')}, port: {get_config_value('smtptest', 'smtp_server_port')}"

    # Send email to IC server
    actual_email_sent_message = await send_email_with_audio(sender_email, receiver_email, subject, headers,audio_language, formatted_message)
    validate(expected_email_sent_message,actual_email_sent_message,"Validate email sent message",True)

    pattern = r'\[([^]]+)\]'
    match = re.findall(pattern, actual_email_sent_message)
    if match:
        scrid = match[0]
        print(f"Scrid: {scrid}")
    else:
        raise CustomBaseException(f"Scrid is not generated, Email Acknowledgement: {actual_email_sent_message}")

    # Get received email detail
    email_response = await handler.get_received_email_details(scrid)

    # Parse received email and save to dict
    received_email = EmailResponseParser(email_response['email_from'],email_response['email_to'],email_response['email_content'])
    received_email.get_parse_email_text()

    expected_email_body = f"\"{test_data_tc['verify_response_email_audio_converted_text']}\""

    # Validate received email header and body
    validate(get_config_value("smtptest", "email_response_header_content_type"), received_email.email_headers['Content-Type'].strip(), 'Validate Content-Type')

    validate(get_config_value("smtptest", "email_response_header_mime_version"), received_email.email_headers['MIME-Version'].strip(),  'MIME-Version')
    validate(get_config_value("smtptest", "email_response_header_content_transfer_encoding"), received_email.email_headers['Content-Transfer-Encoding'].strip(), 'Validate Content-Transfer-Encoding')
    validate(test_data_tc['verify_email_response_from'], received_email.email_headers['From'].strip(),  'Validate Email From')
    validate(sender_email,received_email.email_headers['To'].strip(),  'Validate Email To')
    validate(test_data_tc['verify_response_email_subject'], received_email.email_headers['Subject'].strip(),  'Validate Email Subject')
    validate(test_data_tc['verify_response_email_X-Reference'], received_email.email_headers['X-Reference'].strip(),  'Validate X-Reference')
    validate(test_data_tc['verify_response_recognition_status'],received_email.email_headers['recognition_status'].strip(),'Validate recognition_status')
    validate(test_data_tc['input_audio_language'],received_email.email_headers['requested_languages'].strip(),'Validate recognition language')
