// <copyright file="TranscriptionToHtml.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Globalization;
    using System.Linq;
    using System.Text;

    public static class TranscriptionToHtml
    {
        public static string ToHtml(SpeechTranscript transcription, string filename)
        {
            if (transcription == null)
            {
                throw new ArgumentNullException(nameof(transcription));
            }

            var isMultiChannel = transcription?.RecognizedPhrases != null && transcription.RecognizedPhrases.Any(s => s != null && s.Channel != 0);

            var html = $@"<html lang='en'> <head> <meta http-equiv='content-type' content='text/html; charset=utf-8'> <title>{filename}</title>";
            html += @"<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css' integrity='sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh' crossorigin='anonymous'>";
            html += @"<style>a.inline-decoration { color:#000000 !important; text-decoration:none; } .utterance-playing { background-color: lightgray; opacity: 0.9; }</style>";
            html += @"<link href='https://amp.azure.net/libs/amp/2.3.3/skins/amp-default/azuremediaplayer.min.css' rel='stylesheet'>";
            html += @"<script src='https://amp.azure.net/libs/amp/2.3.3/azuremediaplayer.min.js'></script>";
            html += @"</head>";
            html += $@"<body> <h3>{filename}</h3> <div class='text-center'><audio id='azuremediaplayer2' class='azuremediaplayer amp-default-skin amp-big-play-centered' tabindex='0'></audio></div><br><br><div class='text-center'><img </div><br><table class='table'><thead><tr><td>Speaker</td>{(isMultiChannel ? "<td>Channel</td><td>Text</td><td>Text</td>" : "<td>Text</td>")}</tr></thead><tbody>";

            var htmlBuilder = new StringBuilder(html);

            if (transcription.RecognizedPhrases != null)
            {
                foreach (var recognizedPhrase in transcription.RecognizedPhrases.OrderBy(s => s.Offset))
                {
                    var sentence = recognizedPhrase.NBest.FirstOrDefault();

                    if (sentence == null)
                    {
                        continue;
                    }

                    htmlBuilder.Append("<tr>");
                    htmlBuilder.Append("<td>" + recognizedPhrase.Speaker + @"</td>");

                    var confidenceIndication = "<span";

                    if (sentence.Confidence <= .5)
                    {
                        confidenceIndication = "<span class='table-danger'";
                    }
                    else if (sentence.Confidence <= .7)
                    {
                        confidenceIndication = "<span class='table-warning'";
                    }

                    if (isMultiChannel)
                    {
                        htmlBuilder.Append(CultureInfo.InvariantCulture, $"<td>{recognizedPhrase.Channel}</td>");

                        if (recognizedPhrase.Channel % 2 == 1)
                        {
                            htmlBuilder.Append(CultureInfo.InvariantCulture, $"<td><a href='javascript:jumpTime({recognizedPhrase.Offset});' class='inline-decoration'></a></td>");
                            htmlBuilder.Append(CultureInfo.InvariantCulture, $"<td><a href='javascript:jumpTime({recognizedPhrase.Offset});' class='inline-decoration'{confidenceIndication}>{sentence.Display}</span></a></td>");
                        }
                        else
                        {
                            htmlBuilder.Append(CultureInfo.InvariantCulture, $"<td><a href='javascript:jumpTime({recognizedPhrase.Offset});' class='inline-decoration'{confidenceIndication}>{sentence.Display}</span></a></td>");
                            htmlBuilder.Append(CultureInfo.InvariantCulture, $"<td><a href='javascript:jumpTime({recognizedPhrase.Offset});' class='inline-decoration'></a></td>");
                        }
                    }
                    else
                    {
                        htmlBuilder.Append(CultureInfo.InvariantCulture, $"<td><a href='javascript:jumpTime({recognizedPhrase.Offset});' class='inline-decoration'{confidenceIndication}>{sentence.Display}</span></a></td>");
                    }

                    htmlBuilder.Append("</tr>");
                    htmlBuilder.AppendLine(string.Empty);
                }
            }

            htmlBuilder.Append(@"</tbody></table>");
            htmlBuilder.Append(@"<script> var myOptions = { controls: true, width: '450', height: '250', poster: 'http://img-prod-cms-rt-microsoft-com.akamaized.net/cms/api/am/imageFileData/RE2qVsJ?ver=3f74'  }; </script>");
            htmlBuilder.Append(@"</body></html>");
            return htmlBuilder.ToString();
        }
    }
}