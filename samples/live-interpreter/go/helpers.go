// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md in the project root.

package main

import (
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"sync"
	"time"
)

func requiredEnvironment(name string) (string, error) {
	value := os.Getenv(name)
	if value == "" {
		return "", fmt.Errorf("missing required environment variable: %s", name)
	}
	return value, nil
}

// This sample deliberately accepts only 16-kHz, mono, 16-bit PCM RIFF/WAVE.
func validateWav(input io.ReadSeeker) error {
	invalid := func() error {
		return fmt.Errorf("invalid WAV: use a nonempty 16-kHz, mono, 16-bit PCM RIFF/WAVE file")
	}
	length, err := input.Seek(0, io.SeekEnd)
	if err != nil {
		return fmt.Errorf("seek WAV: %w", err)
	}
	if length < 12 {
		return invalid()
	}
	if _, err := input.Seek(0, io.SeekStart); err != nil {
		return fmt.Errorf("seek WAV header: %w", err)
	}
	var header [12]byte
	if _, err := io.ReadFull(input, header[:]); err != nil {
		return fmt.Errorf("read WAV header: %w", err)
	}
	if string(header[:4]) != "RIFF" || string(header[8:]) != "WAVE" ||
		int64(binary.LittleEndian.Uint32(header[4:8]))+8 != length {
		return invalid()
	}
	formatFound, dataFound := false, false
	for position := int64(12); position < length; {
		var chunk [8]byte
		if length-position < 8 {
			return invalid()
		}
		if _, err := io.ReadFull(input, chunk[:]); err != nil {
			return fmt.Errorf("read WAV chunk: %w", err)
		}
		size := int64(binary.LittleEndian.Uint32(chunk[4:]))
		position += 8
		paddedSize := size + size%2
		if paddedSize > length-position {
			return invalid()
		}
		switch string(chunk[:4]) {
		case "fmt ":
			if formatFound || dataFound || size < 16 {
				return invalid()
			}
			var format [16]byte
			if _, err := io.ReadFull(input, format[:]); err != nil {
				return fmt.Errorf("read WAV format: %w", err)
			}
			if binary.LittleEndian.Uint16(format[:2]) != 1 ||
				binary.LittleEndian.Uint16(format[2:4]) != 1 ||
				binary.LittleEndian.Uint32(format[4:8]) != 16000 ||
				binary.LittleEndian.Uint32(format[8:12]) != 32000 ||
				binary.LittleEndian.Uint16(format[12:14]) != 2 ||
				binary.LittleEndian.Uint16(format[14:16]) != 16 {
				return invalid()
			}
			formatFound = true
		case "data":
			if !formatFound || dataFound || size == 0 || size%2 != 0 {
				return invalid()
			}
			dataFound = true
		}
		position += paddedSize
		if _, err := input.Seek(position, io.SeekStart); err != nil {
			return fmt.Errorf("seek WAV chunk: %w", err)
		}
	}
	if !formatFound || !dataFound {
		return invalid()
	}
	return nil
}

func validateWavFile(path string) error {
	file, err := os.Open(path)
	if err != nil {
		return fmt.Errorf("open input WAV: %w", err)
	}
	validationErr := validateWav(file)
	closeErr := file.Close()
	if validationErr != nil {
		return validationErr
	}
	if closeErr != nil {
		return fmt.Errorf("close input WAV: %w", closeErr)
	}
	return nil
}

func writeNewFile(path string, data []byte) error {
	file, err := os.OpenFile(path, os.O_WRONLY|os.O_CREATE|os.O_EXCL, 0600)
	if err != nil {
		return fmt.Errorf("create new output (existing files are never overwritten): %w", err)
	}
	n, writeErr := file.Write(data)
	closeErr := file.Close()
	if writeErr != nil {
		return fmt.Errorf("write output; a partial file may remain: %w", writeErr)
	}
	if n != len(data) {
		return fmt.Errorf("write output; a partial file may remain: %w", io.ErrShortWrite)
	}
	if closeErr != nil {
		return fmt.Errorf("close output; a partial file may remain: %w", closeErr)
	}
	return nil
}

type sessionState struct {
	mu         sync.Mutex
	done       chan struct{}
	finished   bool
	err        error
	audioIndex uint64
}

func newSessionState() *sessionState {
	return &sessionState{done: make(chan struct{})}
}

func (state *sessionState) recordFailureLocked(err error) {
	if state.err == nil && err != nil {
		state.err = err
		fmt.Fprintln(os.Stderr, "Live Interpreter callback failed:", err)
	}
}

// Only terminal session events call finishLocked.
func (state *sessionState) finishLocked(err error) {
	state.recordFailureLocked(err)
	if !state.finished {
		state.finished = true
		close(state.done)
	}
}

func (state *sessionState) failure() error {
	state.mu.Lock()
	defer state.mu.Unlock()
	return state.err
}

// Bound native calls too. On timeout, don't race native threads by trying to
// Close their resources; the process exits without normal SDK cleanup.
func startWatchdog(timeout time.Duration) *time.Timer {
	return time.AfterFunc(timeout, func() {
		fmt.Fprintln(os.Stderr, "Live Interpreter completion timeout; terminating without SDK cleanup.")
		os.Exit(1)
	})
}
