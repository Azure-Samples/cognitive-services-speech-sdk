#!/bin/bash
set -e

echo "Updating ACS event subscription webhook URL..."

# Get your container app public URL via az CLI
CONTAINER_APP_URL=$(az containerapp show --name your-containerapp-name --resource-group your-rg-name --query properties.configuration.ingress.fqdn -o tsv)
WEBHOOK_URL="https://${CONTAINER_APP_URL}/acs/incomingcall"

echo "Container App URL detected: $WEBHOOK_URL"

# Export ACS connection string as env var
export ACS_CONNECTION_STRING=$(az keyvault secret show --vault-name your-keyvault-name --name 'ACS-CONNECTION-STRING' --query value -o tsv)

# Run python script that updates ACS subscription (make sure python and dependencies are installed)
python3 ./scripts/update_acs_subscription.py "$WEBHOOK_URL"
