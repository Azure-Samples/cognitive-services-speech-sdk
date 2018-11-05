//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { PropertyCollection } from "../sdk/Exports";

export enum RecognitionMode {
    Interactive,
    Conversation,
    Dictation,
}

export enum SpeechResultFormat {
    Simple,
    Detailed,
}

export class RecognizerConfig {
    private recognitionMode: RecognitionMode = RecognitionMode.Interactive;
    private platformConfig: PlatformConfig;
    private recognitionActivityTimeout: number;
    private speechConfig: PropertyCollection;

    constructor(
        platformConfig: PlatformConfig,
        recognitionMode: RecognitionMode = RecognitionMode.Interactive,
        speechConfig: PropertyCollection) {
        this.platformConfig = platformConfig ? platformConfig : new PlatformConfig(new Context(null));
        this.recognitionMode = recognitionMode;
        this.recognitionActivityTimeout = recognitionMode === RecognitionMode.Interactive ? 8000 : 25000;
        this.speechConfig = speechConfig;
    }

    public get parameters(): PropertyCollection {
        return this.speechConfig;
    }

    public get RecognitionMode(): RecognitionMode {
        return this.recognitionMode;
    }

    public get PlatformConfig(): PlatformConfig {
        return this.platformConfig;
    }

    public get RecognitionActivityTimeout(): number {
        return this.recognitionActivityTimeout;
    }

    public get IsContinuousRecognition(): boolean {
        return this.recognitionMode !== RecognitionMode.Interactive;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class PlatformConfig {
    private context: Context;

    constructor(context: Context) {
        this.context = context;
    }

    public Serialize = (): string => {
        return JSON.stringify(this, (key: any, value: any): any => {
            if (value && typeof value === "object") {
                const replacement: any = {};
                for (const k in value) {
                    if (Object.hasOwnProperty.call(value, k)) {
                        replacement[k && k.charAt(0).toLowerCase() + k.substring(1)] = value[k];
                    }
                }
                return replacement;
            }
            return value;
        });
    }

    public get Context(): Context {
        return this.context;
    }

}

// tslint:disable-next-line:max-classes-per-file
export class Context {
    private system: System;
    private os: OS;

    constructor(os: OS) {
        this.system = new System();
        this.os = os;
    }

    public get System(): System {
        return this.system;
    }

    public get OS(): OS {
        return this.os;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class System {
    private name: string;
    private version: string;
    private build: string;
    private lang: string;
    constructor() {
        // Note: below will be patched for official builds.
        const SPEECHSDK_CLIENTSDK_VERSION = "1.1.0-alpha.0.1";

        this.name = "SpeechSDK";
        this.version = SPEECHSDK_CLIENTSDK_VERSION;
        this.build = "JavaScript";
        this.lang = "JavaScript";
    }
    public get Version(): string {
        // Controlled by sdk
        return this.version;
    }
    public get Lang(): string {
        // Controlled by sdk
        return this.lang;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class OS {

    private platform: string;
    private name: string;
    private version: string;

    constructor(platform: string, name: string, version: string) {
        this.platform = platform;
        this.name = name;
        this.version = version;
    }

    public get Platform(): string {
        return this.platform;
    }

    public get Name(): string {
        return this.name;
    }

    public get Version(): string {
        return this.version;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class Device {

    private manufacturer: string;
    private model: string;
    private version: string;

    constructor(manufacturer: string, model: string, version: string) {
        this.manufacturer = manufacturer;
        this.model = model;
        this.version = version;
    }

    public get Manufacturer(): string {
        return this.manufacturer;
    }

    public get Model(): string {
        return this.model;
    }

    public get Version(): string {
        return this.version;
    }
}
