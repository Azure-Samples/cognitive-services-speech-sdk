//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CustomVoice.TtsLib.TtsUtil;

using Flurl.Http;
using System;
using System.Text;
using System.Threading.Tasks;

public class ExceptionHelper
{
    public static async Task<TResponse> PrintHandleExceptionAsync<TResponse>(Func<Task<TResponse>> requestAsyncFunc)
    {
        ArgumentNullException.ThrowIfNull(requestAsyncFunc);

        try
        {
            return await requestAsyncFunc().ConfigureAwait(false);
        }
        catch (FlurlHttpTimeoutException e)
        {
            Console.WriteLine($"Timeout with error: {e.Message}");
            throw;
        }
        catch (FlurlHttpException ex)
        {
            var error = $"{nameof(FlurlHttpException)}: {await ex.GetResponseStringAsync()}";
            Console.WriteLine(error);
            throw;
        }
        catch (Exception ex)
        {
            Console.WriteLine($"{ex.GetType().Name}: {ex.Message}");
            throw;
        }
    }

    public static string BuildExceptionMessage(Exception exception)
    {
        return BuildExceptionMessage(exception, false);
    }

    public static async Task<(bool success, string error, T result)> HasRunWithoutExceptionAsync<T>(Func<Task<T>> func, bool isAppendStackTrace = false)
    {
        if (func == null)
        {
            throw new ArgumentNullException(nameof(func));
        }

        try
        {
            var result = await func().ConfigureAwait(false);
            return (true, null, result);
        }
        catch (Exception e)
        {
            return (false, $"Failed to run function with exception: {BuildExceptionMessage(e, isAppendStackTrace)}", default(T));
        }
    }

    public static async Task<(bool success, string error)> HasRunWithoutExceptionAsync(Func<Task> func, bool isAppendStackTrace = false)
    {
        if (func == null)
        {
            throw new ArgumentNullException(nameof(func));
        }

        try
        {
            await func().ConfigureAwait(false);
            return (true, null);
        }
        catch (Exception e)
        {
            return (false, $"Failed to run function with exception: {BuildExceptionMessage(e, isAppendStackTrace)}");
        }
    }

    public static string BuildExceptionMessage(Exception exception, bool isAppendStackTrace)
    {
        if (exception == null)
        {
            throw new ArgumentNullException(nameof(exception));
        }

        var messageBuilder = new StringBuilder();
        for (Exception current = exception; current != null; current = current.InnerException)
        {
            if (current.InnerException != null)
            {
                messageBuilder.AppendLine(current.Message);
            }
            else
            {
                messageBuilder.Append(current.Message);
            }
        }

        if (isAppendStackTrace)
        {
            messageBuilder.Append(exception.StackTrace);
        }

        return messageBuilder.ToString();
    }

#pragma warning disable CA1031
    public static (bool success, string error) HasRunWithoutException(Func<(bool success, string error)> func, bool isAppendStackTrace = false)
    {
        if (func == null)
        {
            throw new ArgumentNullException(nameof(func));
        }

        try
        {
            return func();
        }
        catch (Exception e)
        {
            return (false, BuildExceptionMessage(e, isAppendStackTrace));
        }
    }

    public static (bool success, string error) HasRunWithoutException(Action action, bool isAppendStackTrace = false)
    {
        if (action == null)
        {
            throw new ArgumentNullException(nameof(action));
        }

        try
        {
            action();
            return (true, null);
        }
        catch (Exception e)
        {
            return (false, BuildExceptionMessage(e, isAppendStackTrace));
        }
    }

    public static (bool success, string error) HasRunWithoutExceptoin(Func<(bool success, string error)> func, bool isAppendStackTrace = false)
    {
        if (func == null)
        {
            throw new ArgumentNullException(nameof(func));
        }

        try
        {
            return func();
        }
        catch (Exception e)
        {
            return (false, ExceptionHelper.BuildExceptionMessage(e, isAppendStackTrace));
        }
    }

    public static (bool success, string error) HasRunWithoutExceptoin(Action action, bool isAppendStackTrace = false)
    {
        if (action == null)
        {
            throw new ArgumentNullException(nameof(action));
        }

        try
        {
            action();
            return (true, null);
        }
        catch (Exception e)
        {
            return (false, ExceptionHelper.BuildExceptionMessage(e, isAppendStackTrace));
        }
    }

    public static async Task<(bool success, string error)> HasRunWithoutExceptoinAsync(Func<Task> func, bool isAppendStackTrace = false)
    {
        if (func == null)
        {
            throw new ArgumentNullException(nameof(func));
        }

        try
        {
            await func().ConfigureAwait(false);
            return (true, null);
        }
        catch (Exception e)
        {
            return (false, $"Failed to run function with exception: {BuildExceptionMessage(e, isAppendStackTrace)}");
        }
    }

    public static async Task<(bool success, string error, T result)> HasRunWithoutExceptoinAsync<T>(Func<Task<T>> func, bool isAppendStackTrace = false)
    {
        if (func == null)
        {
            throw new ArgumentNullException(nameof(func));
        }

        try
        {
            var result = await func().ConfigureAwait(false);
            return (true, null, result);
        }
        catch (Exception e)
        {
            return (false, $"Failed to run function with exception: {ExceptionHelper.BuildExceptionMessage(e, isAppendStackTrace)}", default(T));
        }
    }

    public static (bool success, string error, T result) HasRunWithoutExceptoin<T>(Func<T> func, bool isAppendStackTrace = false)
    {
        if (func == null)
        {
            throw new ArgumentNullException(nameof(func));
        }

        try
        {
            var result = func();
            return (true, null, result);
        }
        catch (Exception e)
        {
            return (false, $"Failed to run function with exception: {ExceptionHelper.BuildExceptionMessage(e, isAppendStackTrace)}", default(T));
        }
    }
}
