package com.microsoft.cognitiveservices.speech.samples.ttstextstream;

public class MarkdownStreamFilter {
    // ---------- State Machine ----------
    private enum State {
        NORMAL,                 // Plain text
        ESCAPE,                 // Escape: next character output as-is
        CODE_SPAN,              // Inline code `...`
        CODE_BLOCK,            // Code block ```...```
        EMPHASIS_STAR1,        // *italic*
        EMPHASIS_STAR2,        // **bold**
        EMPHASIS_UND1,         // _italic_
        EMPHASIS_UND2,         // __bold__
        STRIKE,                // ~~strikethrough~~
        LINK_TEXT,             // [link text]
        LINK_URL,              // (link URL)
        IMAGE_ALT,             // ![image alt]
        IMAGE_URL,             // (image URL)
        HTML_TAG,             // <tag>
        FOOTNOTE,             // [^footnote]
        TASK_LIST_MARKER,      // Task list [ ] or [x] (after -)
        HEADING,               // Heading: skip all # characters and following space

        // --- Pending States for Boundary Handling ---
        MAYBE_IMAGE_START,      // Seen '!'
        MAYBE_FOOTNOTE_START,   // Seen '['
        MAYBE_LINK_CLOSE,       // Seen ']' in LINK_TEXT
        MAYBE_IMAGE_ALT_CLOSE,  // Seen ']' in IMAGE_ALT
        MAYBE_TASK_LIST_HYPHEN, // Seen '-' at line start
        MAYBE_TASK_LIST_SPACE,  // Seen '- ' at line start
        PENDING_EMPHASIS,       // Seen '*' or '_' at boundary, waiting for next char
        PENDING_BACKTICK,       // Seen '`', waiting to decide between CODE_SPAN (` or ``) and CODE_BLOCK (```)
        PENDING_TILDE,          // Seen '~', waiting to decide if it's strikethrough (~~)

        // LLM-specific states
        CODE_BLOCK_LANG         // First line after ``` (language identifier)
    }

    // ---------- Internal State ----------
    private State state = State.NORMAL;
    
    // Symbol buffer optimization: use array instead of Queue for better performance
    private final char[] symbolBuffer = new char[3]; // Max 3 consecutive symbols (```)
    private int symbolBufferSize = 0;
    
    private final StringBuilder linkTextBuffer = new StringBuilder(); // Link text cache
    private final StringBuilder sentenceBuffer = new StringBuilder(); // Sentence boundary buffer
    private final StringBuilder htmlTagBuffer = new StringBuilder(); // HTML tag name buffer
    private final StringBuilder footnoteBuffer = new StringBuilder(); // Footnote reference buffer
    
    private boolean atLineStart = true;        // Whether current char is at line start
    private boolean pendingBacktickAtLineStart = false; // Snapshot for PENDING_BACKTICK
    private char lastChar = 0;                // Last output character
    private char pendingEmphasisChar = 0;     // For PENDING_EMPHASIS state
    private Character deferredChar = null;    // Character to process in next iteration (avoids recursion)
    
    // Code block language detection
    private boolean inCodeBlockFirstLine = false; // After ``` on new line
    private final StringBuilder codeBlockLangBuffer = new StringBuilder();

    // ---------- Public API ----------
    
    /**
     * Process input without sentence boundary detection (lowest latency).
     * Returns filtered text immediately, character by character.
     */
    public String process(String input) {
        if (input == null || input.isEmpty()) return "";
        return process(input.toCharArray());
    }

    public String process(char[] input) {
        StringBuilder out = new StringBuilder(input.length);
        for (char c : input) {
            processChar(c, out);
        }
        return out.toString();
    }
    
    /**
     * Process input with sentence boundary detection (recommended for TTS).
     * Returns complete sentences only, buffering incomplete ones.
     * This produces more natural speech prosody at cost of slight latency.
     */
    public String processWithSentenceBoundary(String input) {
        if (input == null || input.isEmpty()) return "";
        
        String filtered = process(input);
        sentenceBuffer.append(filtered);
        
        return extractCompleteSentences();
    }
    
