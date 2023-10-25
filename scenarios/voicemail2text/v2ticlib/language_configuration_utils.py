import v2ticlib.constants.fields as Fields
import v2ticlib.string_utils as string_utils
import v2ticlib.config_utils as config_utils
from v2ticlib.logger_utils import log
import v2ticlib.request_utils as request_utils

supported_languages = set()

def get_property(property, literal_eval=False):
    return config_utils.get_property(Fields.METADATA, property, literal_eval=literal_eval)

def get_default_language() -> str:
    return get_property('default_language', literal_eval=False)

def get_default_lid_languages() -> list:
    return get_property('default_lid_languages', literal_eval=True)

def get_default_thresholds() -> dict:
    return get_property('default_thresholds', literal_eval=True)

def get_default_lid_thresholds() -> dict:
    return get_property('default_lid_thresholds', literal_eval=True)

def get_supported_languages() -> set:
    global supported_languages
    if len(supported_languages) == 0:
        supported_languages = get_property('supported_languages', literal_eval=True)

    return supported_languages

def is_non_empty_language_configuration(language_configuration:dict) -> bool:
    return language_configuration != None and len(language_configuration) > 0

def is_empty_language_configuration(language_configuration:dict) -> bool:
    return not is_non_empty_language_configuration(language_configuration)

def is_valid_language(language:str) -> bool:
    return language in get_supported_languages()

def get_valid_languages(languages:list) -> list:
    return [language.strip() for language in languages if is_valid_language(language.strip())]

def get_valid_languages_from_str(language:str) -> list:
    if string_utils.is_blank(language):
        return []
    return get_valid_languages(language.split(","))

def has_more_than_one_language(languages:list) -> bool:
    return len(languages) > 1

def is_empty(languages:list) -> bool:
    return languages == None or len(languages) == 0

def is_string_a_valid_int(string:str) -> bool:
    is_valid:bool = False
    try:
        int(string)
        is_valid = True
    except ValueError:
        pass
    return is_valid

def is_string_an_invalid_int(string:str) -> bool:
    return not is_string_a_valid_int(string)

def is_valid_percentage(percentage_str:str) -> bool:
    if is_string_an_invalid_int(percentage_str):
        return False

    percentage_int: int = int(percentage_str)
    return percentage_int > 0 and percentage_int <= 100

def is_invalid_percentage(percentage_str:str) -> bool:
    return not is_valid_percentage(percentage_str)

def is_valid_audio_length(audio_length_str:str) -> bool:
    if is_string_an_invalid_int(audio_length_str):
        return False
    audio_length_int:int = int(audio_length_str)
    return audio_length_int > 0

def is_invalid_audio_length(audio_length_str:str) -> bool:
    return not is_valid_audio_length(audio_length_str)

def does_not_have_min_percentage(entry:dict) -> bool:
    return entry.get(Fields.MIN_CONFIDENCE_PERCENTAGE) == None

def does_not_have_max_audio_length(entry:dict) -> bool:
    return entry.get(Fields.MAX_AUDIO_LENGTH_SECS) == None

def is_valid_threshold(threshold:dict) -> bool:
    if does_not_have_min_percentage(threshold):
        log(f'missing {Fields.MIN_CONFIDENCE_PERCENTAGE}')
        return False

    if does_not_have_max_audio_length(threshold):
        log(f'missing {Fields.MAX_AUDIO_LENGTH_SECS}')
        return False

    percentage_str = threshold[Fields.MIN_CONFIDENCE_PERCENTAGE]
    if is_invalid_percentage(percentage_str):
        log(f'invalid {Fields.MIN_CONFIDENCE_PERCENTAGE} = {percentage_str}')
        return False

    max_audio_length_str = threshold[Fields.MAX_AUDIO_LENGTH_SECS]
    if is_invalid_audio_length(max_audio_length_str):
        log(f'invalid {Fields.MAX_AUDIO_LENGTH_SECS} = {max_audio_length_str}')
        return False

    return True

def is_invalid_threshold(threshold:dict) -> bool:
    return not is_valid_threshold(threshold)

def is_valid_language_configuration_threshold(entry:str, threshold:dict) -> bool:
    if threshold is None:
        log(f'no threshold found for entry: {entry}')
        return False

    if type(threshold) is not dict:
        log(f'threshold found for entry: {entry} not a dict!!')
        return False

    if is_invalid_threshold(threshold):
        log(f'threshold found for entry: {entry} invalid')
        return False

    return True

