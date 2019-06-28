//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.Bot.Schema;
using System;
using Windows.UI.Xaml.Data;

namespace VirtualAssistantPreview
{
    public enum Sender
    {
        Bot,
        User,
        Channel,
        Other
    }

    public class SenderFormatConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            Sender myEnumValue = (Sender)value;
            return myEnumValue.ToString();
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }

    public class MessageDisplay
    {
        public Sender From { get; set; }
        public string Message { get; set; }
        public MessageDisplay(string msg, Sender from) => (From, Message) = (from, msg);
        public override string ToString()
        {
            return $"{From}: {Message}";
        }
    }

    public class ActivityDisplay
    {
        public Sender From { get; set; }
        public IActivity Activity { get; set; }
        public string Json { get; set; }
        public DateTime Time { get; set; } = DateTime.Now;
        public ActivityDisplay(string json, IActivity activity, DateTime time) => (Json, Activity, Time) = (json, activity, time);
        public ActivityDisplay(string json, IActivity activity, Sender sender) => (Json, Activity, From) = (json, activity, sender);

        public string TypeSummary => Activity.Type == ActivityTypes.Event ? $"Event: {Activity.AsEventActivity().Name}" : Activity.Type;
        public override string ToString()
        {
            return $"{Time.ToString("MM/dd/yyyy HH:mm:ss.fff")}{Environment.NewLine}{Json}";
        }
    }
}
