/**
 * @header Login view
 * Accepts credentials from the user.
 */

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
//  CloudAuthWebViewClient.h
//  UAAClientLoginApp
//
//  Created by RaghavendraX Gutta on 23/02/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CDKNetworkError.h"
#import "CDKHTTPError.h"

@class CloudAuthProvider;
@class AuthViewController;

typedef void (^ICloudAuthWebViewTokenSuccessCb)(NSString* accessToken, NSString* refreshToken);
typedef void (^ICloudAuthWebViewTokenFailureCb)(CDKError* error);
/**
 * The web view which will accept login credentials
 * from the user. To customize, inherit and then call
 * the respective setter in the CloudAuth class.
 * 
 * @see CloudAuthProvider
 */
@interface CloudAuthWebViewClient : NSObject<UIWebViewDelegate>

-(id)init;
-(void)setLoginParams:(NSString*)baseURL clientId:(NSString*)clientId clientSecret:(NSString*)clientSecret onSuccess:(ICloudAuthWebViewTokenSuccessCb)success onFailure:(ICloudAuthWebViewTokenFailureCb)failure;
- (NSURL*)getAuthCodeUrl;

@end
