import os
import requests
import zipfile
import io

# GitHub repo info
REPO_OWNER = "BinarywoodB" 
REPO_NAME = "cognitive-services-speech-sdk"
SAMPLE_FOLDER = "scenarios/python/console/speech_rewrite_sample"
BRANCH = "extension"

# GitHub API URL to download the folder as a zip
GITHUB_API_URL = f"https://api.github.com/repos/{REPO_OWNER}/{REPO_NAME}/contents/{SAMPLE_FOLDER}?ref={BRANCH}"

def download_sample():
    print(f"Downloading {SAMPLE_FOLDER} from {REPO_NAME}...")
    headers = {"Accept": "application/vnd.github.v3+json"}
    response = requests.get(GITHUB_API_URL, headers=headers)
    response.raise_for_status()

    # Parse the API response to get the file URLs
    files = response.json()
    os.makedirs(SAMPLE_FOLDER, exist_ok=True)

    for file in files:
        if file["type"] == "file":
            download_url = file["download_url"]
            print(f"Downloading {file['name']}...")
            file_response = requests.get(download_url)
            with open(os.path.join(SAMPLE_FOLDER, file["name"]), "wb") as f:
                f.write(file_response.content)

if __name__ == "__main__":
    download_sample()
    print(f"Sample folder {SAMPLE_FOLDER} has been downloaded.")
