// <copyright file="MustHaveOneElementAttribute.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Backend.VideoTranslationCoreEngine.Attributes;

using System.Collections;
using System.ComponentModel.DataAnnotations;
using System.Linq;

public sealed class MustHaveOneElementAttribute : ValidationAttribute
{
    public override bool IsValid(object value)
    {
        var list = value as IEnumerable;
        return list != null && list.Cast<object>().Any();
    }
}
