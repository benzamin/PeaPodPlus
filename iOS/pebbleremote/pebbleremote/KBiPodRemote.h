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

#define FIND_PHONE_PLAY_SOUND_KEY @(0xFEE1)
#define GET_BATTERY_STATUS_KEY @(0xFEE2)
#define GET_NOTES_LIST_KEY @(0xFEE3)
#define GET_SPECIFIC_NOTE_KEY @(0xFEE4)
#define GET_EVENTS_REMINDERS_KEY @(0xFEE5)
#define CAMERA_CAPTURE_KEY @(0xFEE6)



@interface KBiPodRemote : NSObject<PBPebbleCentralDelegate>
{
}
-(void)checkEventStoreAccessForCalendar:(BOOL)push;
- (id)initWithViewControllerReference:(KBViewController*)vc;
@property (nonatomic, strong) KBPebbleMessageQueue *message_queue;
@end
