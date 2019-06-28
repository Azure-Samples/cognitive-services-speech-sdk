//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using Windows.UI.Xaml.Controls;

namespace VirtualAssistantPreview
{
    class JsonTreeBuilder
    {
        public static TreeViewNode JsonToTreeNode(JArray jArray, string nodeName)
        {
            if (jArray == null)
                return null;

            var root = new TreeViewNode() { Content = new KeyValuePair<string, string>(nodeName, "{ }") };
            int index = 0;

            foreach (JToken token in jArray)
            {
                if (token.Type == JTokenType.Array)
                {
                    root.Children.Add(JsonToTreeNode((JArray)token, $"{nodeName}[{index++}]"));
                }
                else if (token.Type == JTokenType.Object)
                {
                    root.Children.Add(JsonToTreeNode((JObject)token, $"{nodeName}[{index++}]"));
                }
                else
                {
                    root.Children.Add(new TreeViewNode()
                    {
                        Content = new KeyValuePair<string, JToken>($"{nodeName}[{index++}]", token)
                    });
                }
            }

            return root;
        }

        public static TreeViewNode JsonToTreeNode(JObject obj, string nodeName)
        {
            if (obj == null)
                return null;

            var root = new TreeViewNode() { Content = new KeyValuePair<string, string>(nodeName, "") };

            foreach (KeyValuePair<string, JToken> pair in obj)
            {
                if (pair.Value.Type == JTokenType.Object)
                {
                    root.Children.Add(JsonToTreeNode((JObject)pair.Value, pair.Key));
                }
                else if (pair.Value.Type == JTokenType.Array)
                {
                    root.Children.Add(JsonToTreeNode((JArray)pair.Value, pair.Key));
                }
                else
                {
                    root.Children.Add(GetChild(pair));
                }
            }

            return root;
        }

        private static TreeViewNode GetChild(KeyValuePair<string, JToken> pair)
        {
            if (pair.Value == null)
                return null;

            TreeViewNode child = new TreeViewNode()
            {
                Content = pair
            };

            return child;
        }
    }
}
