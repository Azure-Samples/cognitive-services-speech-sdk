# Feedback

Use only after an explicit feedback request or accepted invitation. Integration errors and retries alone do not start this workflow.

## 1. Record issues

After consent to draft, collect relevant service, SDK, documentation, skill or unsupported-request issues from existing records and session evidence. Do not start background tracking during integration. Issue-free feedback needs no record.

1. Create or load `.azure-tts-integrate/<integration-id>.json` in the target project and tell the developer its location. Exclude it from Git locally (for example `.git/info/exclude`) or use another approved private location. Never upload or commit it.
2. Keep one record per integration:

| Field | Contents |
|---|---|
| Integration | Local ID, short non-sensitive goal, path, status and evidence sources |
| Invitation | Preserve the existing invitation and drafting-consent state; it does not authorize sharing |
| Issues | Stable issue ID, symptom, actions and verification history, status (`pending`, `in_progress`, `resolved`); mark untested results `unverified` |

| Event | Action |
|---|---|
| Issue update | Update the same issue ID; keep failed fixes and resolved history until closeout |
| Resume, retry or path change | Reload the same record; keep integration identity and consent state |
| Missing, invalid or deleted record | Report that history is unavailable; never recreate an uninvited record for the same integration |
| Storage failure | Report it; chat-only tracking is not a substitute |

Keep records minimal and redacted. Exclude keys, tokens, SAS URLs, connection strings, private hostnames or IP addresses, personal or customer data, proprietary details, raw audio and full logs.

## 2. Preserve consent

The [entry point](../SKILL.md#report-status) handles the one-time invitation. An accepted invitation or explicit feedback request permits drafting; proceed without another invitation. When a record exists, preserve that consent state. Never repeat an invitation after refusal, silence, resume or another report. Draft consent does not approve sharing.

## 3. Draft and review

After consent to draft:

1. Reload the record (if any) and summarize every issue, including resolved ones, into one draft. Without a record, summarize from this session only.
2. Separate facts from hypotheses; write `unknown` for missing details.
3. Show the draft for corrections. Agree on shortening rather than dropping issues.

**Title:** `Azure TTS integration feedback: <path> - <main issue>`, at most 200 characters.
**Body:** at most 4,000 characters; omit empty sections:

```text
Summary: <outcome>
Environment: <path, language/runtime, SDK version, relevant non-secret settings>
Issues (repeat, including resolved issues):
- ID / Status: <local issue ID and status>
- Observed / Impact: <symptom and effect>
- Actions / Verification: <attempts, results and evidence sources>
Remaining: <unresolved questions>
```

With no issues, use type `skill` and describe the completed validation; do not invent issues. For an unsupported capability, use path `unsupported` and do not present it as supported.

## 4. Share

1. Explain that a prefilled form sends its content to Microsoft Forms through the URL and may remain in browser history; offer a blank form instead.
2. Get explicit approval of the reviewed content before generating or opening a prefilled form. Changed content needs renewed approval. Draft consent, validation confirmation and silence are not sharing approval.
3. Run the [helper](../scripts/feedback_form_url.py) with the project's Python interpreter. Quote absolute paths, pass the JSON through standard input (never as arguments), and add `--consent-confirmed` only after approval.

| `integration_path` | Path |
|---|---|
| `sdk` | Speech SDK |
| `rest` | REST |
| `text-stream` | Text input streaming |
| `batch` | Batch synthesis |
| `unsupported` | Unsupported request |

`feedback_type`: `product`, `docs`, `sample`, `sdk`, `skill` or `unsupported`.

| Choice | Flags | Standard input |
|---|---|---|
| Open prefilled form | `--consent-confirmed --open` | JSON with only `title`, `body`, `integration_path`, `feedback_type` |
| Open blank form | `--consent-confirmed --blank --open` | none |
| Explicit link-only request | omit `--open` | as above; the URL then enters chat and tool logs |

| Failure | Recovery |
|---|---|
| URL over 8,000 characters | Shorten, get renewed approval, retry once |
| Sensitive-content rejection | Report the category, not the content; redact or offer a blank form; never bypass the check |
| Browser open failure | Report it without the URL; show a link only on explicit request |
| Python unavailable | Offer only the blank form link after consent |

The developer alone answers the form's submission-approval question and submits. Opening the form is not proof of submission; report submission only when the developer confirms it.

## 5. Clean up

| Stage | Action |
|---|---|
| Before inviting | Release owned runtime resources; remove task-created temporary scripts, test outputs, logs and audio |
| Paused or incomplete integration | Same cleanup; keep the record for resumption |
| Final closeout (feedback handed off, declined, cancelled or abandoned) | Delete the record, drafts, the `.azure-tts-integrate` directory if empty, and any Git exclusion you added |

Delete only files this task created; keep the application, deliverables and user-owned files. Report cleanup failures. File cleanup does not erase chat, tool logs or browser history.

Route product support requests to Microsoft support or documentation feedback, not this form.
