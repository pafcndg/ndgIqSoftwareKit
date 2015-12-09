#ifndef __JavaBridge_eAuditLabels__
#define __JavaBridge_eAuditLabels__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class eAuditLabels;

typedef NS_ENUM(NSInteger, eeAuditLabels)
{
 	eAuditLabels_TEST_LABEL , 
 	eAuditLabels_RESOLVER_SDK_LABEL , 
 	eAuditLabels_PROVIDERS_LABEL , 
 	eAuditLabels_ROUTE_PROVIDER , 
 	eAuditLabels_EVENTS_SDK_LABEL , 
 	eAuditLabels_PLACES_RAW_SENSOR_DATA_GPS , 
 	eAuditLabels_PLACES_RAW_SENSOR_DATA_FUSED , 
 	eAuditLabels_PLACES_RAW_SENSOR_DATA_WIFI , 
 	eAuditLabels_PLACES_RAW_SENSOR_DATA_ACTIVITY , 
 	eAuditLabels_PLACES_RAW_SENSOR_DATA_BLUETOOTH , 
 	eAuditLabels_PLACES_RAW_SENSOR_DATA_BATTERY , 
 	eAuditLabels_PLACES_RAW_SENSOR_DATA_USER_INPUT , 
 	eAuditLabels_PLACES_RAW_SENSOR_DATA_PEDOMETER , 
 	eAuditLabels_PLACES_RAW_SENSOR_DATA_DEVICE_STATE , 
 	eAuditLabels_VISIT_IN_PLACE_EVENTS , 
 	eAuditLabels_USER_STATE , 
 	eAuditLabels_PLACE_TRIGGER , 
 	eAuditLabels_CHARGE_TRIGGER , 
 	eAuditLabels_TIME_TRIGGER , 
 	eAuditLabels_MOT_TRIGGER , 
 	eAuditLabels_BE_EVENT_AUDIT , 
 	eAuditLabels_TTL_EVENT_AUDIT , 
 	eAuditLabels_MOT_CHANGE , 
 	eAuditLabels_API_USAGE_AUDIT , 
 	eAuditLabels_DEVICE_DATA , 

};

@interface eAuditLabels : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (eAuditLabels *)valueOfWithNSString:(NSString*)name;
+ (eAuditLabels *)valueOf:(NSString*)name;
+ (eAuditLabels *)parseWithInt:(int)name;
+ (eAuditLabels *)fromIdWithInt:(int)name;
+ (eAuditLabels *)fromId:(int)val;

+ (void)load;
-(eeAuditLabels) ordinal;

 +(eAuditLabels*) TEST_LABEL; 
 +(eAuditLabels*) RESOLVER_SDK_LABEL; 
 +(eAuditLabels*) PROVIDERS_LABEL; 
 +(eAuditLabels*) ROUTE_PROVIDER; 
 +(eAuditLabels*) EVENTS_SDK_LABEL; 
 +(eAuditLabels*) PLACES_RAW_SENSOR_DATA_GPS; 
 +(eAuditLabels*) PLACES_RAW_SENSOR_DATA_FUSED; 
 +(eAuditLabels*) PLACES_RAW_SENSOR_DATA_WIFI; 
 +(eAuditLabels*) PLACES_RAW_SENSOR_DATA_ACTIVITY; 
 +(eAuditLabels*) PLACES_RAW_SENSOR_DATA_BLUETOOTH; 
 +(eAuditLabels*) PLACES_RAW_SENSOR_DATA_BATTERY; 
 +(eAuditLabels*) PLACES_RAW_SENSOR_DATA_USER_INPUT; 
 +(eAuditLabels*) PLACES_RAW_SENSOR_DATA_PEDOMETER; 
 +(eAuditLabels*) PLACES_RAW_SENSOR_DATA_DEVICE_STATE; 
 +(eAuditLabels*) VISIT_IN_PLACE_EVENTS; 
 +(eAuditLabels*) USER_STATE; 
 +(eAuditLabels*) PLACE_TRIGGER; 
 +(eAuditLabels*) CHARGE_TRIGGER; 
 +(eAuditLabels*) TIME_TRIGGER; 
 +(eAuditLabels*) MOT_TRIGGER; 
 +(eAuditLabels*) BE_EVENT_AUDIT; 
 +(eAuditLabels*) TTL_EVENT_AUDIT; 
 +(eAuditLabels*) MOT_CHANGE; 
 +(eAuditLabels*) API_USAGE_AUDIT; 
 +(eAuditLabels*) DEVICE_DATA; 


