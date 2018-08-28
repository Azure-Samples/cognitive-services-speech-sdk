import { ArgumentNullError, Promise, PromiseHelper } from "../../common/Exports";
import { AuthInfo, IAuthentication } from "./IAuthentication";

const AuthHeader: string = "Ocp-Apim-Subscription-Key";

export class CognitiveSubscriptionKeyAuthentication implements IAuthentication {
    private authInfo: AuthInfo;

    constructor(subscriptionKey: string) {
        if (!subscriptionKey) {
            throw new ArgumentNullError("subscriptionKey");
        }

        this.authInfo = new AuthInfo(AuthHeader, subscriptionKey);
    }

    public Fetch = (authFetchEventId: string): Promise<AuthInfo> => {
        return PromiseHelper.FromResult(this.authInfo);
    }

    public FetchOnExpiry = (authFetchEventId: string): Promise<AuthInfo> => {
        return PromiseHelper.FromResult(this.authInfo);
    }
}
