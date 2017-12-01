// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PRIVATE_AUDIO_MANAGER_H
#define PRIVATE_AUDIO_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif
 
#include "audio_manager.h"

/**
* The CORTANA_PLAY_UPDATE_INFO type represents a structure used to hold all fields
* required for a now playing update from a music player
*/
#define CORTANA_PLAY_UPDATE_INFO_INTERNAL_DELETE(var) STRING_delete(var.provider); STRING_delete(var.artist);  \
                                                      STRING_delete(var.title); STRING_delete(var.album); STRING_delete(var.imageUri); \
                                                      STRING_delete(var.trackId); STRING_delete(var.streamUri); STRING_delete(var.itemType);\

typedef struct _CORTANA_PLAY_UPDATE_INFO_INTERNAL
{
	enum cortana_mediactrl_state state;
	enum cortana_shuffle_state shuffleState;
	enum cortana_repeat_state repeatState;
	enum cortana_rating_state ratingState;
	STRING_HANDLE provider;
	unsigned int duration;
	unsigned int position;
	STRING_HANDLE artist;
	STRING_HANDLE title;
	STRING_HANDLE album;
	STRING_HANDLE imageUri;
	STRING_HANDLE trackId;
	STRING_HANDLE streamUri;
	STRING_HANDLE itemType;
} CORTANA_PLAY_UPDATE_INFO_INTERNAL;

/**
* The CORTANA_PLAY_REPORT_INFO type represents a structure used to hold all fields
* required for a music heartbeat from device to service
*/
#define CORTANA_PLAY_REPORT_INFO_INTERNAL_DELETE(var) CORTANA_PLAY_UPDATE_INFO_INTERNAL_DELETE(var);
#define CORTANA_PLAY_REPORT_INFO_INTERNAL CORTANA_PLAY_UPDATE_INFO_INTERNAL

/*
* The CORTANA_FETCH_INFO type represents a structure used to hold fields required
* for next, previous, and prefetch request events
*/
#define CORTANA_FETCH_INFO_INTERNAL_DELETE(var) STRING_delete(var.provider); STRING_delete(var.trackId);
typedef struct _CORTANA_FETCH_INFO_INTERNAL
{
	STRING_HANDLE provider;
	STRING_HANDLE trackId;
} CORTANA_FETCH_INFO_INTERNAL;

/*
* The CORTANA_VOLUME_CHANGED_INFO type represents a structure used to hold fields required
* for a volume changed event
*/
#define CORTANA_VOLUME_CHANGED_INFO_INTERNAL_DELETE(var) (void)var;
typedef struct _CORTANA_VOLUME_CHANGED_INFO_INTERNAL
{
	unsigned int volume;
	enum cortana_audio_type audioGroup;
} CORTANA_VOLUME_CHANGED_INFO_INTERNAL;

/*
* The CORTANA_PLAYBACK_ERROR_INFO type represents a structure used to hold fields required
* for an error event that is encounted when trying to playback a song
*/
#define CORTANA_PLAYBACK_ERROR_INFO_INTERNAL_DELETE(var) STRING_delete(var.provider); STRING_delete(var.trackId); STRING_delete(var.streamUri);STRING_delete(var.errorMessage);
typedef struct _CORTANA_PLAYBACK_ERROR_INFO_INTERNAL
{
	STRING_HANDLE provider;
	STRING_HANDLE trackId;
	STRING_HANDLE streamUri;
	int errorCode;
	STRING_HANDLE errorMessage;
	int isHttpError;
} CORTANA_PLAYBACK_ERROR_INFO_INTERNAL;

/**
* Indicates whether or not audio is ducked.
* @param handle The Cortana handle.
* @return A value from cortana_ducking_state.
*/
enum cortana_ducking_state cortana_duck_getstate(
    CORTANA_HANDLE handle);

/**
* Initializes the music player
* @param handle The Cortana handle.
* @return A return code or zero if successful
*/
int cortana_music_initialize(SPEECH_HANDLE handle);

/**
* Deinitializes the music player
*/
void cortana_music_shutdown();

/**
* Mutes the speakers so no music is heard (still playing in the background)
* @param handle The Cortana handle.
*/
void cortana_music_mute(SPEECH_HANDLE handle);

/**
* Unmutes the speakers
* @param handle The Cortana handle.
*/
void cortana_music_unmute(SPEECH_HANDLE handle);


#ifdef __cplusplus
}
#endif

#endif /* PRIVATE_AUDIO_MANAGER_H */