@end

 #define eAuditLabels_get_TEST_LABEL() [eAuditLabels TEST_LABEL] 
 #define eAuditLabels_get_RESOLVER_SDK_LABEL() [eAuditLabels RESOLVER_SDK_LABEL] 
 #define eAuditLabels_get_PROVIDERS_LABEL() [eAuditLabels PROVIDERS_LABEL] 
 #define eAuditLabels_get_ROUTE_PROVIDER() [eAuditLabels ROUTE_PROVIDER] 
 #define eAuditLabels_get_EVENTS_SDK_LABEL() [eAuditLabels EVENTS_SDK_LABEL] 
 #define eAuditLabels_get_PLACES_RAW_SENSOR_DATA_GPS() [eAuditLabels PLACES_RAW_SENSOR_DATA_GPS] 
 #define eAuditLabels_get_PLACES_RAW_SENSOR_DATA_FUSED() [eAuditLabels PLACES_RAW_SENSOR_DATA_FUSED] 
 #define eAuditLabels_get_PLACES_RAW_SENSOR_DATA_WIFI() [eAuditLabels PLACES_RAW_SENSOR_DATA_WIFI] 
 #define eAuditLabels_get_PLACES_RAW_SENSOR_DATA_ACTIVITY() [eAuditLabels PLACES_RAW_SENSOR_DATA_ACTIVITY] 
 #define eAuditLabels_get_PLACES_RAW_SENSOR_DATA_BLUETOOTH() [eAuditLabels PLACES_RAW_SENSOR_DATA_BLUETOOTH] 
 #define eAuditLabels_get_PLACES_RAW_SENSOR_DATA_BATTERY() [eAuditLabels PLACES_RAW_SENSOR_DATA_BATTERY] 
 #define eAuditLabels_get_PLACES_RAW_SENSOR_DATA_USER_INPUT() [eAuditLabels PLACES_RAW_SENSOR_DATA_USER_INPUT] 
 #define eAuditLabels_get_PLACES_RAW_SENSOR_DATA_PEDOMETER() [eAuditLabels PLACES_RAW_SENSOR_DATA_PEDOMETER] 
 #define eAuditLabels_get_PLACES_RAW_SENSOR_DATA_DEVICE_STATE() [eAuditLabels PLACES_RAW_SENSOR_DATA_DEVICE_STATE] 
 #define eAuditLabels_get_VISIT_IN_PLACE_EVENTS() [eAuditLabels VISIT_IN_PLACE_EVENTS] 
 #define eAuditLabels_get_USER_STATE() [eAuditLabels USER_STATE] 
 #define eAuditLabels_get_PLACE_TRIGGER() [eAuditLabels PLACE_TRIGGER] 
 #define eAuditLabels_get_CHARGE_TRIGGER() [eAuditLabels CHARGE_TRIGGER] 
 #define eAuditLabels_get_TIME_TRIGGER() [eAuditLabels TIME_TRIGGER] 
 #define eAuditLabels_get_MOT_TRIGGER() [eAuditLabels MOT_TRIGGER] 
 #define eAuditLabels_get_BE_EVENT_AUDIT() [eAuditLabels BE_EVENT_AUDIT] 
 #define eAuditLabels_get_TTL_EVENT_AUDIT() [eAuditLabels TTL_EVENT_AUDIT] 
 #define eAuditLabels_get_MOT_CHANGE() [eAuditLabels MOT_CHANGE] 
 #define eAuditLabels_get_API_USAGE_AUDIT() [eAuditLabels API_USAGE_AUDIT] 
 #define eAuditLabels_get_DEVICE_DATA() [eAuditLabels DEVICE_DATA] 


#endif
