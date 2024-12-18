using System;

namespace Microsoft.SpeechServices.CommonLib.Public.Interface;

public class ApimApiRegionConfig : IRegionConfig
{
    public ApimApiRegionConfig(string regionIdentifier)
    {
        ArgumentNullException.ThrowIfNull(regionIdentifier);
        this.RegionIdentifier = regionIdentifier;
    }

    public string RegionIdentifier { get; private set; }

    public Uri EndpointUrl => new Uri($"https://{HostName}");

    private string HostName => $"{this.RegionIdentifier}.api.cognitive.microsoft.com";
}