    /**
     * Extract complete sentences from buffer.
     * Sentences end with: . ! ? 。！？ followed by space/newline/end
     */
    private String extractCompleteSentences() {
        StringBuilder result = new StringBuilder();
        String buffer = sentenceBuffer.toString();
        int lastSentenceEnd = -1;
        
        for (int i = 0; i < buffer.length(); i++) {
            char c = buffer.charAt(i);
            // Check for sentence-ending punctuation
            if (c == '.' || c == '!' || c == '?' || c == '。' || c == '！' || c == '？') {
                // Ensure next char is whitespace or end of buffer
                if (i + 1 >= buffer.length() || Character.isWhitespace(buffer.charAt(i + 1))) {
                    lastSentenceEnd = i + 1;
                }
            }
        }
        
        if (lastSentenceEnd > 0) {
            result.append(buffer, 0, lastSentenceEnd);
            sentenceBuffer.delete(0, lastSentenceEnd);
        }
        
        return result.toString();
    }

    public String flush() {
        StringBuilder leftover = new StringBuilder();
        
        switch (state) {
            case LINK_TEXT:
                // Unclosed link: output as plain text with brackets
                leftover.append('[').append(linkTextBuffer);
                break;
            case MAYBE_IMAGE_START:
                leftover.append('!');
                break;
            case MAYBE_FOOTNOTE_START:
                leftover.append('[');
                break;
            case MAYBE_LINK_CLOSE:
                leftover.append('[').append(linkTextBuffer).append(']');
                break;
            case MAYBE_IMAGE_ALT_CLOSE:
                // Incomplete image, output '!' as text
                leftover.append('!');
                break;
            case MAYBE_TASK_LIST_HYPHEN:
                // Incomplete task list, ignore hyphen (was a list marker)
                break;
            case MAYBE_TASK_LIST_SPACE:
                // Incomplete task list, ignore (was list marker)
                break;
            case PENDING_EMPHASIS:
                // Output accumulated emphasis markers as plain text
                for (int i = 0; i < symbolBufferSize; i++) {
                    leftover.append(symbolBuffer[i]);
                }
                if (pendingEmphasisChar != 0) {
                    leftover.append(pendingEmphasisChar);
                }
                break;
            case PENDING_TILDE:
                // Output accumulated tildes as plain text
                for (int i = 0; i < symbolBufferSize; i++) {
                   leftover.append(symbolBuffer[i]);
                }
                break;
            case PENDING_BACKTICK:
                // Output accumulated backticks
                for (int i = 0; i < symbolBufferSize; i++) {
                    leftover.append(symbolBuffer[i]);
                }
                break;
            case CODE_SPAN:
            case CODE_BLOCK:
            case CODE_BLOCK_LANG:
                // Unclosed code: silenced (no output)
                break;
            case EMPHASIS_STAR1:
            case EMPHASIS_STAR2:
            case EMPHASIS_UND1:
            case EMPHASIS_UND2:
            case STRIKE:
                // Unclosed emphasis: content already output, just incomplete closing
                break;
            case IMAGE_ALT:
            case IMAGE_URL:
            case HTML_TAG:
                // Incomplete HTML tag, discard
                htmlTagBuffer.setLength(0);
                break;
            case FOOTNOTE:
                footnoteBuffer.setLength(0);
                break;
            case TASK_LIST_MARKER:
                // These states silence content, incomplete means nothing to output
                break;
            case NORMAL:
            case ESCAPE:
                // No leftover
                break;
        }
        
        // Also flush sentence buffer
        leftover.append(sentenceBuffer);
        
        reset();
        return leftover.toString();
    }

    private void reset() {
        state = State.NORMAL;
        linkTextBuffer.setLength(0);
        sentenceBuffer.setLength(0);
        htmlTagBuffer.setLength(0);
        footnoteBuffer.setLength(0);
        symbolBufferSize = 0;
        atLineStart = true;
        pendingBacktickAtLineStart = false;
        lastChar = 0;
        pendingEmphasisChar = 0;
        deferredChar = null;
        inCodeBlockFirstLine = false;
        codeBlockLangBuffer.setLength(0);
    }

