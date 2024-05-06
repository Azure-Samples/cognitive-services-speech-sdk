//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchAvatarSample.dto;
using System;

public class BatchAvatarRequest
{
    public required string InputKind { get; set; }

    public required IList<BatchAvatarInput> Inputs { get; set; }

    public IReadOnlyDictionary<string, Guid>? CustomVoices { get; set; }

    public BatchSynthesisconfig? SynthesisConfig { get; set; }

    public BatchSynthesisProperties? Properties { get; set; }

    public required BatchAvatarconfig AvatarConfig { get; set; }
}

public class BatchAvatarJob
{
    public required string Id { get; set; }

    public string? Description { get; set; }

    public required string Status { get; set; }
    
    public DateTime CreatedDateTime { get; set; }
    
    public DateTime LastActionDateTime { get; set; }
        
    public IReadOnlyDictionary<string, Guid>? CustomVoices { get; set; }
    
    public BatchSynthesisconfig? SynthesisConfig { get; set; }
    
    public BatchSynthesisProperties? Properties { get; set; }
    
    public required BatchAvatarconfig AvatarConfig { get; set; }
    
    public BatchSynthesisOutputs? Outputs { get; set; }
}


public class BatchAvatarInput
{
    public required string Content { get; set; }
}


public class BatchSynthesisProperties
{
    public int TimeToLiveInHours { get; set; }

    public Uri? DestinationContainerUrl { get; set; }

    public string? DestinationPath { get; set; }

    public int? sizeInBytes { get; set; }
    
    public int? succeededCount { get; set; }
    
    public int? failedCount { get; set; }
    
    public int? durationInMilliseconds { get; set; }
    
    public Billingdetails? billingDetails { get; set; }

    public Error? Error { get; set; }
}

public class Billingdetails
{
    public int neuralCharacters { get; set; }
    public int talkingAvatarDurationSeconds { get; set; }
}

public class BatchSynthesisconfig
{
    public required string Voice { get; set; }

    public string? Style { get; set; }

    public string? Rate { get; set; }

    public string? Pitch { get; set; }

    public string? Volume { get; set; }
}

public class BatchAvatarconfig
{
    public required string TalkingAvatarCharacter { get; set; }

    public string? TalkingAvatarStyle { get; set; }

    public string? VideoFormat { get; set; }
    
    public string? VideoCodec { get; set; }
    
    public string? SubtitleType { get; set; }
    
    public string? BackgroundColor { get; set; }
    
    public Uri? BackgroundImage { get; set; }

    public Coordinate? AvatarPosition { get; set; }

    public Coordinate? AvatarSize { get; set; }

    public Videocrop? VideoCrop { get; set; }
    
    public int? BitrateKbps { get; set; }

    public bool? Customized { get; set; }
}

public class Videocrop
{
    public required Coordinate TopLeft { get; set; }
    
    public required Coordinate BottomRight { get; set; }
}

public record Coordinate(int X, int Y);

public class BatchSynthesisOutputs
{
    public required string Result { get; set; }

    public required string Summary { get; set; }
}

public class BillingDetails 
{
    public long NeuralCharacters { get; set; }

    public long CustomNeuralCharacters { get; set; }

    public long talkingAvatarDurationSeconds { get; set; }

    public long AoaiCharacters { get; set; }

    public long AoaiHDCharacters { get; set; }
}

public class Error
{
    public required string Code { get; set; }

    public required string Message { get; set; }
}

public class PaginatedResults<T>
{
    public required IList<T> Value { get; set; }

    public Uri? NextLink { get; set; }
}