
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
    private language: string;
    private format: SpeechResultFormat;
    private speechConfig: SpeechConfig;
    private recognitionActivityTimeout: number;

    constructor(
        platformConfig: SpeechConfig,
        recognitionMode: RecognitionMode = RecognitionMode.Interactive,
        language: string = "en-us",
        format: SpeechResultFormat = SpeechResultFormat.Simple) {
        this.speechConfig = platformConfig ? platformConfig : new SpeechConfig(new Context(null, null));
        this.recognitionMode = recognitionMode;
        this.language = language;
        this.format = format;
        this.recognitionActivityTimeout = recognitionMode === RecognitionMode.Interactive ? 8000 : 25000;
    }

    public get RecognitionMode(): RecognitionMode {
        return this.recognitionMode;
    }

    public get Language(): string {
        return this.language;
    }

    public get Format(): SpeechResultFormat {
        return this.format;
    }

    public get SpeechConfig(): SpeechConfig {
        return this.speechConfig;
    }

    public get RecognitionActivityTimeout(): number {
        return this.recognitionActivityTimeout;
    }

    public get IsContinuousRecognition(): boolean {
        return this.recognitionMode !== RecognitionMode.Interactive;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechConfig {
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
    private device: Device;

    constructor(os: OS, device: Device) {
        this.system = new System();
        this.os = os;
        this.device = device;
    }

    public get System(): System {
        return this.system;
    }

    public get OS(): OS {
        return this.os;
    }

    public get Device(): Device {
        return this.device;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class System {
    private version: string;
    constructor() {
        // TODO: Tie this with the sdk Version somehow
        this.version = "1.0.00000";
    }
    public get Version(): string {
        // Controlled by sdk
        return this.version;
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
