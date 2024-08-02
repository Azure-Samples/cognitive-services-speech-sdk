using Avatar.Models;

namespace Avatar.Services
{
    public interface IClientService
    {
        Guid InitializeClient();
        ClientContext GetClientContext(Guid clientId);
    }

}