    // ---------- Core Character Processing ----------
    private void processChar(char c, StringBuilder out) {
        deferredChar = c;
        
        while (deferredChar != null) {
            char current = deferredChar;
            deferredChar = null; // Clear for this iteration
            
            switch (state) {
                case NORMAL:            handleNormal(current, out); break;
                case ESCAPE:           handleEscape(current, out); break;
                case CODE_SPAN:        handleCodeSpan(current, out); break;
                case CODE_BLOCK:       handleCodeBlock(current, out); break;
                case EMPHASIS_STAR1:   handleEmphasisStar1(current, out); break;
                case EMPHASIS_STAR2:   handleEmphasisStar2(current, out); break;
                case EMPHASIS_UND1:    handleEmphasisUnd1(current, out); break;
                case EMPHASIS_UND2:    handleEmphasisUnd2(current, out); break;
                case STRIKE:           handleStrike(current, out); break;
                case LINK_TEXT:        handleLinkText(current, out); break;
                case LINK_URL:         handleLinkUrl(current, out); break;
                case IMAGE_ALT:        handleImageAlt(current, out); break;
                case IMAGE_URL:        handleImageUrl(current, out); break;
                case HTML_TAG:         handleHtmlTag(current, out); break;
                case FOOTNOTE:         handleFootnote(current, out); break;
                case TASK_LIST_MARKER: handleTaskListMarker(current, out); break;
                case HEADING:          handleHeading(current, out); break;

                // Pending States
                case MAYBE_IMAGE_START:      handleMaybeImageStart(current, out); break;
                case MAYBE_FOOTNOTE_START:   handleMaybeFootnoteStart(current, out); break;
                case MAYBE_LINK_CLOSE:       handleMaybeLinkClose(current, out); break;
                case MAYBE_IMAGE_ALT_CLOSE:  handleMaybeImageAltClose(current, out); break;
                case MAYBE_TASK_LIST_HYPHEN: handleMaybeTaskListHyphen(current, out); break;
                case MAYBE_TASK_LIST_SPACE:  handleMaybeTaskListSpace(current, out); break;
                case PENDING_EMPHASIS:       handlePendingEmphasis(current, out); break;
                case PENDING_BACKTICK:       handlePendingBacktick(current, out); break;
                case PENDING_TILDE:          handlePendingTilde(current, out); break; // Add handler
                case CODE_BLOCK_LANG:        handleCodeBlockLang(current, out); break;
            }

            // Update line start flag
            if (current == '\n') {
                atLineStart = true;
            } else if (!Character.isWhitespace(current)) {
                atLineStart = false;
            }
        }
    }

    // ---------- NORMAL State Handling ----------
    private void handleNormal(char c, StringBuilder out) {
        // 1. Escape character
        if (c == '\\') {
            state = State.ESCAPE;
            return;
        }

        // 2. Line-start special syntax (headings, lists, quotes)
        if (atLineStart) {
            if (c == '#') {                     // Heading: skip all # characters and following space
                state = State.HEADING;
                return;
            }
            if (c == '>') {                     // Blockquote: skip '>' and optional following space
                out.append(' ');               // Add implicit space for safety
                return;                        // Skip '>'
            }
            if (c == '-') {
                state = State.MAYBE_TASK_LIST_HYPHEN;
                return;
            }
            // NOTE: '*' and '+' at line start are list markers only if followed by space.
            // Without lookahead, we can't determine this in streaming mode.
            // Conservative approach: only filter '-' lists. Output '*' and '+' as-is.
            // This prevents false positives with sentences starting with * or +.
            if (Character.isDigit(c)) {
                out.append(c);
                lastChar = c;
                return;
            }
        }

        // 3. Code markers
        if (c == '`') {
            state = State.PENDING_BACKTICK;
            pendingBacktickAtLineStart = atLineStart;
            pushSymbol(c);
            return;
        }
        
        // Markdown table separator: replace | with space for TTS
        if (c == '|') {
            out.append(' ');
            lastChar = ' ';
            return;
        }

        // 4. Emphasis markers (Delegated to PENDING_EMPHASIS to handle lookahead)
        if (c == '*' || c == '_') {
             state = State.PENDING_EMPHASIS;
             pendingEmphasisChar = c;
             // Do not push to symbolBuffer yet, handlePendingEmphasis will decide
             return;
        }

        // 5. Strikethrough
        if (c == '~') {
            state = State.PENDING_TILDE;
            pushSymbol(c);
            return;
        }

        // 6. Links, images, footnotes
        if (c == '[') {
            state = State.MAYBE_FOOTNOTE_START; // Wait for possible '^'
            return;
        }
        if (c == '!') {
            state = State.MAYBE_IMAGE_START; // Wait for possible '['
            return;
        }

        // 7. HTML tags
        if (c == '<') {
            state = State.HTML_TAG;
            return;
        }

        // 8. Plain characters
        out.append(c);
        lastChar = c;
    }

