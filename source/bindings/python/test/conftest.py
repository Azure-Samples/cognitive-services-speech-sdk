import pytest
import os
from collections import namedtuple


def pytest_addoption(parser):
    parser.addoption("--inputdir")
    parser.addoption("--subscription")
    parser.addoption("--speech-region")
    parser.addoption("--luis-subscription")
    parser.addoption("--luis-region")
    parser.addoption("--endpoint")
    parser.addoption("--language-understanding-app-id")
    parser.addoption('--no-use-default-microphone', action='store_true', dest="no_use_default_microphone",
                     default=False, help="disable tests that require a default microphone")


@pytest.fixture
def subscription(request):
    return request.config.getoption("--subscription")


@pytest.fixture
def luis_subscription(request):
    return request.config.getoption("--luis-subscription")


@pytest.fixture
def speech_region(request):
    return request.config.getoption("--speech-region", default="westus")


@pytest.fixture
def luis_region(request):
    return request.config.getoption("--luis-region", default="westus")


@pytest.fixture
def endpoint(request):
    return request.config.getoption("--endpoint")


@pytest.fixture
def language_understanding_app_id(request):
    return request.config.getoption("--language-understanding-app-id")


SpeechInput = namedtuple('SpeechInputData',
                         ('path', 'transcription', 'offset', 'duration', 'input_language', 'translations'))

batman_offsets, batman_durations = zip(*[
    (5400000, 200000000),
    (213400000, 6100000),
    (229200000, 200000000),
    (440500000, 200000000),
    (648500000, 41900000),
    (708000000, 68600000),
    (788700000, 200000000),
    (996700000, 113000000),
    (1151900000, 200000000),
    (1359900000, 6200000),
    (1375800000, 200000000),
    (1587100000, 200000000),
    (1795100000, 41900000),
    (1854600000, 68600000),
    (1935300000, 200000000),
    (2143300000, 113000000)])

speech_input_data_raw = {'weather':
                         ('whatstheweatherlike.wav', ["What's the weather like?"], (300000, ),
                          (15900000, ), 'en-US',
                          {'fr': "Quel est le temps?",
                           'de': "Wie ist das Wetter?"}
                          ),
                         'silence': ('silenceshort.wav', [''], None, None, 'en-US', dict()),
                         'batman': ('batman.wav', [
                             "Skills and abilities Batman has no inherent super powers, he relies on his own scientific knowledge detective skills and athletic prowess in the stories. Batman is regarded as one of the world's greatest detective.",
                             "If not the world's greatest crime solver. Batman has been repeatedly described as having genius level intellect. One of the greatest martial artists in the DC universe and having peak human physical conditioning.",
                             "Traveled the world, acquiring the skills needed to aid his crusade grounds crime, his knowledge and expertise in almost every discipline known to man is nearly unparalleled by any other character in the universe Batman's in it.",
                             "Possible wealth allows him to access advanced technology as a proficient scientists use able to use and modify those technologies to his advantage. Batman describes Superman as the most dangerous men."
                             "Able to defeat a team of Super powered extraterrestrials by himself in order to rescue his imprisoned teammates in the.",
                             "Storyline Superman also considers Batman to be one of the most brilliant minds on the planet. Batman has the ability to function under great physical pain and withstand mind control, he is a master.",
                             "Disguise Multilingual and an expert in espionage, often gathering information under different identity's Batman's.",
                             "Karate judo and jujitsu training has made him after Master of stealth an escape, allowing tumor allowing him to appear and disappear at Will and to break free."
                         ], batman_offsets, batman_durations, 'en-US', dict())}


@pytest.fixture
def speech_input(request):
    inputdir = request.config.getoption("--inputdir")

    filename, *args = speech_input_data_raw[request.param]
    path = os.path.join(inputdir, filename)
    return SpeechInput(path, *args)


IntentInput = namedtuple('IntentInputData',
                         ('path', 'transcription', 'offset', 'duration', 'intent_id'))
intent_input_data_raw = {
    'lamp': ('TurnOnTheLamp.wav', "Turn on the lamp.", 0, 0, 'HomeAutomation.TurnOn')}


@pytest.fixture
def intent_input(request):
    inputdir = request.config.getoption("--inputdir")

    filename, *args = intent_input_data_raw[request.param]
    path = os.path.join(inputdir, filename)
    return IntentInput(path, *args)
