// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md in the project root.

package main

import (
	"testing"

	"github.com/Microsoft/cognitive-services-speech-sdk-go/common"
	"github.com/Microsoft/cognitive-services-speech-sdk-go/speech"
)

func TestTranslationOutputFormat(t *testing.T) {
	config, err := speech.NewSpeechTranslationConfigFromEndpoint(
		"wss://example.invalid/stt/speech/universal/v2?setfeature=llm")
	if err != nil {
		t.Fatal(err)
	}
	defer config.Close()
	if err := config.SetSpeechSynthesisOutputFormat(common.Riff8Khz16BitMonoPcm); err != nil {
		t.Fatal(err)
	}
	if err := configureTranslation(config); err != nil {
		t.Fatal(err)
	}
	const want = "riff-16khz-16bit-mono-pcm"
	if got := config.SpeechSynthesisOutputFormat(); got != want {
		t.Fatalf("output format = %q; want explicit %q", got, want)
	}
	t.Logf("SDK output format: %s", config.SpeechSynthesisOutputFormat())
}
