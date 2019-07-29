//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "speechapi_private.h"

@implementation SpeechSynthesisResultPropertyCollection
{
    // We have to keep the speech synthesis result instance alive in order to make sure the property collection is valid.
    std::shared_ptr<SpeechImpl::SpeechSynthesisResult>resultImpl;
}

-(instancetype)initFrom :(std::shared_ptr<SpeechImpl::SpeechSynthesisResult>)resultHandle;
{
    self = [super init];
    resultImpl = resultHandle;
    return self;
}

-(NSString *)getPropertyByName:(NSString *)name
{
    return [NSString StringWithStdString:resultImpl->Properties.GetProperty([name toSpxString])];
}

-(NSString *)getPropertyByName:(NSString *)name defaultValue:(NSString *)defaultValue
{
    return [NSString StringWithStdString:resultImpl->Properties.GetProperty([name toSpxString], [defaultValue toSpxString])];
}

-(NSString *)getPropertyById:(SPXPropertyId)propertyId
{
    return [NSString StringWithStdString:resultImpl->Properties.GetProperty((SpeechImpl::PropertyId)(int)propertyId)];
}

-(NSString *)getPropertyById:(SPXPropertyId)propertyId defaultValue:(NSString *)defaultValue
{
    return [NSString StringWithStdString:resultImpl->Properties.GetProperty((SpeechImpl::PropertyId)(int)propertyId, [defaultValue toSpxString])];
}

@end
