import os
import asyncio.subprocess as asubprocess
from v2ticlib.logger_utils import log
from v2ticlib.constants.fields import TRANSCODER, DURATION, AUDIO_TRUNCATED, AUDIO_INFO
import v2ticlib.config_utils as config_utils
import v2ticlib.request_utils as request_utils
class TranscodeUtils():

    def get_terminate_after_duration_secs(self):
        return config_utils.get_timelength_property_secs(TRANSCODER, 'terminate_after')

    def get_kill_after_duration_secs(self):
        return config_utils.get_timelength_property_secs(TRANSCODER, 'kill_after')

    async def transcode(self, request):

        max_audio_length = request_utils.get_max_audio_length_secs(request)

        transcode_cmd = f'ffmpeg -y -i pipe: -f s16le -acodec pcm_s16le -ac 1 -ar 16000 -ss 00:00:00 -t {max_audio_length} pipe:'

        terminate_after_duration_secs = self.get_terminate_after_duration_secs()
        kill_after_duration_secs = self.get_kill_after_duration_secs()
        if os.name != 'nt':
            transcode_cmd = f'timeout -k {kill_after_duration_secs}s {terminate_after_duration_secs}s {transcode_cmd}'

        audio_bytes = request_utils.get_audio(request)
        audio_out_bytes = await self.do_transcode(transcode_cmd, audio_bytes)

        audio_info = {}
        duration = len(audio_out_bytes) / 16000 / 2
        audio_info[DURATION] = duration
        audio_info[AUDIO_TRUNCATED] = self.is_truncated(audio_info[DURATION], max_audio_length)
        log(f'{AUDIO_INFO} = {audio_info}')

        request_utils.set_audio_info(request, audio_info)
        request_utils.set_audio(request, audio_out_bytes)

    async def do_transcode(self, transcode_cmd, audio_bytes):
        return_code, output, audio_output_bytes = await self.exec_stdin(transcode_cmd, audio_bytes)
        if (return_code != 0):
            log(f'transcoding failed, return output not successful! {output}')
            raise Exception('Transcoder Failed!')

        return audio_output_bytes

    def is_truncated(self, duration, max_audio_length):
        truncated = False
        if duration >= max_audio_length:
            truncated = True
        return truncated

    async def exec_stdin(self, cmd, in_bytes):
        log(f'transcode command: {cmd}')
        process = await asubprocess.create_subprocess_shell(cmd, stdin=asubprocess.PIPE, stdout=asubprocess.PIPE, stderr=asubprocess.PIPE)
        out_bytes, stderr = await process.communicate(input=in_bytes)
        log(f'transcoder return code = {process.returncode}')
        return process.returncode, stderr.decode(), out_bytes