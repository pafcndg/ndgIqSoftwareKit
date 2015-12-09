//
//  IOSHttpProvider.h
//  places-api
//
//  Created by Ben Nagar on 3/9/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IHttpProvider.h"
#import "ResolveRequestV1.h"
#import "IJSONUtils.h"
#import "JBHashSet.h"

@interface IOSHttpProvider : NSObject <IHttpProvider>
{
    id<IJSONUtils> m_JSONUtils;
    HashSet *m_httpSentDataListeners;
    NSObject *m_lock;
}

-(ResultData*) sendAndReceiveWithId:(NSObject *)request
                    withReflectType:(JavaLangReflectType)typeOf
                       withNSString:(NSString*)url;

- (ResultData *)sendAndReceiveWithId:(id)request
                     withReflectType:(JavaLangReflectType)typeOfResponse
                        withNSString:(NSString *)url
                             withInt:(int)readTimeOut
                             withInt:(int)connectionTimeOut;

- (ResultData *)sendAndReceiveWithInputStream:(InputStream *)inputStream
                              withReflectType:(JavaLangReflectType)typeOf
                                 withNSString:(NSString *)url;

- (void)registerListenerWithIHttpSentDataListener:(id<IHttpSentDataListener>)httpSentDataListener;

- (void)unRegisterListenerWithIHttpSentDataListener:(id<IHttpSentDataListener>)httpSentDataListener;

- (void)setNetworkCredentialsWithId:(id)credentials;

@end
