// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "SpeechAPI.h"
typedef SPEECH_HANDLE   CORTANA_HANDLE;

enum cortana_mediactrl_state
{
    CORTANA_MEDIACTRL_PLAY      = 1,
    CORTANA_MEDIACTRL_PAUSE     = 2,
    CORTANA_MEDIACTRL_RESUME    = 3,
    CORTANA_MEDIACTRL_STOP      = 4,
    CORTANA_MEDIACTRL_NEXT      = 7,
    CORTANA_MEDIACTRL_PREV      = 8,
};

enum cortana_audio_type
{
    CORTANA_AUDIO_TYPE_DEFAULT = 0,
    CORTANA_AUDIO_TYPE_SPEECH,
    CORTANA_AUDIO_TYPE_VOICE,
    CORTANA_AUDIO_TYPE_MUSIC,
    CORTANA_AUDIO_TYPE_TIMER,
    CORTANA_AUDIO_TYPE_ALARM,
    CORTANA_AUDIO_TYPE_EARCON,
    CORTANA_AUDIO_TYPE_TELEPHONY_INPUT,
    CORTANA_AUDIO_TYPE_TELEPHONY_OUT,
    CORTANA_AUDIO_TYPE_LAST
};

enum cortana_volume_change_type
{
    CORTANA_VOLUME_UP = 0,
    CORTANA_VOLUME_DOWN,
    CORTANA_VOLUME_ABSOLUTE
};

enum cortana_ducking_state
{
    AUDIO_DUCKING_RESTORED = 0,
    AUDIO_DUCKING_DUCKED = 1,
};

typedef enum {
    EVENT_INVALID = 0x0,
    EVENT_PREFETCH = 0x1,
    EVENT_REPORT_PLAYTIME = 0x2,
    EVENT_REQUEST_NEXT = 0x4,
    EVENT_REQUEST_PREV = 0x8,
    EVENT_PLAYBACK_CHANGED = 0x10,
    EVENT_VOLUME_CHANGED = 0x20,
    EVENT_PLAYBACK_ERROR = 0x40,
    EVENT_SHUFFLE_CHANGED = 0x80,
    EVENT_REPEAT_CHANGED = 0x100,
    EVENT_RATING_CHANGED = 0x200
} CORTANA_EVENT_TYPE;

typedef enum {
    MEDIA_OPTION_SHUFFLE,
    MEDIA_OPTION_SHUFFLE_OFF,
    MEDIA_OPTION_REPEAT_ALL,
    MEDIA_OPTION_REPEAT_ONE,
    MEDIA_OPTION_REPEAT_OFF,
    MEDIA_OPTION_MUTE,
    MEDIA_OPTION_UNMUTE,
    MEDIA_OPTION_RATING_LIKED,
    MEDIA_OPTION_RATING_DISLIKED,
    MEDIA_OPTION_RATING_UNRATED
} CORTANA_MEDIA_OPTION;

/**
* The PCORTANA_AUDIODUCK_CALLBACK type represents an application-defined
* status callback function used to give the application to handle audio
* ducking events.
* @param handle The Cortana handle.
* @param pContext A pointer to the application-defined callback context.
* @param state Ducking state. AUDIO_DUCKING_DUCKED when audio is being ducking,
* and AUDIO_DUCKING_RESTORED when audio has been restored.
*/
typedef void(*PCORTANA_AUDIODUCK_CALLBACK)(
    CORTANA_HANDLE             handle,
    void*                      pContext,
    enum cortana_ducking_state state);

/**
* The PCORTANA_MEDIACONTROL_CALLBACK type represents a status callback function
* used to notify the caller of media control events.
* @param handle The Cortana handle.
* @param pContext A pointer to the application-defined callback context.
* @param state The new state to be transitioned to.
*/
typedef void(*PCORTANA_MEDIACONTROL_CALLBACK)(
    CORTANA_HANDLE               handle,
    void*                        pContext,
    enum cortana_mediactrl_state state);

/**
* The PCORTANA_MEDIAEVENT_CALLBACK type represents an event callback function
* @param handle The Cortana handle.
* @param event The event which is triggered
* @param pContext Event-specific context, such as CORTANA_PLAY_UPDATE_INFO
*/
typedef void(*PCORTANA_MEDIAEVENT_CALLBACK)(
    CORTANA_HANDLE               handle,
    CORTANA_EVENT_TYPE           event,
    void*                        pContext);

/**
* The PCORTANA_MEDIAVOLUME_CALLBACK type represents a volume action callback function
* @param handle The Cortana handle.
* @param group The volume group affected (eg music)
* @param amount The amount to change or set
* @param action Volume up, down or set
*/
typedef void(*PCORTANA_MEDIAVOLUME_CALLBACK)(
    CORTANA_HANDLE                    handle,
    enum cortana_audio_type           group,
    short                             amount,
    enum cortana_volume_change_type   action);

/**
* The PCORTANA_MEDIAOPTION_CALLBACK type represents a media option action callback function
* @param handle The Cortana handle.
* @param option Shuffle/Repeat/Mute/etc.
*/
typedef void(*PCORTANA_MEDIAOPTION_CALLBACK)(
    CORTANA_HANDLE               handle,
    CORTANA_MEDIA_OPTION         option);

/**
* The PCORTANA_MEDIAPOSITION_CALLBACK type represents a media seek action callback function
* @param handle The Cortana handle.
* @param position The new track position
*/
typedef void(*PCORTANA_MEDIAPOSITION_CALLBACK)(
    CORTANA_HANDLE               handle,
    int64_t                      position);

/**
* The CORTANA_AUDIO_PLAYER type represents a structure used to register for
* all various audio player events.
*/
typedef struct _CORTANA_AUDIO_PLAYER
{
    const char*                      pszPlayerName;
    enum cortana_audio_type          audioType;
    PCORTANA_AUDIODUCK_CALLBACK      OnAudioDuck;
    PCORTANA_MEDIACONTROL_CALLBACK   OnMediaControl;
    PCORTANA_MEDIAEVENT_CALLBACK     OnMediaEvent;
    PCORTANA_MEDIAVOLUME_CALLBACK    OnSetVolume;
    PCORTANA_MEDIAOPTION_CALLBACK    OnSetOption;
    PCORTANA_MEDIAPOSITION_CALLBACK  OnSetPosition;
} CORTANA_AUDIO_PLAYER;

/**
* Registers a new audio player source.
* @param handle The Cortana handle.
* @param pPlayer The new player definition and callbacks.
* @param pContext The application defined context to return.
* @return A return code or zero if successful.
*/
int cortana_register_player(
    CORTANA_HANDLE               handle,
    const CORTANA_AUDIO_PLAYER*  pPlayer,
    void *                       pContext);

/**
* Unregisters an existing audio player source.
* @param handle The Cortana handle.
* @param pszPlayerName The unique name of the player being unregistered.
*/
void cortana_unregister_player(
    CORTANA_HANDLE handle,
    const char*    pszPlayerName);

/**
* If player wants to request volume change from the audio manager.
* @param handle The Cortana handle.
* @param audioGroup The audio group.
* @return The current audio level.  A value from 0 to 100 or < 0 on failure.
*/
void player_set_volume(CORTANA_HANDLE handle, short amount, enum cortana_volume_change_type action);

#ifdef __cplusplus
}
#endif

#endif /* MUSIC_PLAYER_H */