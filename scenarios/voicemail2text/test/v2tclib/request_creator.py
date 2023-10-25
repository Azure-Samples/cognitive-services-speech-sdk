import sys, os
sys.path.append(os.path.join(os.path.abspath(os.curdir)))
import v2ticlib.constants.fields as Fields

class RequestCreator():
    def __init__(self):
        self.request = {}
        self.add_metadata(self.request)
        self.metadata = self.request[Fields.METADATA]

    def get(self):
        return self.request

    def add_metadata(self, request):
        request[Fields.METADATA] = {}
        return self

    def add_language(self, language):
        self.metadata[Fields.LANGUAGE] = language

    def create_language_configuration(self):
        self.metadata[Fields.LANGUAGE_CONFIGURATION] = {}

    def add_language_configuration_entry(self, language:str, entry:dict):
        language_configuration = self.metadata[Fields.LANGUAGE_CONFIGURATION]
        language_configuration[language] = entry

    def create_language_configuration_entry(self, min_confidence_percentage_str, max_audio_length_str):
        entry = {}
        entry[Fields.MIN_CONFIDENCE_PERCENTAGE] = min_confidence_percentage_str
        entry[Fields.MAX_AUDIO_LENGTH_SECS] = max_audio_length_str
        return entry

    def create_single_language_with_matching_entry(self, language, min_confidence_percentage_str, max_audio_length_str):
        entry = self.create_language_configuration_entry(min_confidence_percentage_str, max_audio_length_str)
        self.add_language(language)
        self.create_language_configuration()
        self.add_language_configuration_entry(language, entry)
        return self.get()