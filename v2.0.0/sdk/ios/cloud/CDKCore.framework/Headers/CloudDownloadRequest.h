//
//  CloudDownloadRequest.h
//  CloudSDK
//
//  Created by RaghavendraX Gutta on 07/08/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import "CloudRequest.h"

/**
 *  An app callback that returns the cloud's download start/completion to the application.
 *
 *  @param statusCode response status code
 *  @param payLoad Message on start or completion
 */
typedef void (^IDownloadStartOrCompletionCb)(int statusCode, NSData* payLoad);

/**
 *  An app callback that returns the cloud's error response to the application.
 *
 *  @param statusCode response status code
 *  @param error contains the cloud error response
 */
typedef void (^IDownloadFailedCb)(int statusCode, CDKError *error);

/**
 *  Download progress callback. Intermittently informs app as data byes
 *  in a long-running download arrive.
 *
 *  @param bytesWritten              number of bytes written locally
 *  @param totalBytesWritten         total bytes written until now
 *  @param totalBytesExpectedToWrite total download size
 */
typedef void (^IDownloadProgressCb)(int64_t bytesWritten, int64_t totalBytesWritten,int64_t totalBytesExpectedToWrite);

/**
 * Download request interface
 */
@interface CloudDownloadRequest : CloudRequest

/**
 * folder name where the download should be saved in Documents dir
 */
@property(readonly,nonatomic,retain)NSString *folderName;

/**
 * file name where the download should be saved in Documents/folder dir
 */
@property(readonly,nonatomic,retain)NSString *fileName;


/**
 * create the download request
 * @param baseUrl see property with same name
 * @param folderName  save download in folder Documents/folderName
 * @param fileName  save download at Documents/folderName/fileName
 * @param sessionType see property with same name
 * @param creds the authentication credentials
 */
-(id)initDownloadRequestWith:(NSString*)baseUrl
                      folder:(NSString*)folderName
                        file:(NSString*)fileName
             httpSessionType:(HttpSessionType)sessionType
                 credentials:(AuthCredentials*)creds;

-(void)fireDownloadTask:(BOOL)doAllowCellularAccess
                success:(IDownloadStartOrCompletionCb)successCallback
                failure:(IDownloadFailedCb)failureCallback
               progress:(IDownloadProgressCb)progressCallback
              completed:(IDownloadStartOrCompletionCb)finishedCallback;

+(CDKError*)cancelDownloadOn:(NSString*)downloadFileId;
+(void)cancelAllDownloads;

@end
