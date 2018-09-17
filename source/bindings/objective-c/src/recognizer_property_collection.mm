//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation RecognizerPropertyCollection
{
    // We have to keep the recognizer instance alive in order to make sure the propertyHandle is valid
    RecognizerSharedPtr recognizerImpl;
    SpeechImpl::PropertyCollection *propertiesImpl;
}

-(instancetype)initWithPropertyCollection :(SpeechImpl::PropertyCollection *)propertiesHandle from:(RecognizerSharedPtr)recoHandle
{
    self = [super init];
    recognizerImpl = recoHandle;
    propertiesImpl = propertiesHandle;
    return self;
}

-(NSString *)getPropertyByName:(NSString *)name
{
    return [NSString stringWithString:propertiesImpl->GetProperty([name string])];
}

-(void)setPropertyTo:(NSString *)value byName:(NSString *)name
{
    propertiesImpl->SetProperty([name string], [value string]);
}

// -(void)setValue:(NSString *)value usingId:(SpeechPropertyId)propertyID;
// -(NSString *)getValueUsingId:(SpeechPropertyId)propertyID;

@end
