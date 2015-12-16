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
//  CloudAuthRequestFactory.h
//  CloudSDK
//
//  Created by RaghavendraX Gutta on 11/03/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>

@class CloudAuthProvider;
@class CloudJsonRequest;

@interface CloudAuthRequestFactory : NSObject

-(NSString*)getAuthCodeFromUrl:(NSURL*)url;
-(CloudJsonRequest*)getLoginRequest:(NSString*)serviceBaseUrl authCodeUrl:(NSURL*)authCodeUrl
                        redirectUrl:(NSString*)redirectUrl clientId:(NSString*)clientId
                       clientSecret:(NSString*)clientSecret;
-(CloudJsonRequest*)getLoginRequest:(NSString *)email baseUrl:(NSString*)baseUrl token:(NSString *)token
                           provider:(NSString *)provider scope:(NSString *)scope clientId:(NSString*)clientId clientSecret:(NSString*)clientSecret;
-(CloudJsonRequest*)getRefreshTokenRequest:(NSString*)refreshToken baseUrl:(NSString*)baseUrl clientId:(NSString*)clientId clientSecret:(NSString*)clientSecret;
-(CloudJsonRequest*)getValidateTokenRequest:(NSString*)token baseUrl:(NSString*)baseUrl clientId:(NSString*)clientId clientSecret:(NSString*)clientSecret;
//-(NSString*) getAuthCodeRedirectUrl;
- (CloudJsonRequest*)getCreateAccountRequest:(NSString *)userId password:(NSString *)password
                                firstName:(NSString *)firstName lastName:(NSString *)lastName baseUrl:(NSString*)baseUrl clientId:(NSString*)clientId clientSecret:(NSString*)clientSecret;
-(CloudJsonRequest*)getDeleteAccountRequest:(NSString *)userId baseUrl:(NSString*)baseUrl clientId:(NSString*)clientId authorization:(NSString *)authorization;

- (CloudJsonRequest*)getChangePasswordRequest:(NSString *)userId password:(NSString *)password
                                  newPassword:(NSString *)new_password baseUrl:(NSString*)baseUrl
                                     clientId:(NSString*)clientId authorization:(NSString*)authorization;

-(CloudJsonRequest*)getUserInfoRequest:(NSString*)baseUrl clientId:(NSString*)clientId authorization:(NSString*)authorization;
@end
