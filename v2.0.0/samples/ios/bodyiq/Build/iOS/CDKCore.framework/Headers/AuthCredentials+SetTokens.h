/*******************************************************************************
 * INTEL CONFIDENTIAL
 *
 * Copyright  2014 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to the source code
 * ("Material") are owned by Intel Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors. The Material contains trade
 * secrets and proprietary and confidential information of Intel or its suppliers and licensors.
 * The Material is protected by worldwide copyright and trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior express written
 * permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual property right is
 * granted to or conferred upon you by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
 ******************************************************************************/
//
//  AuthCredentials+SetTokens.h
//  CloudSDK
//
//  Created by RaghavendraX Gutta on 10/03/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import "AuthCredentials.h"
#import "CDKError.h"
#import "CloudJsonRequest.h"

#define ACCESS_TOKEN_KEY @"access_token"
#define REFRESH_TOKEN_KEY @"refresh_token"
#define EMPTY_STRING @""
#define NSERROR_OBJECT @"NSError_Object"

@interface AuthCredentials (SetTokens)

@property(nonatomic,retain)NSString *identifier;
@property(nonatomic,retain)NSString *accessToken;
@property(nonatomic,retain)NSString *refreshToken;
@property(nonatomic,retain)NSString *userId;
@property(nonatomic,retain)NSString *userName;
@property(nonatomic,retain)NSString *clientId;
@property(nonatomic,retain)NSString *clientSecret;
@property(nonatomic,retain)NSString *authBaseUrl;
@property(nonatomic,retain)CDKError* createError;//error during auth-token response parsing

- (id)initWithAccessToken:(NSString*)accessToken refreshToken:(NSString*)refreshToken
                 provider:(CloudAuthProvider*)authProvider;
- (AuthCredentials*) initWithJsonPayload: (NSData*) jsonPayload identifier:(NSString*)identifier;
- (AuthCredentials*)initWithIdentifier:(NSString*)identifier;
- (void)validateToken:(NSString*)token callback:(ICloudResponseCb)callback;
- (void)refreshAccessToken:(ICloudResponseCb)callback;
- (CDKError*)storeTokens:(NSData*)tokenPayLoad identifier:(NSString*)identifier;
+ (NSString*)getIdentifier:(CloudAuthProvider*)authProvider;
- (NSData*)createKeychainPayloadWithAccessToken:(NSString*)accessToken refreshToken:(NSString*)refreshToken;
- (NSData*)createKeychainPayloadWithRefreshToken:(NSString*)refreshToken;

@end