    // ---------- State Handlers ----------
    private void handleEscape(char c, StringBuilder out) {
        out.append(c);
        lastChar = c;
        state = State.NORMAL;
    }

    private void handleCodeSpan(char c, StringBuilder out) {
        if (c == '`') {
            pushSymbol(c);
            if (checkSymbols("`")) {
                state = State.NORMAL;
                clearSymbolBuffer();
            }
        } else {
            // Preserve inline code content for TTS (e.g., `ListView` -> "ListView")
            out.append(c);
            lastChar = c;
        }
    }

    private void handleCodeBlock(char c, StringBuilder out) {
        if (c == '`') {
            pushSymbol(c);
            if (checkSymbols("```")) {
                state = State.NORMAL;
                clearSymbolBuffer();
                inCodeBlockFirstLine = false;
                
                // Optimization: Code content is silent, but we add a space to avoid text merging.
                // Example: "Check this```code```now" -> "Check this now"
                out.append(' ');
                lastChar = ' ';
            }
        } else if (inCodeBlockFirstLine) {
            // First line after ``` opening: language identifier (e.g., "python", "java")
            state = State.CODE_BLOCK_LANG;
            codeBlockLangBuffer.setLength(0);
            deferredChar = c;
        }
        // Code content is completely silenced (no output)
    }
    
    private void handleCodeBlockLang(char c, StringBuilder out) {
        if (c == '\n') {
            // End of language identifier line, back to code block
            state = State.CODE_BLOCK;
            inCodeBlockFirstLine = false;
            codeBlockLangBuffer.setLength(0);
        }
        // Language identifier is silenced (no output)
    }
    
    // --- Pending State Handlers ---
    
    private void handlePendingTilde(char c, StringBuilder out) {
        if (c == '~') {
            pushSymbol(c);
            if (checkSymbols("~~")) {
                state = State.STRIKE;
                clearSymbolBuffer();
            }
            return;
        }
        
        // Not a tilde, check if we have enough tildes buffered
        // Note: single '~' is not strikethrough, output it.
        if (symbolBufferSize > 0) {
            for (int i = 0; i < symbolBufferSize; i++) {
                out.append(symbolBuffer[i]);
            }
            if (symbolBufferSize > 0) lastChar = symbolBuffer[symbolBufferSize - 1];
            clearSymbolBuffer();
        }
        
        state = State.NORMAL;
        deferredChar = c; // Process non-tilde char in NORMAL
    }

    private void handlePendingBacktick(char c, StringBuilder out) {
        if (c == '`') {
            pushSymbol(c);
            // Keep accumulating backticks
            return;
        }

        // Non-backtick char received. Decision time.
        if (checkSymbols("```")) {
            // 3 or more backticks -> CODE BLOCK
            state = State.CODE_BLOCK;
            clearSymbolBuffer();
            
            // If fence started at line start, we treat next content as language identifier
            // (unless newline immediately, which means empty language)
            inCodeBlockFirstLine = pendingBacktickAtLineStart && c != '\n';
            
            // Optimization: Code content is silent, but we add a space to avoid text merging.
            if (out.length() > 0 && out.charAt(out.length() - 1) != ' ') {
                out.append(' ');
                lastChar = ' ';
            }
            
            // Re-process current char in CODE_BLOCK state
            deferredChar = c;
            
        } else {
            // 1 or 2 backticks -> CODE SPAN (Inline code)
            state = State.CODE_SPAN;
            clearSymbolBuffer();
            
            // Re-process current char in CODE_SPAN state
            deferredChar = c;
        }
    }

