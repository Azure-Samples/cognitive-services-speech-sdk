import { IConnection } from "../../common/Exports";
import { AuthInfo } from "./IAuthentication";
import { RecognizerConfig } from "./RecognizerConfig";

export interface IConnectionFactory {
    Create(
        config: RecognizerConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection;
}
