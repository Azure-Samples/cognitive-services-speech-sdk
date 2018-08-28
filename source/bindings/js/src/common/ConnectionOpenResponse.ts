
export class ConnectionOpenResponse {
    private statusCode: number;
    private reason: string;

    constructor(statusCode: number, reason: string) {
        this.statusCode = statusCode;
        this.reason = reason;
    }

    public get StatusCode(): number {
        return this.statusCode;
    }

    public get Reason(): string {
        return this.reason;
    }
}
