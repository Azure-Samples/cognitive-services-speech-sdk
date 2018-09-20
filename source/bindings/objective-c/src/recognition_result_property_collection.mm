//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "speechapi_private.h"

@implementation RecognitionResultPropertyCollection
{
    // We have to keep the recognizer instance alive in order to make sure the property collection is valid.
    std::shared_ptr<SpeechImpl::RecognitionResult>resultImpl;
}

-(instancetype)initFrom :(std::shared_ptr<SpeechImpl::RecognitionResult>)resultHandle;
{
    self = [super init];
    resultImpl = resultHandle;
    return self;
}

-(NSString *)getPropertyByName:(NSString *)name
{
    return [NSString stringWithString:resultImpl->Properties.GetProperty([name string])];
}

-(NSString *)getPropertyByName:(NSString *)name defaultValue:(NSString *)defaultValue
{
    return [NSString stringWithString:resultImpl->Properties.GetProperty([name string], [defaultValue string])];
}

-(NSString *)getPropertyById:(SPXPropertyId)propertyId
{
    return [NSString stringWithString:resultImpl->Properties.GetProperty((SpeechImpl::PropertyId)(int)propertyId)];
}

-(NSString *)getPropertyById:(SPXPropertyId)propertyId defaultValue:(NSString *)defaultValue
{
    return [NSString stringWithString:resultImpl->Properties.GetProperty((SpeechImpl::PropertyId)(int)propertyId, [defaultValue string])];
}

@end
