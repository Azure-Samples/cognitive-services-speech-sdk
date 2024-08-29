//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.CommandParser;

using System;

[AttributeUsage(AttributeTargets.Class, Inherited = false)]
public sealed class CommentAttribute : Attribute
{
    private readonly string headComment;
    private readonly string rearComment = "Copyright (C) Microsoft Corporation. All rights reserved.";

    public CommentAttribute(string headComment)
    {
        if (headComment == null)
        {
            throw new ArgumentNullException(nameof(headComment));
        }

        this.headComment = headComment;
    }

    public CommentAttribute(string headComment, string rearComment)
    {
        if (headComment == null)
        {
            throw new ArgumentNullException(nameof(headComment));
        }

        if (rearComment == null)
        {
            throw new ArgumentNullException(nameof(rearComment));
        }

        this.headComment = headComment;
        this.rearComment = rearComment;
    }

    public string HeadComment
    {
        get { return this.headComment; }
    }

    public string RearComment
    {
        get { return this.rearComment; }
    }
}