def get_threshold_from_language_configuration_or_default(request:dict, default_threshold:dict, *entries) -> dict:
    language_configuration = request_utils.get_language_configuration(request)
    if is_empty_language_configuration(language_configuration):
        log(f'missing or empty language configuration, using default threshold')
        return default_threshold

    for entry in entries:
        threshold = language_configuration.get(entry)
        if is_valid_language_configuration_threshold(entry, threshold):
            log(f'found valid threshold for entry: {entry}')
            return threshold

    log(f'no valid valid threshold found for entries {entries}, using default threshold')
    return default_threshold

def resolve(request:dict):
    log('resolving language configuration...')

    language:str = request_utils.get_language_field(request)
    if string_utils.is_not_blank(language):
        log(f'language field found containing the following: {language}')

    languages:list = get_valid_languages_from_str(language)
    if is_empty(languages):
        resolve_has_no_language_field(request)
    else:
        resolve_has_language_field(request, languages)

def resolve_has_language_field(request:dict, languages:list):
    log(f'resolving with the following valid languages: {languages}')
    if has_more_than_one_language(languages):
        resolve_lid_languages(request, languages)
    else:
        resolve_single_language(request, languages)

def resolve_single_language(request:dict, language_list:list):
    lid_enabled = False
    default_threshold = get_default_thresholds()
    language = language_list[0]
    threshold = get_threshold_from_language_configuration_or_default(request, default_threshold, language, Fields.DEFAULT)

    update_request(request, lid_enabled, language_list, threshold)

def resolve_lid_languages(request:dict, language_list:list):
    lid_enabled = True
    default_lid_threshold = get_default_lid_thresholds()

    threshold = get_threshold_from_language_configuration_or_default(request, default_lid_threshold, Fields.LID)

    update_request(request, lid_enabled, language_list, threshold)

def is_lid_enabled(request:dict) -> bool:
    is_lid_enabled = request_utils.get_acs_client_property(request, Fields.LID_ENABLED, literal_eval=True)
    if is_lid_enabled is None:
        is_lid_enabled = config_utils.get_property(Fields.ACS_CLIENT, Fields.LID_ENABLED, literal_eval=True)
        log(f'no runtime acs_client.lid_enabled configuration found, fetching lid_enabled = {is_lid_enabled} from configuration')
    return is_lid_enabled

def resolve_has_no_language_field(request:dict):
    log(f'resolving without a language field')
    lid_enabled = is_lid_enabled(request)

    if lid_enabled:
        resolve_has_no_language_field_lid_enabled(request)
    else:
        resolve_has_no_language_field_single_language(request)

def get_lid_languages(request) -> str:
    default_lid_languages = get_default_lid_languages()

    language_configuration = request_utils.get_language_configuration(request)
    if is_empty_language_configuration(language_configuration):
        log(f'no language_configuration found, using default lid languages from configuration {default_lid_languages}')
        return default_lid_languages

    entries = language_configuration.keys()
    log(f'language configuration found with the following entries = {entries}')
    languages = get_valid_languages(entries)
    if len(languages) > 0:
        log(f'the following entries were found to be valid languages: {languages}')
        return languages

    log(f'no valid language from entries, using default lid languages from configuration: {default_lid_languages}')
    return default_lid_languages

def resolve_has_no_language_field_lid_enabled(request):
    languages = get_lid_languages(request)
    resolve_lid_languages(request, languages)

def resolve_has_no_language_field_single_language(request):
    language = get_default_language()
    language_list:list = []
    language_list.append(language)
    log(f'resolving using default language: {language}')
    resolve_single_language(request, language_list)

def update_request(request:dict, lid_enabled:bool, language_list:list, threshold:dict):
    ammend_to_metdata = {
        Fields.LID_ENABLED: lid_enabled,
        Fields.REQUESTED_LANGUAGES: language_list,
        Fields.MIN_CONFIDENCE_PERCENTAGE: int(threshold[Fields.MIN_CONFIDENCE_PERCENTAGE]),
        Fields.MAX_AUDIO_LENGTH_SECS: int(threshold[Fields.MAX_AUDIO_LENGTH_SECS])
    }
    request_utils.update_metadata_fields_if_not_exist(request, ammend_to_metdata)
    log(f'using the following to process this request: {ammend_to_metdata}')