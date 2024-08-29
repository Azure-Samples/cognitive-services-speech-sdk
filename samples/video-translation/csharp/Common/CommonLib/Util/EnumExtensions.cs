//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Util;

using System;
using System.Linq;
using System.Runtime.Serialization;

public static class EnumExtensions
{
    public static string AsString<TEnum>(this TEnum enumValue)
        where TEnum : Enum
    {
        if (typeof(TEnum).GetCustomAttributes(typeof(FlagsAttribute), false).Any())
        {
            return enumValue.ToString();
        }

        var enumMemberName = Enum.GetName(typeof(TEnum), enumValue);

        var enumMember = typeof(TEnum).GetMember(enumMemberName).Single();
        var jsonPropertyAttribute = enumMember
            .GetCustomAttributes(typeof(DataMemberAttribute), true)
            .Cast<DataMemberAttribute>()
            .SingleOrDefault();

        if (jsonPropertyAttribute != null)
        {
            return jsonPropertyAttribute.Name;
        }

        return enumMemberName;
    }

    public static TEnum AsEnumValue<TEnum>(this string value)
    {
        return value.AsEnumValue<TEnum>(false);
    }

    public static TEnum AsEnumValue<TEnum>(this string value, bool ignoreCase)
    {
        var enumMembers = typeof(TEnum).GetMembers();
        var membersAndAttributes = enumMembers
            .Select(m => (member: m, attribute: m.GetCustomAttributes(typeof(DataMemberAttribute), true).Cast<DataMemberAttribute>().SingleOrDefault()))
            .Where(m => m.attribute != null)
            .Where(m => m.attribute.Name == value);

        if (membersAndAttributes.Any())
        {
            value = membersAndAttributes.Single().member.Name;
        }

        return (TEnum)Enum.Parse(typeof(TEnum), value, ignoreCase);
    }
}