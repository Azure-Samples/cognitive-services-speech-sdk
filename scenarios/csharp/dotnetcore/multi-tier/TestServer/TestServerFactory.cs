
using Azure.AI.Test.Common.Logging;

namespace Azure.AI.Test.Common.Servers
{
    public static class TestServerFactory
    {
        public static TestServer CreateTestServer(Logging.ILogger logger, TestServerConfig serverConfig = null)
        {
            var config = serverConfig ?? new TestServerConfig();
            config.SetLogger(logger);

            TestServer server = null;
            try
            {
                server = new TestServer(config);
                logger.LogInfo("Test server listening at " + server.HttpEndpoint);
                return server;
            }
            catch (Exception)
            {
                server?.Dispose();
                throw;
            }
        }
    }
}
