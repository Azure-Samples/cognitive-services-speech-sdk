// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "music_player.h"

extern const char*kBluetoothMusicProviderName;  // "Bluetooth"
extern const char*kSpotifyMusicProviderName;    // "Spotify"

typedef struct
{
    const char* streamUri;
    const char* artist;
    const char* album;
    const char* track;
    const char* imageUri;
    const char* provider;
    const char* streamFormat;
    const char* trackId;
    const char* itemType;
    int playtimeInterval;
} AudioPlayer_PlayStream;

enum cortana_shuffle_state
{
    CORTANA_SHUFFLE_OFF = 0,
    CORTANA_SHUFFLE_ON = 1,
};

enum cortana_repeat_state
{
    CORTANA_REPEAT_OFF = 0,
    CORTANA_REPEAT_ALL = 1,
    CORTANA_REPEAT_ONE = 2,
};

enum cortana_rating_state
{
    CORTANA_RATING_UNRATED = 0,
    CORTANA_RATING_LIKED = 1,
    CORTANA_RATING_DISLIKED = 2,
};

/*
* The CORTANA_FETCH_INFO type represents a structure used to hold fields required
* for next, previous, and prefetch request events
*/
typedef struct _CORTANA_FETCH_INFO
{
    const char* provider;
    const char* trackId;
} CORTANA_FETCH_INFO;

/**
* The CORTANA_PLAY_UPDATE_INFO type represents a structure used to hold all fields
* required for a now playing update from a music player
*/
typedef struct _CORTANA_PLAY_UPDATE_INFO
{
    enum cortana_mediactrl_state state;
    enum cortana_shuffle_state shuffleState;
    enum cortana_repeat_state repeatState;
    enum cortana_rating_state ratingState;
    const char* type;
    const char* provider;
    const char* playlist;
    unsigned int duration;
    unsigned int position;
    const char* artist;
    const char* title;
    const char* album;
    const char* imageUri;
    const char* trackId;
    const char* streamUri;
    const char* itemType;
} CORTANA_PLAY_UPDATE_INFO;

/*
* The CORTANA_PLAYBACK_ERROR_INFO type represents a structure used to hold fields required
* for an error event that is encountered when trying to playback a song
*/
typedef struct _CORTANA_PLAYBACK_ERROR_INFO
{
    const char* provider;
    const char* trackId;
    const char* streamUri;
    int errorCode;
    const char* errorMessage;
    int isHttpError;

} CORTANA_PLAYBACK_ERROR_INFO;

/*
* The CORTANA_VOLUME_CHANGED_INFO type represents a structure used to hold fields required
* for a volume changed event
*/
typedef struct _CORTANA_VOLUME_CHANGED_INFO
{
    unsigned int volume;
    enum cortana_audio_type audioGroup;
} CORTANA_VOLUME_CHANGED_INFO;

/**
* The CORTANA_PLAY_REPORT_INFO type represents a structure used to hold all fields
* required for a music heartbeat from device to service
*/
#define CORTANA_PLAY_REPORT_INFO CORTANA_PLAY_UPDATE_INFO

/**
* The PCORTANA_GET_VOLUME_CALLBACK exposes the volume value to the cortana code
* @param handle The Cortana handle.
* @param group The appropriate audio group for which the volume should be returned
* @return The volume amount from 0-100
*/
typedef int(*PCORTANA_GET_VOLUME_CALLBACK)(CORTANA_HANDLE handle, enum cortana_audio_type group);

/**
* The PCORTANA_VOLUME_CALLBACK type represents an application-defined
* status callback function used to give the application an opportunity
handle volume state changes.
* @param handle The Cortana handle.
* @param pContext A pointer to the application-defined callback context.
* @param audioGroup The audio group associated with the volume change.
* @param volumeLevel The updated volume level.
*/
typedef void(*PCORTANA_VOLUME_CALLBACK)(
    CORTANA_HANDLE          handle,
    void*                   pContext,
    enum cortana_audio_type audioType,
    int                     volumeLevel);

/**
* Sets the current media control state.
* @param handle The Cortana handle.
* @param mediaEvent The media event to set. Refer to the cortana_mediactrl_state
* enumeration.
* @param pContext Any additional context for the event
*/
void cortana_mediacontrol_setstate(
    CORTANA_HANDLE handle,
    enum cortana_mediactrl_state mediaEvent);