    private void handleMaybeImageStart(char c, StringBuilder out) {
        if (c == '[') {
            state = State.IMAGE_ALT;
        } else {
            out.append('!');
            lastChar = '!';
            state = State.NORMAL;
            deferredChar = c; // Defer processing instead of recursion
        }
    }

    private void handleMaybeFootnoteStart(char c, StringBuilder out) {
        if (c == '^') {
            state = State.FOOTNOTE;
        } else {
            // It was a link text start
            state = State.LINK_TEXT;
            linkTextBuffer.setLength(0);
            deferredChar = c; // Defer to process via handleLinkText
        }
    }

    private void handleMaybeLinkClose(char c, StringBuilder out) {
        if (c == '(') {
            state = State.LINK_URL;
        } else {
            // Not a link, output buffered text
            out.append('[').append(linkTextBuffer).append(']');
            lastChar = ']';
            linkTextBuffer.setLength(0);
            state = State.NORMAL;
            deferredChar = c; // Defer processing
        }
    }

    private void handleMaybeImageAltClose(char c, StringBuilder out) {
        if (c == '(') {
            state = State.IMAGE_URL;
        } else {
            // Not an image, output '!' (we silenced alt text)
            out.append('!');
            lastChar = '!';
            state = State.NORMAL;
            deferredChar = c; // Defer processing
        }
    }

    private void handleMaybeTaskListHyphen(char c, StringBuilder out) {
        if (c == ' ') {
            state = State.MAYBE_TASK_LIST_SPACE;
        } else {
            // Was just a hyphen at line start (unordered list marker), ignore it
            // Continue to NORMAL state and process next character
            state = State.NORMAL;
            deferredChar = c; // Defer processing
        }
    }

    private void handleMaybeTaskListSpace(char c, StringBuilder out) {
        if (c == '[') {
            state = State.TASK_LIST_MARKER;
        } else {
            // Was just hyphen+space (unordered list marker)
            // Ignore the hyphen and space, but continue processing the content
            state = State.NORMAL;
            deferredChar = c; // Process this character as content
        }
    }

    private void handlePendingEmphasis(char c, StringBuilder out) {
        char marker = pendingEmphasisChar;
        
        if (c == marker) {
            // Accumulate consecutive markers (e.g., ** or ___)
            pushSymbol(marker); // Push the previously pending marker
            // Keep state PENDING, marker unchanged, to wait for next char
            return;
        }
        
        // c is different from marker. Check if it's a valid emphasis start.
        boolean validStart = !Character.isWhitespace(c);
        
        if (validStart) {
             // Valid emphasis start confirmed
             pushSymbol(marker); // Push the pending marker(s)
             
             // Determine emphasis type based on accumulated markers
             if (marker == '*') {
                if (checkSymbols("**")) {
                    state = State.EMPHASIS_STAR2;
                    clearSymbolBuffer();
                } else { // Single *
                    state = State.EMPHASIS_STAR1;
                    clearSymbolBuffer();
                }
             } else { // '_'
                if (checkSymbols("__")) {
                    state = State.EMPHASIS_UND2;
                    clearSymbolBuffer();
                } else {
                    state = State.EMPHASIS_UND1;
                    clearSymbolBuffer();
                }
             }
             
             // Defer processing of content character
             deferredChar = c;
        } else {
             // Invalid start (whitespace after marker). Output all accumulated markers as plain text.
             // First output any markers in symbolBuffer
             for (int i = 0; i < symbolBufferSize; i++) {
                 out.append(symbolBuffer[i]);
             }
             clearSymbolBuffer();
             // Then output the pending marker
             out.append(marker);
             lastChar = marker;
             
             state = State.NORMAL;
             deferredChar = c; // Defer processing of whitespace
        }
    }


