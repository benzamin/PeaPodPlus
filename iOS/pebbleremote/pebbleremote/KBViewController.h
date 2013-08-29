//
//  KBViewController.h
//  pebbleremote
//
//  Created by Katharine Berry on 25/05/2013.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import <UIKit/UIKit.h>

#define NOTE_KEY @"BBPebbleNotes"
#define CAPTURE_DELAY_KEY  @"BBCameraCaptureDelayKey"
#define SOUND_ENABLED_KEY  @"BBSoundEnabledOrNotKey"
#define AUDIO_TYPE_PING_KEY @"AudioTypePingKey"
#define AUDIO_TYPE_CONNECT_KEY @"AudioTypeConnectKey"
#define AUDIO_TYPE_DISCONNECT_KEY @"AudioTypeDCKey"

#define ConnectSystemSoundIDNNewsFlash    1028
#define DisconnectSystemSoundIDNoir    1029
#define PingSystemSoundIDUpdate    1036


@interface KBViewController : UIViewController <UINavigationControllerDelegate, UIImagePickerControllerDelegate>

-(void)initiateCamera;
-(void) removeCameraWindow;
-(void)operateCamera:(NSInteger)operationKey;
@end
