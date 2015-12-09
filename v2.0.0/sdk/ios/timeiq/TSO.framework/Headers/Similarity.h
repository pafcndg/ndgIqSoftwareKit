#ifndef __JavaBridge_Similarity__
#define __JavaBridge_Similarity__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class Similarity;

typedef NS_ENUM(NSInteger, eSimilarity)
{
 	Similarity_StrongSimilarity , 
 	Similarity_ModerateSimilarity , 
 	Similarity_WeakSimilarity , 
 	Similarity_NoSimilarity , 

};

@interface Similarity : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (Similarity *)valueOfWithNSString:(NSString*)name;
+ (Similarity *)valueOf:(NSString*)name;
+ (Similarity *)parseWithInt:(int)name;
+ (Similarity *)fromIdWithInt:(int)name;
+ (Similarity *)fromId:(int)val;

+ (void)load;
-(eSimilarity) ordinal;

 +(Similarity*) StrongSimilarity; 
 +(Similarity*) ModerateSimilarity; 
 +(Similarity*) WeakSimilarity; 
 +(Similarity*) NoSimilarity; 


@end

 #define Similarity_get_StrongSimilarity() [Similarity StrongSimilarity] 
 #define Similarity_get_ModerateSimilarity() [Similarity ModerateSimilarity] 
 #define Similarity_get_WeakSimilarity() [Similarity WeakSimilarity] 
 #define Similarity_get_NoSimilarity() [Similarity NoSimilarity] 


#endif
