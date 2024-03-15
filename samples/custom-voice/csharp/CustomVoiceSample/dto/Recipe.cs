//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public enum RecipeKind
{
    Default = 1,
    CrossLingual = 2,
    MultiStyle = 3
}

public class Recipe
{
    public string Version { get; set; }

    public RecipeKind Kind { get; set; }

    public string Description { get; set; }
}