/**
* Requests to send an event to the music manager.
* @param handle The Cortana handle.
* @param event The type of event
* @param context Event specific context, defined below
*                EVENT_PREFETCH, NEXT, PREV --> CORTANA_FETCH_INFO
*                EVENT_REPORT_PLAYTIME --> CORTANA_PLAY_REPORT_INFO
*                EVENT_PLAYBACK_CHANGED --> CORTANA_PLAY_UPDATE_INFO
*                EVENT_VOLUME_CHANGED --> CORTANA_VOLUME_CHANGED_INFO
*                EVENT_PLAYBACK_ERROR --> CORTANA_PLAYBACK_ERROR_INFO
*/
void cortana_mediacontrol_sendevent(
    CORTANA_HANDLE handle,
    CORTANA_EVENT_TYPE event,
    void* context);

/**
* Requests to set volume.
* @param handle The Cortana handle.
* @param amount The amoutn to change or set
* @param action Type of volume change action such as up, down or set
*/
void cortana_mediacontrol_setvolume(
    CORTANA_HANDLE handle,
    enum cortana_audio_type group,
    short amount,
    enum cortana_volume_change_type action);

/**
* Sets media control options for players.
* @param handle The Cortana handle.
* @param option The media option. Refer to the cortana_media_option
* enumeration.
*/
void cortana_mediacontrol_setoption(
    CORTANA_HANDLE handle,
    CORTANA_MEDIA_OPTION option);

/**
* Requests to set position.
* @param handle The Cortana handle.
* @param event The type of event
* @param context Any additional context
*/
void cortana_mediacontrol_setposition(
    CORTANA_HANDLE handle,
    int position);

/**
* Gets the current audio device
* @param handle The Cortana handle.
* @param type The device type to retrieve.
* @return The current audio device handle.
*/
AUDIO_SYS_HANDLE cortana_getaudiodevice(
    CORTANA_HANDLE          hCortana,
    enum cortana_audio_type type
);

/**
* Sets the audio device name for a device type.
* @param handle The Cortana handle.
* @param type The device type to set.
* @return A return code or zero if successful.
*/
int cortana_setaudiodevice(
    CORTANA_HANDLE          hCortana,
    enum cortana_audio_type type,
    const char*             pszDeviceName
);

/**
* Plays an audio file from disk.
* @param handle The Cortana handle.
* @param type The audio device that should be rendered.
* @param pszFileName A NULL terminated file path.
* @return A return code or zero if successful.
*/
int  cortana_playfile(
    CORTANA_HANDLE          handle,
    enum cortana_audio_type type,
    const char*             pszFileName);

/**
* Gets the current volume level for the requested audio group.
* @param handle The Cortana handle.
* @param audioGroup The audio group.
* @return The current audio level.  A value from 0 to 100 or < 0 on failure.
*/
int cortana_getvolume(CORTANA_HANDLE handle, enum cortana_audio_type audioGroup);

/**
* Raises the volume by the given amount
* @param handle The Cortana handle.
* @param changeAmount The amount to raise (0-100)
*/
void cortana_change_volume(CORTANA_HANDLE handle, short changeAmount, enum cortana_audio_type group, enum cortana_volume_change_type type);

/**
* Dumps Cortana's audio buffers to a WAV file.  This API exists to help debug
* sound quality issues.
* @param handle The Cortana handle.
* @param pszPath The path to write or overwrite the WAV file.
*/
void cortana_dump_audio_buffer(
    CORTANA_HANDLE handle,
    const char* pszPath);

/*
* --------------------------------------------------------------------------------------
*                       Send events to Cortana Server
* -------------------------------------------------------------------------------------
*/
/**
* Sends a fetch (e.g. prefetch, next) state or action event up to the Cortana cloud
* @param handle The Cortana handle.
* @param event Type of event that is being sent
* @param info Info required for the event that is being sent
*/
void cortana_send_fetch_event(
    CORTANA_HANDLE handle,
    CORTANA_EVENT_TYPE event,
    CORTANA_FETCH_INFO info);

