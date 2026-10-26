#!/usr/bin/env python3
"""Build a Microsoft Forms feedback URL for azure-tts-integrate; never submits.

The caller must obtain user consent and review minimized content before using
--consent-confirmed; this flag is an attestation, not authentication. The deny
check catches obvious sensitive text, not every secret, so content still needs
human review. build_url is a pure helper without consent checks or I/O.

Adapted from microsoft-foundry/voicelive-samples skills/azure-avatar-integrate
(MIT License).
"""

import argparse
import json
import os
import re
import sys
import webbrowser
from ipaddress import ip_address
from urllib.parse import quote, unquote, urlencode, urlsplit


FORM_URL = "https://forms.cloud.microsoft/Pages/ResponsePage.aspx"
FORM_ID = "v4j5cvGGr0GRqy180BHbR3sYjVYgsGZIm2YAvZlP3A5UMjZZNzVHNDJaM0NMSFc4TkVYUUM4OUZQNS4u"
TITLE_FIELD = "rc460127c64ee47f689a3394122808d8b"
BODY_FIELD = "r116644ff8d764d02a77cf526edfa9aed"
PATH_FIELD = "r32ab9b383659417eb82b006b5abf6471"
TYPE_FIELD = "r8a69c00a4c3043bd870f995584eec175"
MAX_TITLE_LENGTH = 200
MAX_BODY_LENGTH = 4_000
MAX_URL_LENGTH = 8_000

# Values must match the form's choice text exactly, or prefill is ignored.
PATH_VALUES = {
    "sdk": "Speech SDK",
    "rest": "REST",
    "text-stream": "Text input streaming",
    "batch": "Batch synthesis",
    "unsupported": "Unsupported request",
}
TYPE_VALUES = {
    "product": "Product",
    "docs": "Documentation",
    "sample": "Sample",
    "sdk": "SDK",
    "skill": "Skill",
    "unsupported": "Unsupported capability",
}

# Deliberately conservative: a 32-hex string may be a Speech key, and UUIDs may
# identify subscriptions or tenants. This is a deny check, not a secret scanner.
SENSITIVE_TEXT = re.compile(
    r"(?P<credential>\bbearer\s+\S+"
    r"|\b[\w-]*(?:api[ _-]?key|password|passwd|token|subscription[ _-]?key"
    r"|client[ _-]?secret|account[ _-]?key|shared[ _-]?access[ _-]?key"
    r"|shared[ _-]?access[ _-]?signature|secret[ _-]?access[ _-]?key)"
    r"[\"']?\s*[:=]\s*\S+|[?&]sig\s*=\s*\S+)"
    r"|(?P<connection_string>\b(?:DefaultEndpointsProtocol|AccountName"
    r"|EndpointSuffix|InstrumentationKey)\s*=\s*\S+"
    r"|\b(?:Server|Data Source|Host)\s*=[^;\r\n]+;"
    r"|\b(?:postgres(?:ql)?|mysql|mssql|mongodb(?:\+srv)?|rediss?)://\S+)"
    r"|(?P<private_key>-----BEGIN (?:RSA |EC |OPENSSH |DSA |ENCRYPTED )?PRIVATE KEY-----)"
    r"|(?P<token>\b(?:gh[pousr]_[A-Za-z0-9]{20,}"
    r"|github_pat_[A-Za-z0-9_]{20,}|sk-[A-Za-z0-9_-]{20,}"
    r"|(?:AKIA|ASIA)[A-Z0-9]{16}"
    r"|eyJ[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+)\b)"
    r"|(?P<email>[\w.+%-]+@(?:[\w-]+\.)+[\w-]{2,})"
    r"|(?P<local_path>\b[A-Z]:[\\/](?:Users|Documents and Settings)[\\/]"
    r"|/(?:home|Users)/[^\s/]+|/root(?:/|\b)|~[\\/])"
    r"|(?P<identifier>\b[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}\b"
    r"|\b(?:[0-9a-f]{32}|[0-9a-f]{64})\b)",
    re.IGNORECASE,
)
INTERNAL_SUFFIXES = (".local", ".internal", ".intranet", ".corp", ".lan", ".localhost", ".invalid")


