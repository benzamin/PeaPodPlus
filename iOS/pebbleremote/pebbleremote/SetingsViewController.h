//
//  SetingsViewController.h
//  pebbleremote
//
//  Created by Benzamin on 8/29/13.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>

@interface SetingsViewController : UIViewController <MPMediaPickerControllerDelegate>

@property(nonatomic, assign) float silderValue;
@property (nonatomic, strong) IBOutlet UISlider *sldrCaptureValue;
@property (nonatomic, strong) IBOutlet UILabel *lblCaptureValue;
@property (nonatomic, strong) IBOutlet UITextField *txtPingSound;
@property (nonatomic, strong) IBOutlet UITextField *txtConnectSound;
@property (nonatomic, strong) IBOutlet UITextField *txtDisconnectSound;
@property (nonatomic, strong) IBOutlet UIButton *btnInfoSounds;
@property (nonatomic, strong) IBOutlet UIButton *btnResetSounds;
@property (nonatomic, strong) IBOutlet UISwitch *switchSounds;

@end
