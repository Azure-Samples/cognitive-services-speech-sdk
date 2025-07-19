# Voice Live Agent Template (Python + ACS)

Lightweight template to test real-time voice calls using **Azure Communication Services (ACS)** Call Automation + **Azure Voice Live API** — no PSTN number needed. Start locally with `uv run`, deploy later to Azure Web App.

---

## Prerequisites

- [Azure CLI](https://learn.microsoft.com/en-us/cli/azure/install-azure-cli)
- [Bicep CLI](https://learn.microsoft.com/en-us/azure/azure-resource-manager/bicep/install)
- [azd CLI (Azure Developer CLI)](https://learn.microsoft.com/en-us/azure/developer/azure-developer-cli/install-azd)

---

## Project Structure
voice-live-agent/
├── server/
│ ├── server.py               # Entrypoint – handles triggers from clients
│ ├── voice_live_service.py # Azure Voice Live API logic (session, messaging)
│ ├── media_handler.py      # Router between streaming clients and AI service
│ ├── acs_media_streaming_handler.py  # Handles ACS WebSocket streaming
│ └── helper.py             # Utility functions for JSON parsing and data extraction
├── client/
│ ├── index.html # Mic capture UI
└── .env # local test

## Deployment

Deploy this accelerator using the provided infrastructure-as-code (Bicep) templates.  
The **Azure Developer CLI (`azd`)** is the recommended method, offering simple authentication, environment setup, and resource provisioning.

---

### Step 1: Authenticate

```bash
azd auth login
```

### Step 2: Create a New Environment

```bash
azd auth new
```
You’ll be prompted to choose:

- Azure Subscription
- Azure Region (use swedencentral for best compatibility)
- Environment Name (e.g. dev, test, prod)

###  Step 3: Customize Your Deployment (Optional)
```bash
    azd env set SPEECH_PROVIDER <option>
    azd env set AZURE_SPEECH_LANG <locale(s)>
```

| Parameter           | Default              | Options / Description                                                                                                                                                                                                                 |
|---------------------|----------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `SPEECH_PROVIDER`   | `azure-ai-speech`    | Choose the speech-to-text provider:`azure-ai-speech` or `azure-openai-gpt4o-transcribe`.   |
| `AZURE_SPEECH_LANG` | `en-US`              | Specify one or more supported locales (comma-separated, e.g. `en-US,nl-NL`). See the [full list of supported languages](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/language-support?tabs=stt). When multiple locales are set, automatic language identification is enabled. |

###  Step 4: Deploy resources with:

    ```bash
    azd up
    ```

During deployment, you’ll be prompted for:

    | Parameter           | Description                                                                  |
    |---------------------|------------------------------------------------------------------------------|
    | Azure Subscription  | The Azure subscription for resource deployment.                              |
    | Azure Location      | The Azure region for resources                                               |
    | Environment Name    | A unique environment name (used as a prefix for resource names).             |

    For best compatibility, use `swedencentral` as your Azure region. Other regions may not be fully supported or tested.

After deployment, the CLI will display a link to your web service. Open it in your browser, you should see `{"status": "healthy"}` to confirm the service is running.

## Notes
- Region: swedencentral is strongly recommended due to AI Foundry availability.

- Post-Deployment: You can separately deploy ACS Event Grid subscription or configure additional integrations.

## Clean up resources

When you no longer need the resources created in this article, run the following command to power down the app:

```bash
azd down
```

If you want to redeploy to a different region, delete the `.azure` directory before running `azd up` again. In a more advanced scenario, you could selectively edit files within the `.azure` directory to change the region.
