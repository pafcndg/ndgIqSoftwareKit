//
//  JBLog.h
//  places-api
//
//  Created by Bari Levi on 3/4/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

/**
 
 The log types are:
 JBLOG_DBG
 JBLOG_NRM
 JBLOG_ALT
 JBLOG_ERR
 
 */

#ifndef __JavaBridge__JBLog_h
#define __JavaBridge__JBLog_h

#ifndef JBLOG_LEVEL
#if TARGET_IPHONE_SIMULATOR != 0
#define JBLOG_LEVEL 0
#else
#define JBLOG_LEVEL 5
#endif
#endif

/*****************************************************************************/
/* Entry/exit trace macros                                                   */
/*****************************************************************************/
#if JBLOG_LEVEL == 0
#define JB_ENTRY    NSLog(@"ENTRY: %s:%d:", __PRETTY_FUNCTION__,__LINE__);
#define JB_EXIT     NSLog(@"EXIT:  %s:%d:", __PRETTY_FUNCTION__,__LINE__);
#else
#define JB_ENTRY
#define JB_EXIT
#endif

/*****************************************************************************/
/* Debug trace macros                                                        */
/*****************************************************************************/
#if (JBLOG_LEVEL <= 1)
#define JBLOG_DBG(A, ...) NSLog(@"DEBUG: %s:%d:%@", __PRETTY_FUNCTION__,__LINE__,[NSString stringWithFormat:A, ## __VA_ARGS__]);
#else
#define JBLOG_DBG(A, ...)
#endif

#if (JBLOG_LEVEL <= 2)
#define JBLOG_NRM(A, ...) NSLog(@"NORMAL:%s:%d:%@", __PRETTY_FUNCTION__,__LINE__,[NSString stringWithFormat:A, ## __VA_ARGS__]);
#else
#define JBLOG_NRM(A, ...)
#endif

#if (JBLOG_LEVEL <= 3)
#define JBLOG_ALT(A, ...) NSLog(@"ALERT: %s:%d:%@", __PRETTY_FUNCTION__,__LINE__,[NSString stringWithFormat:A, ## __VA_ARGS__]);
#else
#define JBLOG_ALT(A, ...)
#endif

#if (JBLOG_LEVEL <= 4)
#define JBLOG_ERR(A, ...) NSLog(@"ERROR: %s:%d:%@", __PRETTY_FUNCTION__,__LINE__,[NSString stringWithFormat:A, ## __VA_ARGS__]);
#else
#define JBLOG_ERR(A, ...)
#endif
#endif