def _sensitive_category(text):
    match = SENSITIVE_TEXT.search(text)
    if match:
        return (match.lastgroup or "sensitive information").replace("_", " ")
    for match in re.finditer(r"\b(?:https?|wss?)://[^\s<>\"`]+", text, re.IGNORECASE):
        try:
            address = urlsplit(match.group())
            hostname = unquote(address.hostname or "").rstrip(".").lower()
        except ValueError:
            return "unreviewable URL"
        if address.username is not None or address.password is not None:
            return "URL credential"
        try:
            if not ip_address(hostname).is_global:
                return "non-public IP address"
        except ValueError:
            if "." not in hostname or hostname.endswith(INTERNAL_SUFFIXES):
                return "internal URL"
    for match in re.finditer(
        r"(?<![\w.])(?:\d{1,3}\.){3}\d{1,3}(?![\w.])"
        r"|(?<![\w:])(?:[0-9a-f]{0,4}:){2,}[0-9a-f:.%]+(?![\w:])",
        text,
        re.IGNORECASE,
    ):
        try:
            if not ip_address(match.group()).is_global:
                return "non-public IP address"
        except ValueError:
            pass
    return None


def _required_text(payload, name, max_length):
    value = payload.get(name)
    if not isinstance(value, str) or not value.strip():
        raise ValueError(f"{name} must be a non-empty string")
    value = value.strip()
    if len(value) > max_length:
        raise ValueError(f"{name} exceeds {max_length} characters")
    return value


def build_url(payload):
    unexpected = set(payload) - {"title", "body", "integration_path", "feedback_type"}
    if unexpected:
        raise ValueError("unexpected fields: " + ", ".join(sorted(unexpected)))
    title = _required_text(payload, "title", MAX_TITLE_LENGTH)
    body = _required_text(payload, "body", MAX_BODY_LENGTH)
    category = _sensitive_category(f"{title}\n{body}")
    if category:
        raise ValueError(f"feedback may contain {category}; remove it and review the minimized content before sharing")
    integration_path = payload.get("integration_path")
    feedback_type = payload.get("feedback_type")
    if integration_path not in PATH_VALUES:
        raise ValueError("integration_path must be one of: " + ", ".join(PATH_VALUES))
    if feedback_type not in TYPE_VALUES:
        raise ValueError("feedback_type must be one of: " + ", ".join(TYPE_VALUES))
    query = urlencode(
        {
            "id": FORM_ID,
            TITLE_FIELD: title,
            BODY_FIELD: body,
            PATH_FIELD: json.dumps(PATH_VALUES[integration_path]),
            TYPE_FIELD: json.dumps(TYPE_VALUES[feedback_type]),
        },
        quote_via=quote,
    )
    url = f"{FORM_URL}?{query}"
    if len(url) > MAX_URL_LENGTH:
        raise ValueError(f"encoded form URL exceeds {MAX_URL_LENGTH} characters; shorten the feedback body")
    return url


def blank_url():
    return f"{FORM_URL}?{urlencode({'id': FORM_ID}, quote_via=quote)}"


def open_url(url):
    try:
        if sys.platform == "win32":
            os.startfile(url)
            return True
        return webbrowser.open(url, new=2)
    except (OSError, webbrowser.Error):
        return False


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--consent-confirmed", action="store_true",
                        help="attest to user consent and, for prefill, content minimization and review")
    parser.add_argument("--open", action="store_true",
                        help="open the browser without printing the URL; omit only for an explicit link request")
    parser.add_argument("--blank", action="store_true", help="use an unfilled form without reading standard input")
    args = parser.parse_args(argv)
    if not args.consent_confirmed:
        print("error: obtain user consent and review minimized content, then supply --consent-confirmed", file=sys.stderr)
        return 2
    try:
        if args.blank:
            url = blank_url()
        else:
            payload = json.load(sys.stdin)
            if not isinstance(payload, dict):
                raise ValueError("input must be a JSON object")
            url = build_url(payload)
        if args.open:
            if not open_url(url):
                print("error: browser open failed; request a link explicitly to display it", file=sys.stderr)
                return 1
            print("Browser open requested; page load and submission are unverified.")
        else:
            print(url)
    except (json.JSONDecodeError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
