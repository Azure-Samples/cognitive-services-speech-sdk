from datetime import datetime
from dateutil import parser

def get_rtf(deposit_time:datetime, final_audio_length_secs:int):
    end_datetime = datetime.now()
    delta = end_datetime - deposit_time
    return round(delta.total_seconds() / final_audio_length_secs, 2)

def to_date_time(datetime_str: str):
    return parser.parse(datetime_str)
