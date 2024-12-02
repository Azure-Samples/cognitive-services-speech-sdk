function formatControlMessage(message) {
  let html = "";
  try {
    const data = JSON.parse(message);
    const timestamp = formatTimestamp(new Date());
    const messageType = data.type || "unknown";
    const text = data.text || "";
    const reason = data.reason || "";
    const latency = data.latency || ""; // Assume latency is in milliseconds

    switch (messageType) {
      case "recognized":
        html = `<div class="message recognized">
                  <div class="message-header">
                    <span class="role">You</span>
                    <span class="timestamp">${timestamp}</span>
                    <span class="message-content system-message"> - Latency: <strong class="latency">${
          latency ? `${latency} ms` : "N/A"
        }</strong>.</span>
                  </div>
                  <span class="message-content user-message">${text}</span>

                </div>`;
        break;

      case "ai response":
        html = `<div class="message ai-response">
                  <div class="message-header">
                    <span class="role">AI</span>
                    <span class="timestamp">${timestamp}</span>
                    <span class="message-content system-message"> - Latency: <strong class="latency">${
          latency ? `${latency} ms` : "N/A"
        }</strong>.</span>
                  </div>
                  <span class="message-content ai-message">${text}</span>
                </div>`;
        break;

      case "thinking":
        html = `<div class="thinking">
                  <span class="message-content system-message">AI is thinking...</span>
                </div>`;
        break;

      case "stream_start":
        html = `<div class="stream-start">
                  <span class="message-content system-message">Streaming started${
                    reason ? ` (${reason})` : ""
                  } - Latency: <strong class="latency">${
          latency ? `${latency} ms` : "N/A"
        }</strong>.</span>
                </div>`;
        break;

      case "stream_stop":
        html = `<div class="stream-stop">
                  <span class="message-content system-message">Streaming stopped${
                    reason ? ` (${reason})` : ""
                  }</span>
                </div>`;
        break;

      default:
        html = `<div class="message unknown">
                  <div class="message-header">
                    <span class="role">Unknown message type</span>
                    <span class="timestamp">${timestamp}</span>
                    <span class="message">${message}</span>
                  </div>
                  <span class="message-content system-message">Unknown message type</span>
                </div>`;
        break;
    }
  } catch (e) {
    html = `<div class="message error">
              <div class="message-header">
                <span class="role">Error</span>
                <span class="timestamp">${timestamp}</span>
              </div>
              <span class="message-content system-message">Failed to parse message: ${message}</span>
            </div>`;
  }
  return html;
}

function updateControlMessagesLog(messages) {
  const controlMessagesLog = document.getElementById("controlMessagesLog");
  messages.forEach((msg) => {
    controlMessagesLog.innerHTML += formatControlMessage(msg);
  });

  controlMessagesLog.scrollTop = controlMessagesLog.scrollHeight;
}

/**
 * Converts a Date object or a timestamp to "YYYY-MM-DD HH:mm:ss.SSS" format.
 * @param {Date|string|number} date - The date object, timestamp, or date string to format.
 * @returns {string} - The formatted date string.
 */
function formatTimestamp(date) {
  // Convert the input to a Date object if it's not already
  const d = new Date(date);
  if (isNaN(d.getTime())) {
    // Return an error or handle invalid date input
    console.error("Invalid date provided");
    return "Invalid Date";
  }

  // Extract parts of the date
  const year = d.getFullYear();
  const month = String(d.getMonth() + 1).padStart(2, "0"); // Months are 0-based
  const day = String(d.getDate()).padStart(2, "0");
  const hours = String(d.getHours()).padStart(2, "0");
  const minutes = String(d.getMinutes()).padStart(2, "0");
  const seconds = String(d.getSeconds()).padStart(2, "0");
  const milliseconds = String(d.getMilliseconds()).padStart(3, "0");

  // Return the formatted date string
  return `${year}-${month}-${day} ${hours}:${minutes}:${seconds}.${milliseconds}`;
}

// Example usage
// const timestamp = new Date(); // or use a specific timestamp
// console.log(formatTimestamp(timestamp)); // Outputs: "2024-07-30 07:16:10.349" or current date/time

// updateControlMessagesLog([
//   `{"type": "recognized", "text": "Ha ha ha.", "time": "2024-07-30 07:16:06.938"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:06.943"}`,
//   `{"type": "ai response", "text": "What's making you laugh, Rob? Care to share a joke or something funny? I'm always up for a good laugh!", "time": "2024-07-30 07:16:07.303"}`,
//   `{ "type": "stream_start" }`,
//   `{"type": "stream_start", "reason": "TTS started", "time": "2024-07-30 07:16:08.063"}`,
//   `{"type": "recognized", "text": "Short hair.", "time": "2024-07-30 07:16:10.337"}`,
//   `{"type": "stream_stop", "reason": "interrupted"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "thinking", "time": "2024-07-30 07:16:10.349"}`,
//   `{"type": "ai response", "text": " Ah, I see! Short hair can be a funny topic for some people. It's amazing how a simple change in hairstyle can bring out a laugh. Thanks for sharing, Rob! Do you have any other jokes or funny stories you'd like to share?", "time": "2024-07-30 07:16:10.805"}`,
// ]);
