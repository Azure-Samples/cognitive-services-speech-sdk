import os
import sys
from azure.communication.callautomation import CallAutomationClient

def update_subscription(webhook_url: str):
    connection_string = os.getenv("ACS_CONNECTION_STRING")
    if not connection_string:
        raise ValueError("ACS_CONNECTION_STRING env var not set")

    client = CallAutomationClient.from_connection_string(connection_string)

    subs = client.communication_event_subscriptions.list()
    for sub in subs:
        print(f"Found subscription: id={sub.id}, webhook={sub.webhook_uri}")

        client.communication_event_subscriptions.update(
            event_subscription_id=sub.id,
            webhook_uri=webhook_url
        )
        print(f"Updated subscription {sub.id} to webhook {webhook_url}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: update_acs_subscription.py <webhook_url>")
        sys.exit(1)

    update_subscription(sys.argv[1])
