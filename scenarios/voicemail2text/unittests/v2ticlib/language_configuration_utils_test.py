import unittest
from unittest.mock import MagicMock
import sys, os
sys.path.append(os.path.join(os.path.abspath(os.curdir)))
from v2ticlib.language_configuration_utils import *

class TestLanguageConfigurationUtils(unittest.TestCase):

    def setUp(self):
        self.request = {
            'language': 'en-US',
            'language_configuration': {
                'en-US': {
                    'min_confidence_percentage': 50,
                    'max_audio_length_secs': 10
                }
            }
        }

    def test_is_valid_language(self):
        self.assertTrue(is_valid_language('en-US'))
        self.assertFalse(is_valid_language('invalid-language'))

    def test_get_valid_languages(self):
        self.assertEqual(get_valid_languages(['en-US', 'fr-FR', 'es-ES']), ['en-US', 'fr-FR', 'es-ES'])
        self.assertEqual(get_valid_languages(['en-US', 'invalid-language', 'es-ES']), ['en-US', 'es-ES'])

    def test_get_valid_languages_from_str(self):
        self.assertEqual(get_valid_languages_from_str('en-US,fr-FR,es-ES'), ['en-US', 'fr-FR', 'es-ES'])
        self.assertEqual(get_valid_languages_from_str('en-US,invalid-language,es-ES'), ['en-US', 'es-ES'])

    def test_has_more_than_one_language(self):
        self.assertTrue(has_more_than_one_language(['en-US', 'fr-FR']))
        self.assertFalse(has_more_than_one_language(['en-US']))

    def test_is_empty(self):
        self.assertTrue(is_empty([]))
        self.assertFalse(is_empty(['en-US']))

    def test_is_valid_percentage(self):
        self.assertTrue(is_valid_percentage('50'))
        self.assertFalse(is_valid_percentage('invalid-percentage'))

    def test_is_invalid_percentage(self):
        self.assertTrue(is_invalid_percentage('invalid-percentage'))
        self.assertFalse(is_invalid_percentage('50'))

    def test_is_valid_audio_length(self):
        self.assertTrue(is_valid_audio_length('10'))
        self.assertFalse(is_valid_audio_length('invalid-audio-length'))

    def test_is_invalid_audio_length(self):
        self.assertTrue(is_invalid_audio_length('invalid-audio-length'))
        self.assertFalse(is_invalid_audio_length('10'))

    def test_does_not_have_min_percentage(self):
        self.assertTrue(does_not_have_min_percentage({'max_audio_length_secs': 10}))
        self.assertFalse(does_not_have_min_percentage({'min_confidence_percentage': 50, 'max_audio_length_secs': 10}))

    def test_does_not_have_max_audio_length(self):
        self.assertTrue(does_not_have_max_audio_length({'min_confidence_percentage': 50}))
        self.assertFalse(does_not_have_max_audio_length({'min_confidence_percentage': 50, 'max_audio_length_secs': 10}))

    def test_is_valid_threshold(self):
        self.assertTrue(is_valid_threshold({'min_confidence_percentage': 50, 'max_audio_length_secs': 10}))
        self.assertFalse(is_valid_threshold({'min_confidence_percentage': 'invalid-percentage', 'max_audio_length_secs': 10}))

    def test_is_invalid_threshold(self):
        self.assertTrue(is_invalid_threshold({'min_confidence_percentage': 'invalid-percentage', 'max_audio_length_secs': 10}))
        self.assertFalse(is_invalid_threshold({'min_confidence_percentage': 50, 'max_audio_length_secs': 10}))

    def test_is_valid_language_configuration_threshold(self):
        self.assertTrue(is_valid_language_configuration_threshold('en-US', {'min_confidence_percentage': 50, 'max_audio_length_secs': 10}))
        self.assertFalse(is_valid_language_configuration_threshold('en-US', {'min_confidence_percentage': 'invalid-percentage', 'max_audio_length_secs': 10}))

    def test_get_threshold_from_language_configuration_or_default(self):
        request_utils.get_language_configuration = MagicMock(return_value={
            'en-US': {'min_confidence_percentage': 50, 'max_audio_length_secs': 10},
            'fr-FR': {'min_confidence_percentage': 60, 'max_audio_length_secs': 20}
        })
        self.assertEqual(get_threshold_from_language_configuration_or_default(self.request, {'min_confidence_percentage': 70, 'max_audio_length_secs': 30}, 'en-US', 'fr-FR'), {'min_confidence_percentage': 50, 'max_audio_length_secs': 10})
        self.assertEqual(get_threshold_from_language_configuration_or_default(self.request, {'min_confidence_percentage': 70, 'max_audio_length_secs': 30}, 'invalid-language', 'fr-FR'), {'min_confidence_percentage': 60, 'max_audio_length_secs': 20})
        self.assertEqual(get_threshold_from_language_configuration_or_default(self.request, {'min_confidence_percentage': 70, 'max_audio_length_secs': 30}, 'invalid-language-1', 'invalid-language-2'), {'min_confidence_percentage': 70, 'max_audio_length_secs': 30})

if __name__ == '__main__':
    unittest.main()