/**
* Sends a control change (e.g. repeat,shuffle)  event up to the Cortana cloud
* @param handle The Cortana handle.
* @param event Type of event that is being sent
* @param info Info required for the event that is being sent
*/
void cortana_send_control_change_event(
    CORTANA_HANDLE handle,
    CORTANA_EVENT_TYPE event,
    CORTANA_PLAY_UPDATE_INFO info);

/**
* Sends a playtime report event up to the Cortana cloud
* @param handle The Cortana handle.
* @param info Info required for the event that is being sent
*/
void cortana_send_playtime_event(
    CORTANA_HANDLE handle,
    CORTANA_PLAY_REPORT_INFO info);

/**
* Sends a playback changed event up to the Cortana cloud
* @param handle The Cortana handle.
* @param info Info required for the event that is being sent
*/
void cortana_send_playback_event(
    CORTANA_HANDLE handle,
    CORTANA_PLAY_UPDATE_INFO info);

/**
* Sends a playback error event to the Cortana cloud
* @param handle The Cortana handle.
* @param info info required for the error event
*/
void cortana_send_playback_error_event(
    CORTANA_HANDLE handle,
    CORTANA_PLAYBACK_ERROR_INFO info);

/**
* Sends a volume changed event up to the Cortana cloud
* @param handle The Cortana handle.
* @param info Info required for the event that is being sent
*/
void cortana_send_volume_event(
    CORTANA_HANDLE handle,
    CORTANA_VOLUME_CHANGED_INFO info);

/*
* --------------------------------------------------------------------------------------
*                       Music Adapter Functions
* -------------------------------------------------------------------------------------
*/
/**
* Stops the built-in music player's current track
*/
void cortana_music_stop(SPEECH_HANDLE handle);

/**
* Pauses the built-in music player's current track
*/
void cortana_music_pause(SPEECH_HANDLE handle);

/**
* Resumes the music player's current track
*/
void cortana_music_resume(SPEECH_HANDLE handle);

/**
* Skips to the next song on the built-in player
*/
void cortana_music_next(SPEECH_HANDLE handle);

/**
* Goes back to the previous track on the built-in player
*/
void cortana_music_prev(SPEECH_HANDLE handle);

/**
* Sets the shuffle state
*/
void cortana_music_set_shuffle(SPEECH_HANDLE handle, enum cortana_shuffle_state state);

/**
* sets the repeat state
*/
void cortana_music_set_repeat(SPEECH_HANDLE handle, enum cortana_repeat_state state);

/**
* Sets the rating  state
*/
void cortana_music_set_rating(SPEECH_HANDLE handle, enum cortana_rating_state state);

/**
* Sets the current position of the music stream
*/

void cortana_music_set_play_position(int64_t newTime);

/**
* Plays a music stream
* @param handle The Cortana handle.
* @param url A NULL terminated URL resource.
*/
void cortana_music_playurl(SPEECH_HANDLE handle, AudioPlayer_PlayStream* playStream);

/**
* Retrieves the elapsed time (in ms) for the currently playing song
* @return The playtime (in ms)
*/
int cortana_music_get_playtime();

/**
* Retrieves the total time (in ms) for the currently playing song
* @return The total time of the track (in ms)
*/
int cortana_music_get_totaltime();

/**
* Returns if any music is playing
* @return 1 if any music is playing
*/
int cortana_music_get_isplaying();

/**
* Retrieves the title of the currently playing song
* @return The title of the song
*/
STRING_HANDLE cortana_music_get_title();

/**
* Retrieves the artist of the currently playing song
* @return The artist of the song
*/
STRING_HANDLE cortana_music_get_artist();

/**
* Retrieves the album of the currently playing song
* @return The album of the song
*/
STRING_HANDLE cortana_music_get_album();

/**
* Retrieves the item type of the currently playing song
* @return The item type of the song
*/
STRING_HANDLE cortana_music_get_itemtype();

/**
* Requests next song from music playlist
* (usage is when user presses the next button)
*/
void cortana_music_request_next(SPEECH_HANDLE handle);

/**
* Requests previous song from music playlist
* (usage is when user presses the previous button)
*/
void cortana_music_request_previous(SPEECH_HANDLE handle);

/**
* Enqueues prefetched song to current playlist
*/
void cortana_music_prefetch(SPEECH_HANDLE handle, AudioPlayer_PlayStream* playStream);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_MANAGER_H */