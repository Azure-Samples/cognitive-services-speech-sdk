./scripts/load_python_env.ps1

$venvPythonPath = "./.venv/scripts/python.exe"
if (Test-Path -Path "/usr") {
  # fallback to Linux venv path
  $venvPythonPath = "./.venv/bin/python"
}

# Define the path to the Python script
$aiSearchSetupScriptPath = "scripts/setup_aisearch.py"

Start-Process -FilePath $venvPythonPath -ArgumentList $aiSearchSetupScriptPath -Wait -NoNewWindow


# Define the path to the Python script
$cosmosDbSetupScriptPath = "scripts/setup_cosmos.py"

Start-Process -FilePath $venvPythonPath -ArgumentList $cosmosDbSetupScriptPath -Wait -NoNewWindow