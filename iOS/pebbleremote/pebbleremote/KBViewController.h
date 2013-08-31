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
#define AUDIO_TYPE_PING_KEY @"BBAudioTypePingKey"
#define AUDIO_TYPE_CONNECT_KEY @"BBAudioTypeConnectKey"
#define AUDIO_TYPE_DISCONNECT_KEY @"BBAudioTypeDCKey"
#define APP_IS_HTTPEBBLE_MODE_KEY @"BBAppIsHttpebbleMode"
#define SHOULD_BE_CONNECTED_KEY @"BBShouldBeConnected"

#define ConnectSystemSoundIDNNewsFlash    1028
#define DisconnectSystemSoundIDNoir    1029
#define PingSystemSoundIDUpdate    1036

@class PBWatch;
@class KBiPodRemote;

@interface KBViewController : UIViewController <UINavigationControllerDelegate, UIImagePickerControllerDelegate>
{
    IBOutlet UILabel* connectedLabel;
    IBOutlet UIButton* connectButton;
    IBOutlet UIImageView* pepodImage;
    IBOutlet UISwitch* appModeSwitch;
    BOOL shouldBeConnected;
    BOOL couldConnect;
    BOOL isConnected;
    BOOL isReconnect;
}

@property(nonatomic, strong) KBiPodRemote *remote;

- (void)pebbleFound:(PBWatch *)watch ;
- (void)pebbleConnected:(PBWatch *)watch ;
- (void)pebbleDisconnected:(PBWatch *)watch ;
- (void)pebbleLost:(PBWatch *)watch ;

- (IBAction)toggleConnected:(id)sender;
-(void)initiateCamera;
-(void) removeCameraWindow;
-(void)operateCamera:(NSInteger)operationKey;
@end
