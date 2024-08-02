using Avatar.Models;

namespace Avatar.Services
{
    public static class GlobalVariables
    {
        public static Dictionary<Guid, ClientContext> ClientContexts { get; } = new();
        public static string? SpeechToken { get; set; }
        public static string? IceToken { get; set; }

    }

}
