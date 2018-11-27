// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { WebsocketConnection } from "../common.browser/Exports";
import { IConnection, IStringDictionary, Storage } from "../common/Exports";
import { PropertyId } from "../sdk/Exports";
import { AuthInfo, IConnectionFactory, RecognizerConfig, WebsocketMessageFormatter } from "./Exports";

const TestHooksParamName: string = "testhooks";
const ConnectionIdHeader: string = "X-ConnectionId";

export class IntentConnectionFactory implements IConnectionFactory {

    public create = (
        config: RecognizerConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection => {

        let endpoint: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_Endpoint);
        if (!endpoint) {
            const region: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_IntentRegion);

            endpoint = this.host() + Storage.local.getOrAdd("TranslationRelativeUri", "/speech/" + this.getSpeechRegionFromIntentRegion(region) + "/recognition/interactive/cognitiveservices/v1");
        }

        const queryParams: IStringDictionary<string> = {
            format: "simple",
            language: config.parameters.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage),
        };

        if (this.isDebugModeEnabled) {
            queryParams[TestHooksParamName] = "1";
        }

        const headers: IStringDictionary<string> = {};
        headers[authInfo.headerName] = authInfo.token;
        headers[ConnectionIdHeader] = connectionId;

        return new WebsocketConnection(endpoint, queryParams, headers, new WebsocketMessageFormatter(), connectionId);
    }

    private host(): string {
        return Storage.local.getOrAdd("Host", "wss://speech.platform.bing.com");
    }

    private get isDebugModeEnabled(): boolean {
        const value = Storage.local.getOrAdd("IsDebugModeEnabled", "false");
        return value.toLowerCase() === "true";
    }

    private getSpeechRegionFromIntentRegion(intentRegion: string): string {
        switch (intentRegion) {
            case "West US":
            case "US West":
            case "westus":
                return "uswest";
            case "West US 2":
            case "US West 2":
            case "westus2":
                return "uswest2";
            case "South Central US":
            case "US South Central":
            case "southcentralus":
                return "ussouthcentral";
            case "West Central US":
            case "US West Central":
            case "westcentralus":
                return "uswestcentral";
            case "East US":
            case "US East":
            case "eastus":
                return "useast";
            case "East US 2":
            case "US East 2":
            case "eastus2":
                return "useast2";
            case "West Europe":
            case "Europe West":
            case "westeurope":
                return "europewest";
            case "North Europe":
            case "Europe North":
            case "northeurope":
                return "europenorth";
            case "Brazil South":
            case "South Brazil":
            case "southbrazil":
                return "brazilsouth";
            case "Australia East":
            case "East Australia":
            case "eastaustralia":
                return "australiaeast";
            case "Southeast Asia":
            case "Asia Southeast":
            case "southeastasia":
                return "asiasoutheast";
            case "East Asia":
            case "Asia East":
            case "eastasia":
                return "asiaeast";
            default:
                return intentRegion;
        }
    }
}
