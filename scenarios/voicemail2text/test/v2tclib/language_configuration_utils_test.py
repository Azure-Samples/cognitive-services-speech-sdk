import pytest
import sys, os
sys.path.append(os.path.join(os.path.abspath(os.curdir)))
import v2ticlib.constants.fields as Fields
import v2ticlib.language_configuration_utils as language_configuration_utils
from request_creator import RequestCreator
import v2ticlib.config_utils as config_utils

@pytest.fixture(scope="module")
def blank_request():
    request_creator = RequestCreator()
    return request_creator.get()

@pytest.fixture(scope="module")
def request_single_language_with_matching_entry():
    request_creator = RequestCreator()
    request = request_creator.create_single_language_with_matching_entry(language='en-US', min_confidence_percentage_str='60', max_audio_length_str='240')
    return request

def test_resolve_single_language(request_single_language_with_matching_entry):
    language_configuration_utils.resolve(request_single_language_with_matching_entry)
    metadata = request_single_language_with_matching_entry[Fields.METADATA]

    requested_languages = metadata[Fields.REQUESTED_LANGUAGES]
    requested_language = requested_languages[0]

    language_configuration = metadata[Fields.LANGUAGE_CONFIGURATION]
    min_confidence_percentage_str = language_configuration[requested_language][Fields.MIN_CONFIDENCE_PERCENTAGE]
    max_audio_length_secs_str = language_configuration[requested_language][Fields.MAX_AUDIO_LENGTH_SECS]

    assert len(requested_languages) == 1
    assert requested_language == metadata[Fields.LANGUAGE]
    assert metadata[Fields.LID_ENABLED] == False
    assert metadata[Fields.MIN_CONFIDENCE_PERCENTAGE] == int(min_confidence_percentage_str)
    assert metadata[Fields.MAX_AUDIO_LENGTH_SECS] == int(max_audio_length_secs_str)

def test_resolve_no_language_and_lid_configured_enabled(blank_request):
    language_configuration_utils.resolve(blank_request)
    default_lid_entry_from_configuration = config_utils.get_property(Fields.METADATA, 'default_lid_thresholds', literal_eval=True)
    min_confidence_percentage_str = default_lid_entry_from_configuration[Fields.MIN_CONFIDENCE_PERCENTAGE]
    max_audio_length_secs_str = default_lid_entry_from_configuration[Fields.MAX_AUDIO_LENGTH_SECS]

    default_lid_languages = config_utils.get_property(Fields.METADATA, 'default_lid_languages', literal_eval=True)

    metadata = blank_request[Fields.METADATA]
    requested_languages = metadata[Fields.REQUESTED_LANGUAGES]
    assert sorted(requested_languages) == sorted(default_lid_languages)
    assert metadata[Fields.LID_ENABLED] == True
    assert metadata[Fields.MIN_CONFIDENCE_PERCENTAGE] == int(min_confidence_percentage_str)
    assert metadata[Fields.MAX_AUDIO_LENGTH_SECS] == int(max_audio_length_secs_str)