    private void handleEmphasisStar1(char c, StringBuilder out) {
        if (c == '*') {
            pushSymbol(c);
            if (checkSymbols("*")) {
                if (isValidEmphasisEnd('*')) {
                    state = State.NORMAL;
                    clearSymbolBuffer();
                    return;
                }
                // Not a valid end, output the * as content
                out.append(symbolBuffer[0]);
                lastChar = symbolBuffer[0];
                clearSymbolBuffer();
                return;
            }
            // Symbol not ready yet, wait (don't output)
            return;
        }
        // Not a *, check if we have pending * symbols
        if (symbolBufferSize > 0) {
            // Output any accumulated * as content
            for (int i = 0; i < symbolBufferSize; i++) {
                out.append(symbolBuffer[i]);
            }
            clearSymbolBuffer();
        }
        out.append(c);
        lastChar = c;
    }

    private void handleEmphasisStar2(char c, StringBuilder out) {
        if (c == '*') {
            pushSymbol(c);
            if (checkSymbols("**")) {
                if (isValidEmphasisEnd('*')) {
                    state = State.NORMAL;
                    clearSymbolBuffer();
                    return;
                }
                // Not a valid end, output accumulated * symbols as content
                for (int i = 0; i < symbolBufferSize; i++) {
                    out.append(symbolBuffer[i]);
                    lastChar = symbolBuffer[i];
                }
                clearSymbolBuffer();
                return;
            }
            // Only one * so far, wait for next character (don't output yet)
            return;
        }
        // Not a *, check if we have pending * symbols
        if (symbolBufferSize > 0) {
            // Output any accumulated * as content
            for (int i = 0; i < symbolBufferSize; i++) {
                out.append(symbolBuffer[i]);
            }
            clearSymbolBuffer();
        }
        out.append(c);
        lastChar = c;
    }

    private void handleEmphasisUnd1(char c, StringBuilder out) {
        if (c == '_') {
            pushSymbol(c);
            if (checkSymbols("_")) {
                if (isValidEmphasisEnd('_')) {
                    state = State.NORMAL;
                    clearSymbolBuffer();
                    return;
                }
                // Not a valid end, output the _ as content
                out.append(symbolBuffer[0]);
                lastChar = symbolBuffer[0];
                clearSymbolBuffer();
                return;
            }
            // Symbol not ready yet, wait (don't output)
            return;
        }
        // Not a _, check if we have pending _ symbols
        if (symbolBufferSize > 0) {
            // Output any accumulated _ as content
            for (int i = 0; i < symbolBufferSize; i++) {
                out.append(symbolBuffer[i]);
            }
            clearSymbolBuffer();
        }
        out.append(c);
        lastChar = c;
    }

    private void handleEmphasisUnd2(char c, StringBuilder out) {
        if (c == '_') {
            pushSymbol(c);
            if (checkSymbols("__")) {
                if (isValidEmphasisEnd('_')) {
                    state = State.NORMAL;
                    clearSymbolBuffer();
                    return;
                }
                // Not a valid end, output accumulated _ symbols as content
                for (int i = 0; i < symbolBufferSize; i++) {
                    out.append(symbolBuffer[i]);
                    lastChar = symbolBuffer[i];
                }
                clearSymbolBuffer();
                return;
            }
            // Only one _ so far, wait for next character (don't output yet)
            return;
        }
        // Not a _, check if we have pending _ symbols
        if (symbolBufferSize > 0) {
            // Output any accumulated _ as content
            for (int i = 0; i < symbolBufferSize; i++) {
                out.append(symbolBuffer[i]);
            }
            clearSymbolBuffer();
        }
        out.append(c);
        lastChar = c;
    }

    private void handleStrike(char c, StringBuilder out) {
        if (c == '~') {
            pushSymbol(c);
            if (checkSymbols("~~")) {
                state = State.NORMAL;
                clearSymbolBuffer();
                return;
            }
        }
        out.append(c);
        lastChar = c;
    }

    private void handleLinkText(char c, StringBuilder out) {
        if (c == ']') {
            state = State.MAYBE_LINK_CLOSE;
        } else {
            linkTextBuffer.append(c);
        }
    }

