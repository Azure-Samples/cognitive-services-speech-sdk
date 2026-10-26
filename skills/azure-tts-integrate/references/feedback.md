# Feedback

Record integration issues when they happen and offer optional feedback once, after the integration is complete.

## 1. Record issues

Create a record only when an issue occurs during implementation work: a service error, an SDK or documentation mismatch, a skill instruction that was wrong or missing, or an unsupported request. Do not create one for recommendation-only requests or issue-free work.

1. Create or load `.azure-tts-integrate/<integration-id>.json` in the target project and tell the developer its location. Exclude it from Git locally (for example `.git/info/exclude`) or use another approved private location. Never upload or commit it.
2. Keep one record per integration:

| Field | Contents |
|---|---|
| Integration | Local ID, short non-sensitive goal, path, status and evidence sources |
| Invitation | `feedback_invited=false`, `feedback_response=not_asked` |
| Issues | Stable issue ID, symptom, actions and verification history, status (`pending`, `in_progress`, `resolved`); mark untested results `unverified` |

| Event | Action |
|---|---|
| Issue update | Update the same issue ID; keep failed fixes and resolved history until closeout |
| Resume, retry or path change | Reload the same record; keep integration identity and consent state |
| Missing, invalid or deleted record | Report that history is unavailable; never recreate an uninvited record for the same integration |
| Storage failure | Report it; chat-only tracking is not a substitute |

Keep records minimal and redacted. Exclude keys, tokens, SAS URLs, connection strings, private hostnames or IP addresses, personal or customer data, proprietary details, raw audio and full logs.

## 2. Invite once

Invite only when all of these hold:

- You reported **Integration complete** under the [status rules](../SKILL.md#report-status), or the developer explicitly asked to give feedback.
- Owned runtime resources are released and temporary files removed (the record stays).
- No earlier invitation exists for this integration (`feedback_invited=false` when a record exists).

Ask once, with the question tool or a numbered list:

> Share feedback about this Azure TTS integration? **Review feedback draft** / **Not now**.

When a record exists, save `feedback_invited=true` and `feedback_response=pending` before waiting, then `accepted` or `declined`. Never repeat the invitation after refusal, silence, resume or another report.

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
