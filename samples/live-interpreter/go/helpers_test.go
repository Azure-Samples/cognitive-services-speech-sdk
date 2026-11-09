// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md in the project root.

package main

import (
	"bytes"
	"context"
	"encoding/binary"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"sync"
	"testing"
	"time"
)

func testWav() []byte {
	data := []byte{
		'R', 'I', 'F', 'F', 38, 0, 0, 0, 'W', 'A', 'V', 'E',
		'f', 'm', 't', ' ', 16, 0, 0, 0, 1, 0, 1, 0,
		0x80, 0x3e, 0, 0, 0, 0x7d, 0, 0, 2, 0, 16, 0,
		'd', 'a', 't', 'a', 2, 0, 0, 0, 0, 0,
	}
	return data
}

func TestValidateWav(t *testing.T) {
	tests := []struct {
		name string
		edit func([]byte) []byte
	}{
		{"empty", func(b []byte) []byte { return nil }},
		{"signature", func(b []byte) []byte { b[0] = 'X'; return b }},
		{"stereo", func(b []byte) []byte { b[22] = 2; return b }},
		{"sample rate", func(b []byte) []byte { b[24] = 0; return b }},
		{"bit depth", func(b []byte) []byte { b[34] = 8; return b }},
		{"chunk bounds", func(b []byte) []byte { b[40] = 100; return b }},
		{"RIFF length", func(b []byte) []byte { b[4] = 37; return b }},
		{"no data", func(b []byte) []byte { copy(b[36:40], "JUNK"); return b }},
		{"float", func(b []byte) []byte { b[20] = 3; return b }},
		{"block alignment", func(b []byte) []byte { b[32] = 4; return b }},
		{"duplicate data", func(b []byte) []byte {
			b = append(b, b[36:]...)
			b[4] = 48
			return b
		}},
		{"truncated", func(b []byte) []byte { return b[:45] }},
		{"no audio", func(b []byte) []byte { b[4] = 36; b[40] = 0; return b[:44] }},
	}
	if err := validateWav(bytes.NewReader(testWav())); err != nil {
		t.Fatal(err)
	}
	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			if err := validateWav(bytes.NewReader(test.edit(testWav()))); err == nil {
				t.Fatal("expected invalid WAV error")
			}
		})
	}
	padded := append([]byte{}, testWav()[:12]...)
	padded = append(padded, []byte{'J', 'U', 'N', 'K', 1, 0, 0, 0, 'x', 0}...)
	padded = append(padded, testWav()[12:]...)
	binary.LittleEndian.PutUint32(padded[4:8], uint32(len(padded)-8))
	if err := validateWav(bytes.NewReader(padded)); err != nil {
		t.Fatal(err)
	}
}

func TestWriteNewFile(t *testing.T) {
	dir, err := ioutil.TempDir(".", "live-interpreter-test-")
	if err != nil {
		t.Fatal(err)
	}
	defer os.RemoveAll(dir)
	path := filepath.Join(dir, "translation-1.wav")
	want := []byte{1, 2, 3}
	if err := writeNewFile(path, want); err != nil {
		t.Fatal(err)
	}
	if err := writeNewFile(path, []byte{4}); err == nil {
		t.Fatal("expected exclusive-create failure")
	}
	got, err := ioutil.ReadFile(path)
	if err != nil || !bytes.Equal(got, want) {
		t.Fatalf("existing output changed: %v, %v", got, err)
	}
	if err := writeNewFile(filepath.Join(dir, "missing", "out.wav"), want); err == nil {
		t.Fatal("expected write error")
	}
}

func TestSessionState(t *testing.T) {
	state := newSessionState()
	var callbacks sync.WaitGroup
	for i := 0; i < 20; i++ {
		callbacks.Add(1)
		go func() {
			defer callbacks.Done()
			state.mu.Lock()
			defer state.mu.Unlock()
			state.finishLocked(nil)
		}()
	}
	callbacks.Wait()
	select {
	case <-state.done:
	default:
		t.Fatal("completion not signaled")
	}
	// A late failure must not be hidden by an earlier normal completion.
	state.mu.Lock()
	state.finishLocked(os.ErrPermission)
	state.mu.Unlock()
	if state.failure() != os.ErrPermission {
		t.Fatal("lost late failure")
	}
}

func TestCallbackFailureDoesNotFinishSession(t *testing.T) {
	state := newSessionState()
	state.mu.Lock()
	state.recordFailureLocked(os.ErrPermission)
	state.mu.Unlock()
	if state.failure() != os.ErrPermission {
		t.Fatal("callback failure was not recorded")
	}
	select {
	case <-state.done:
		t.Fatal("a callback failure must not signal session completion")
	default:
	}
	state.mu.Lock()
	state.finishLocked(nil)
	state.mu.Unlock()
	select {
	case <-state.done:
	default:
		t.Fatal("terminal completion was not signaled")
	}
	if state.failure() != os.ErrPermission {
		t.Fatal("terminal completion lost the callback failure")
	}
}

func TestRequiredEnvironment(t *testing.T) {
	const name = "LIVE_INTERPRETER_HELPER_TEST"
	old, present := os.LookupEnv(name)
	defer func() {
		if present {
			os.Setenv(name, old)
		} else {
			os.Unsetenv(name)
		}
	}()
	if err := os.Setenv(name, ""); err != nil {
		t.Fatal(err)
	}
	if _, err := requiredEnvironment(name); err == nil {
		t.Fatal("expected missing environment error")
	}
	const endpoint = "wss://example.invalid/stt/speech/universal/v2?setfeature=llm&other=a%2Bb"
	if err := os.Setenv(name, endpoint); err != nil {
		t.Fatal(err)
	}
	value, err := requiredEnvironment(name)
	if err != nil || value != endpoint {
		t.Fatalf("environment value changed: %q, %v", value, err)
	}
}

func TestCompletionWatchdog(t *testing.T) {
	const childFlag = "LIVE_INTERPRETER_WATCHDOG_TEST"
	if os.Getenv(childFlag) == "1" {
		startWatchdog(30 * time.Millisecond)
		time.Sleep(2 * time.Second)
		return
	}
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()
	cmd := exec.CommandContext(ctx, os.Args[0], "-test.run=^TestCompletionWatchdog$")
	cmd.Env = append(os.Environ(), childFlag+"=1")
	output, err := cmd.CombinedOutput()
	exit, ok := err.(*exec.ExitError)
	if !ok || exit.ExitCode() != 1 || !bytes.Contains(output, []byte("completion timeout")) {
		t.Fatalf("expected watchdog exit 1, got %v: %s", err, output)
	}
}

func TestCompletionWatchdogCanStop(t *testing.T) {
	timer := startWatchdog(time.Hour)
	if !timer.Stop() {
		t.Fatal("watchdog was already running")
	}
}
