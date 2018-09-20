//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include <Foundation/Foundation.h>

/**
  * Represents collection or properties and their values.
  */
@protocol SPXPropertyCollection

/**
  * Returns the property value.
  * If the name is not available, it returns an empty string.
  * @param name property name.
  * @return value of the property.
  */
-(nullable NSString *)getPropertyByName:(nonnull NSString *)name;

/**
  * Returns the property value.
  * If the name is not available, it returns the specified default value.
  * @param name property name.
  * @param defaultValue default value which is returned if the property is not available in the collection.
  * @return value of the property.
  */
-(nullable NSString *)getPropertyByName:(nonnull NSString *)name defaultValue:(nonnull NSString *)defaultValue;

/**
  * Returns the property value.
  * If the specified id is not available, it returns an empty string.
  * @param propertyId property id.
  * @return value of the property.
  */
-(nullable NSString *)getPropertyById:(SPXPropertyId)propertyId;

/**
  * Returns the property value.
  * If the specified id is not available, it returns an empty string.
  * @param propertyId property id.
  * @param defaultValue default value which is returned if the property is not available in the collection.
  * @return value of the property.
  */
-(nullable NSString *)getPropertyById:(SPXPropertyId)propertyId defaultValue:(nonnull NSString *)defaultValue;

/**
  * Sets the propery value by name.
  * @param name property name.
  * @param value value of the property.
  */
@optional
-(void)setPropertyTo:(nonnull NSString *)value byName:(nonnull NSString *)name;

/**
  * Sets the propery value by property id.
  * @param propertyId property id.
  * @param value value of the property.
  */
@optional
-(void)setPropertyTo:(nonnull NSString *)value byId:(SPXPropertyId)propertyId;

@end
