using Microsoft.AspNetCore.SignalR;
using Microsoft.Extensions.Options;
using samples.Hubs;
using System.Text.Json;

namespace samples;

public sealed class Program
{
    public static async Task Main(string[] args)
    {
        var builder = WebApplication.CreateBuilder(args);

        // Add services to the container.
        builder.Services.AddControllersWithViews();

        builder.Services.AddSignalR()
            .AddJsonProtocol(options =>
            {
                options.PayloadSerializerOptions.PropertyNamingPolicy = JsonNamingPolicy.CamelCase;
            });

        builder.Services.AddSingleton<IConfigureOptions<HubOptions<SpeechToTextHub>>, HubOptionsSetup<SpeechToTextHub>>();
        builder.Services.Configure<HubOptions<SpeechToTextHub>>(hubOptions =>
        {
            hubOptions.MaximumReceiveMessageSize = 50000000;
            hubOptions.EnableDetailedErrors = true;
            hubOptions.KeepAliveInterval = TimeSpan.FromSeconds(240);
            hubOptions.ClientTimeoutInterval = TimeSpan.FromSeconds(240);
        });

        builder.Services.AddSingleton<IConfigureOptions<HubOptions<TextToSpeechHub>>, HubOptionsSetup<TextToSpeechHub>>();
        builder.Services.Configure<HubOptions<TextToSpeechHub>>(hubOptions =>
        {
            hubOptions.EnableDetailedErrors = true;
            hubOptions.KeepAliveInterval = TimeSpan.FromSeconds(240);
            hubOptions.ClientTimeoutInterval = TimeSpan.FromSeconds(240);
        });

        var app = builder.Build();

        app.UseHttpsRedirection();

        app.UseStaticFiles();

        app.UseRouting();

        app.UseAuthorization();

        app.MapHub<TextToSpeechHub>("/text-to-speech");
        app.MapHub<SpeechToTextHub>("/speech-to-text");

        app.MapControllerRoute(
            name: "default",
            pattern: "{controller=Speech}/{action=SpeechToText}/{id?}");

        await app.RunAsync();
    }
}
