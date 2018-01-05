//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// metricids.h: definition of event names in metric messages.
//

// All telemetry event names are defined here because our backend enforces a
// limit of 100 unique event names.  Keep the list of event names here to less
// than 85 to avoid alerts about the number of unique event names.
//
// Keep this list in alphabetical order, for ease of maintenance.
//
// This file is included into metrics.h and metrics.c after defining the
// DEFINE_METRIC_ID macro.  Don't put commas or semicolons at the end of each
// line.

DEFINE_METRIC_ID(audio_stream_data)
DEFINE_METRIC_ID(audio_stream_flush)
DEFINE_METRIC_ID(audio_stream_init)
DEFINE_METRIC_ID(error)
DEFINE_METRIC_ID(tts_first_chunk)
DEFINE_METRIC_ID(tts_last_chunk)
DEFINE_METRIC_ID(unexpected_requestid)
DEFINE_METRIC_ID(xport_state_end)
DEFINE_METRIC_ID(xport_state_start)
