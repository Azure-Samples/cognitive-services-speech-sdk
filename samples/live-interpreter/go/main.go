// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md in the project root.

package main

import (
	"fmt"
	"os"
	"time"

	"github.com/Microsoft/cognitive-services-speech-sdk-go/audio"
	"github.com/Microsoft/cognitive-services-speech-sdk-go/common"
	"github.com/Microsoft/cognitive-services-speech-sdk-go/speech"
)

func configureTranslation(config *speech.SpeechTranslationConfig) error {
	if err := config.AddTargetLanguage("fr"); err != nil {
		return fmt.Errorf("add target language: %w", err)
	}
	if err := config.SetVoiceName("personal-voice"); err != nil {
		return fmt.Errorf("set voice: %w", err)
	}
	if err := config.SetSpeechSynthesisOutputFormat(common.Riff16Khz16BitMonoPcm); err != nil {
		return fmt.Errorf("set RIFF synthesis output format: %w", err)
	}
	return nil
}

func run() error {
	key, err := requiredEnvironment("SPEECH_KEY")
	if err != nil {
		return err
	}
	endpoint, err := requiredEnvironment("SPEECH_ENDPOINT")
	if err != nil {
		return err
	}
	input, err := requiredEnvironment("LIVE_INTERPRETER_AUDIO_FILE")
	if err != nil {
		return err
	}
	if err := validateWavFile(input); err != nil {
		return err
	}

	// Pass the universal v2 URL unchanged, including any query parameters.
	config, err := speech.NewSpeechTranslationConfigFromEndpointWithSubscription(endpoint, key)
	if err != nil {
		return fmt.Errorf("create translation config: %w", err)
	}
	defer config.Close()
	if err := configureTranslation(config); err != nil {
		return err
	}
	language, err := speech.NewAutoDetectSourceLanguageConfigFromOpenRange()
	if err != nil {
		return fmt.Errorf("create open-range language detection: %w", err)
	}
	defer language.Close()
	audioConfig, err := audio.NewAudioConfigFromWavFileInput(input)
	if err != nil {
		return fmt.Errorf("create WAV input: %w", err)
	}
	defer audioConfig.Close()
	recognizer, err := speech.NewTranslationRecognizerFromAutoDetectSourceLangConfig(config, language, audioConfig)
	if err != nil {
		return fmt.Errorf("create translation recognizer: %w", err)
	}
	defer recognizer.Close()
	state := newSessionState()

	recognizer.Recognized(func(event speech.TranslationRecognitionEventArgs) {
		state.mu.Lock()
		defer state.mu.Unlock()
		defer event.Close()
		// In v1.53.0, event.Close only releases the event, not its result.
		defer event.Result.Close()
		switch event.Result.Reason {
		case common.TranslatedSpeech, common.RecognizedSpeech:
			detected := event.Result.Properties.GetProperty(common.SpeechServiceConnectionAutoDetectSourceLanguageResult, "")
			fmt.Printf("RECOGNIZED in '%s': %s\n", detected, event.Result.Text)
			for language, translation := range event.Result.GetTranslations() {
				fmt.Printf("  TRANSLATED into '%s': %s\n", language, translation)
			}
		case common.NoMatch:
			fmt.Println("NOMATCH: Speech could not be recognized.")
		default:
			state.recordFailureLocked(fmt.Errorf("unexpected recognition result: %v", event.Result.Reason))
		}
	})

	recognizer.Synthesizing(func(event speech.TranslationSynthesisEventArgs) {
		state.mu.Lock()
		defer state.mu.Unlock()
		// SDK v1.53.0 leaves the synthesis event handle unset and exposes no
		// result Close method. Calling its inherited Close cannot free those
		// native handles; this is a binding limitation, not leak-free cleanup.
		if event.Result.Reason == common.Canceled {
			state.recordFailureLocked(fmt.Errorf("translation synthesis was canceled"))
			return
		}
		data := event.Result.GetAudioData()
		// An empty chunk ends a phrase, not the recognition session.
		if len(data) == 0 || state.err != nil {
			return
		}
		state.audioIndex++
		output := fmt.Sprintf("translation-%d.wav", state.audioIndex)
		if err := writeNewFile(output, data); err != nil {
			state.recordFailureLocked(err)
			return
		}
		fmt.Printf("AUDIO: %s (%d bytes)\n", output, len(data))
	})

	recognizer.Canceled(func(event speech.TranslationRecognitionCanceledEventArgs) {
		state.mu.Lock()
		defer state.mu.Unlock()
		defer event.Close()
		defer event.Result.Close()
		fmt.Printf("CANCELED: Reason=%v\n", event.Reason)
		// EndOfStream is normal for WAV input. All other cancellations fail.
		if event.Reason != common.EndOfStream {
			state.finishLocked(fmt.Errorf("translation canceled: reason=%v code=%v details=%s",
				event.Reason, event.ErrorCode, event.ErrorDetails))
		} else {
			state.finishLocked(nil)
		}
	})

	recognizer.SessionStopped(func(event speech.SessionEventArgs) {
		state.mu.Lock()
		defer state.mu.Unlock()
		defer event.Close()
		fmt.Println("Session stopped.")
		state.finishLocked(nil)
	})

	fmt.Println("Start translation to fr...")
	if err := <-recognizer.StartContinuousRecognitionAsync(); err != nil {
		return fmt.Errorf("start continuous recognition: %w", err)
	}
	<-state.done
	// Never call Stop or Close from an SDK callback.
	stopErr := <-recognizer.StopContinuousRecognitionAsync()
	recognizer.Recognized(nil)
	recognizer.Synthesizing(nil)
	recognizer.Canceled(nil)
	recognizer.SessionStopped(nil)
	if stopErr != nil {
		return fmt.Errorf("stop continuous recognition: %v (session error: %v)", stopErr, state.failure())
	}
	return state.failure()
}

func main() {
	watchdog := startWatchdog(5 * time.Minute)
	err := run()
	watchdog.Stop()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Live Interpreter failed:", err)
		os.Exit(1)
	}
}
