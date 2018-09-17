//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef property_collection_h
#define property_collection_h

#include <Foundation/Foundation.h>

/**
  * Represents collection or properties and their values.
  */
@protocol PropertyCollection

/**
  * Returns the property value.
  * If the name is not available, it returns an empty String.
  * @param name The property name.
  * @return value of the property.
  */
-(NSString *)getPropertyByName:(NSString *)name;

/**
  * Sets the propery value by name
  * @param name The property name.
  * @param value The value of the property.
  */
-(void)setPropertyTo:(NSString *)value byName:(NSString *)name;

// -(void)setPropertyTo:(NSString *)value byId:(SpeechPropertyId)propertyID;
// -(NSString *)getPropertyById:(SpeechPropertyId)propertyID;

@end

#endif /* property_collection_h */
