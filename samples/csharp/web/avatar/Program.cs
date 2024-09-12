//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Avatar.Models;
using Avatar.Services;

var builder = WebApplication.CreateBuilder(args);

// Config ClientSettings
builder.Services.Configure<ClientSettings>(builder.Configuration.GetSection("ClientSettings"));

builder.Services.AddSingleton<ClientContext>();
builder.Services.AddSingleton<IClientService, ClientService>();

// Register HttpClient and services
builder.Services.AddHttpClient<IceTokenService>();
builder.Services.AddHttpClient<SpeechTokenService>();

// Register services with DI container
builder.Services.AddSingleton<IceTokenService>();
builder.Services.AddSingleton<SpeechTokenService>();

// Register the background service to manage token refresh
builder.Services.AddHostedService<TokenRefreshBackgroundService>();

// Add services to the container.
builder.Services.AddControllersWithViews();

var app = builder.Build();

// Configure the HTTP request pipeline.
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Home/Error");
    // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
    app.UseHsts();
}

app.UseHttpsRedirection();
app.UseStaticFiles();

app.UseRouting();

app.UseAuthorization();

app.MapControllerRoute(
    name: "default",
    pattern: "{controller=Home}/{action=Index}/{id?}");

app.Run();
