//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public enum ProjectKind
{
    ProfessionalVoice,
    PersonalVoice
}

public class Project
{
    public string Id { get; set; }

    public string Description { get; set; }

    public ProjectKind Kind { get; set; }

    /// <summary>
    /// The time-stamp when the object was created.
    /// The time stamp is encoded as ISO 8601 date and time format
    /// ("YYYY-MM-DDThh:mm:ssZ", see https://en.wikipedia.org/wiki/ISO_8601#Combined_date_and_time_representations).
    /// </summary>
    public DateTime CreatedDateTime { get; set; }
}
