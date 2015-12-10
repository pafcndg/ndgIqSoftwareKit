//
//  CloudMultipartRequest.h
//  CloudSDK
//
//  Created by RaghavendraX Gutta on 04/08/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import "CloudRequest.h"
#import "CloudResponse.h"

/**
 *  An app callback that returns the cloud's upload start/completion to the application.
 *
 *  @param statusCode response status code
 *  @param payLoad Message on start or completion
 *  @param fileName fileName to be saved on download completion
 */

typedef void (^IUploadStartOrCompletionCb)(int statusCode, NSData* payLoad);

/**
 *  An app callback that returns the cloud's error response to the application.
 *
 *  @param statusCode response status code
 *  @param error contains the cloud error response
 */
typedef void (^IUploadFailedCb)(int statusCode, CDKError *error);

/**
 *  Upload progress callback. Intermittently informs app as data byes
 *  in a long-running upload.
 *
 *  @param bytesSent         Number of bytes sent since the last time this delegate method was called.
 *  @param totalBytesSent    The total number of bytes sent so far
 *  @param totalBytesExpectedToSend total upload size of file
 */
typedef void (^IUploadProgressCb)(int64_t bytesSent, int64_t totalBytesSent, int64_t totalBytesExpectedToSend);

/**
 * Multipart request object for the cloud
 */
@interface CloudMultipartRequest : CloudRequest

/**
 * path of the file to upload
 */
@property(readonly,nonatomic,retain)NSURL *filePath;

/**
 * Open NSInputStream of file at filepath for upload
 */
@property(readonly,nonatomic,retain)NSInputStream *inputStream;

/**
 * Length in bytes of the provided inputStream (-1 if not using for stream uploads)
 */
@property(readonly, nonatomic)NSInteger streamLength;

/**
 * upload file name to be saved as
 */
@property(readonly,nonatomic,retain)NSString *saveAsFile;

-(id)initUploadRequestWith:(NSString*)baseUrl
                  endPoint:(NSString*)urlEndPoint
                      file:(NSURL*)filePath
               toBeSavedAs:(NSString*)saveAsFile
            httpMethodType:(HttpMethodType)methodType
                formParams:(NSDictionary*)bodyParams
               httpHeaders:(NSDictionary*)headers
           httpSessionType:(HttpSessionType)sessionType
               credentials:(AuthCredentials*)creds;

-(id)initUploadRequestWith:(NSString*)baseUrl
                  endPoint:(NSString*)urlEndPoint
                      file:(NSURL*)filePath
               toBeSavedAs:(NSString*)saveAsFile
            httpMethodType:(HttpMethodType)methodType
                formParams:(NSDictionary*)bodyParams
           httpSessionType:(HttpSessionType)sessionType
               credentials:(AuthCredentials*)creds;
    
-(id)initUploadRequestWith:(NSString*)baseUrl
                  endPoint:(NSString*)urlEndPoint
                    stream:(NSInputStream*)stream
                    length:(NSInteger)streamLength
               toBeSavedAs:(NSString*)saveAsFile
            httpMethodType:(HttpMethodType)methodType
                formParams:(NSDictionary*)bodyParams
               httpHeaders:(NSDictionary*)headers
           httpSessionType:(HttpSessionType)sessionType
               credentials:(AuthCredentials*)creds;

-(void)fireUploadTask:(BOOL)doAllowCellularAccess
              success:(IUploadStartOrCompletionCb)successCallback
              failure:(IUploadFailedCb)failureCallback
             progress:(IUploadProgressCb)progressCallback
            completed:(IUploadStartOrCompletionCb)finishedCallback;

-(CloudResponse*)fireUploadTaskSync:(BOOL)doAllowCellularAccess
                           progress:(IUploadProgressCb)progressCallback;

-(CloudResponse*)fireUploadTaskSync:(BOOL)doAllowCellularAccess;

@end
