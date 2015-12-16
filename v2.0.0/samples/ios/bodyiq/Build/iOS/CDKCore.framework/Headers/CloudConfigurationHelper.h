//
//  CloudConfigurationHelper.h
//  CloudSDK
//
//  Created by Johnathan D Raymond on 11/2/15.
//  Copyright © 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>

#define CLDSDK_CONFIG_DOMAIN_NAME @"domain-name"
#define CLDSDK_CONFIG_DEPLOYMENT_TYPE @"deployment-type"
#define CLDSDK_CONFIG_CLIENT_ID @"client-id"
#define CLDSDK_CONFIG_CLIENT_SECRET @"client-secret"
#define CLDSDK_CONFIG_CLIENT_PUBLIC_TOKEN @"client-public-token"
#define CLDSDK_CONFIG_PROTOCOL @"https://"

@interface CloudConfigurationHelper : NSObject

+(NSString*) configurationForKey:(NSString*)key;
+(void) updateConfigurationAtKey:(NSString*)key withValue:(NSString*)value;
+(NSString*) buildServiceUrl:(NSString*)serviceName;

@end
