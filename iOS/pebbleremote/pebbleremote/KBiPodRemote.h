//
//  KBiPodRemote.h
//  pebbleremote
//
//  Created by Katharine Berry on 25/05/2013.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <PebbleKit/PebbleKit.h>
#include <AudioToolbox/AudioToolbox.h>
#import "KBViewController.h"
#import "KBPebbleMessageQueue.h"
#import "KBPebbleValue.h"
#import "NSData+Base64.h"
#import <PebbleKit/PebbleKit.h>
#import <CoreData/CoreData.h>
#import <CoreLocation/CoreLocation.h>

#define FIND_PHONE_PLAY_SOUND_KEY @(0xFEE1)
#define GET_BATTERY_STATUS_KEY @(0xFEE2)
#define GET_NOTES_LIST_KEY @(0xFEE3)
#define GET_SPECIFIC_NOTE_KEY @(0xFEE4)
#define GET_EVENTS_REMINDERS_KEY @(0xFEE5)
#define CAMERA_CAPTURE_KEY @(0xFEE6)

//httpebble
#define HTTP_URL_KEY @(0xFFFF)
#define HTTP_STATUS_KEY @(0xFFFE)
#define HTTP_SUCCESS_KEY_DEPRECATED @(0xFFFD)
#define HTTP_COOKIE_KEY @(0xFFFC)
#define HTTP_CONNECT_KEY @(0xFFFB)

#define HTTP_APP_ID_KEY @(0xFFF2)
#define HTTP_COOKIE_STORE_KEY @(0xFFF0)
#define HTTP_COOKIE_LOAD_KEY @(0xFFF1)
#define HTTP_COOKIE_FSYNC_KEY @(0xFFF3)
#define HTTP_COOKIE_DELETE_KEY @(0xFFF4)

#define HTTP_TIME_KEY @(0xFFF5)
#define HTTP_UTC_OFFSET_KEY @(0xFFF6)
#define HTTP_IS_DST_KEY @(0xFFF7)
#define HTTP_TZ_NAME_KEY @(0xFFF8)

#define HTTP_LOCATION_KEY @(0xFFE0)
#define HTTP_LATITUDE_KEY @(0xFFE1)
#define HTTP_LONGITUDE_KEY @(0xFFE2)
#define HTTP_ALTITUDE_KEY @(0xFFE3)


@class KBiPodRemote;

@protocol KBPebbleRemoteDelegate <NSObject>

- (void)pebbleThing:(KBiPodRemote*)thing connected:(PBWatch *)watch;
- (void)pebbleThing:(KBiPodRemote*)thing disconnected:(PBWatch *)watch;
- (void)pebbleThing:(KBiPodRemote*)thing found:(PBWatch*)watch;
- (void)pebbleThing:(KBiPodRemote*)thing lost:(PBWatch *)watch;

@end

@interface KBiPodRemote : NSObject<PBPebbleCentralDelegate, CLLocationManagerDelegate>
{
}
-(void)checkEventStoreAccessForCalendar:(BOOL)push;
- (id)initWithViewControllerReference:(KBViewController*)vc;
@property (nonatomic, strong) KBPebbleMessageQueue *message_queue;

//httpebble
@property (nonatomic, assign) id<KBPebbleRemoteDelegate> delegate;
- (id)initWithDelegate:(id<KBPebbleRemoteDelegate>)delegate;
- (void)saveKeyValueData;
@end