    private void handleLinkUrl(char c, StringBuilder out) {
        if (c == ')') {
            // Link ends, output cached link text
            out.append(linkTextBuffer);
            if (linkTextBuffer.length() > 0) {
                lastChar = linkTextBuffer.charAt(linkTextBuffer.length() - 1);
            }
            linkTextBuffer.setLength(0);
            state = State.NORMAL;
        }
        // Ignore all characters inside the URL
    }

    private void handleImageAlt(char c, StringBuilder out) {
        if (c == ']') {
            state = State.MAYBE_IMAGE_ALT_CLOSE;
        }
        // Ignore alt text
    }

    private void handleImageUrl(char c, StringBuilder out) {
        if (c == ')') {
            state = State.NORMAL;
        }
        // Ignore URL
    }

    private void handleHtmlTag(char c, StringBuilder out) {
        if (c == '>') {
            // Optimization: HTML tags are silent for TTS.
            // We append a space to ensure words separated by tags (like <br>) don't merge.
            // Example: "Hello<br>World" -> "Hello World"
            out.append(' ');
            
            htmlTagBuffer.setLength(0);
            state = State.NORMAL;
        } else {
            // Buffer tag name (content ignored, just waiting for closing '>')
            htmlTagBuffer.append(c);
        }
    }

    private void handleFootnote(char c, StringBuilder out) {
        if (c == ']') {
            // Optimization: Footnote references like [^1] are silent for TTS.
            // Reading "Note 1" interrupts the flow of speech.
            footnoteBuffer.setLength(0);
            state = State.NORMAL;
        } else {
            // Buffer footnote reference (skip the '^')
            // Content ignored, just waiting for closing ']'
            if (c != '^') {
                footnoteBuffer.append(c);
            }
        }
    }

    private void handleTaskListMarker(char c, StringBuilder out) {
        // In this state, we are inside - [ ] or - [x]
        // Wait for ']' to end, all characters in between are completely silenced
        if (c == ']') {
            state = State.NORMAL;
        }
        // No output for any characters (including 'x', ' ', etc.)
    }

    private void handleHeading(char c, StringBuilder out) {
        // In HEADING state, we ignore all # characters and whitespace.
        // The first non-#/whitespace char ends the heading markers.
        if (c == '#') {
            // Continue skipping additional # characters
            return;
        } else if (Character.isWhitespace(c)) {
            // Space or other whitespace after #, continue skipping until actual content
            // NOTE: Markdown usually requires a space "## Title", but "##Title" is sometimes rendered too.
            // We'll skip consecutive spaces "##    Title" -> "Title"
            return;
        } else {
            // Non-space, non-# character: heading marker zone ended, this is real content
            state = State.NORMAL;
            // Process this character in NORMAL state
            deferredChar = c;
        }
    }

    // ---------- Helper Methods: Emphasis Boundary Detection ----------
    // isValidEmphasisStart REMOVED (Replaced by State Logic)
    
    private boolean isValidEmphasisEnd(char marker) {
        // Last output character must not be whitespace, and not the same marker (double markers merged)
        if (lastChar == 0 || Character.isWhitespace(lastChar)) {
            return false;
        }
        return true;
    }

    // ---------- Symbol Buffer (for detecting consecutive symbols like **, ```, ~~) ----------
    private void pushSymbol(char c) {
        if (symbolBufferSize < symbolBuffer.length) {
            symbolBuffer[symbolBufferSize++] = c;
        } else {
            // Shift left and add new
            System.arraycopy(symbolBuffer, 1, symbolBuffer, 0, symbolBuffer.length - 1);
            symbolBuffer[symbolBuffer.length - 1] = c;
        }
    }

    private boolean checkSymbols(String target) {
        int len = target.length();
        if (symbolBufferSize < len) return false;
        
        // Check last N characters in buffer
        int start = symbolBufferSize - len;
        for (int i = 0; i < len; i++) {
            if (symbolBuffer[start + i] != target.charAt(i)) {
                return false;
            }
        }
        return true;
    }

    private void clearSymbolBuffer() {
        symbolBufferSize = 0;
    }
}
