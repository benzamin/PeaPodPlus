//
//  SetingsViewController.m
//  pebbleremote
//
//  Created by Benzamin on 8/29/13.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import "SetingsViewController.h"
#import "KBViewController.h"

@interface SetingsViewController ()



@end

@implementation SetingsViewController

@synthesize silderValue, sldrCaptureValue, lblCaptureValue, txtConnectSound, txtDisconnectSound, txtPingSound, btnInfoSounds, btnResetSounds, switchSounds;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    [self setTitle:@"Settings"];
    self.navigationController.navigationBar.translucent = NO;
     self.silderValue = [[[NSUserDefaults standardUserDefaults] objectForKey:CAPTURE_DELAY_KEY] floatValue];
    [[self sldrCaptureValue] setValue:self.silderValue animated:YES];
    self.lblCaptureValue.text = [NSString stringWithFormat:@"%.1f Sec", self.sldrCaptureValue.value ];
    [self.switchSounds setOn:[[NSUserDefaults standardUserDefaults] boolForKey:SOUND_ENABLED_KEY]];
    self.txtConnectSound.text = [NSString stringWithFormat:@"%@", [[NSUserDefaults standardUserDefaults] objectForKey:AUDIO_TYPE_CONNECT_KEY]];
    self.txtDisconnectSound.text = [NSString stringWithFormat:@"%@", [[NSUserDefaults standardUserDefaults] objectForKey:AUDIO_TYPE_DISCONNECT_KEY]];
    self.txtPingSound.text = [NSString stringWithFormat:@"%@", [[NSUserDefaults standardUserDefaults] objectForKey:AUDIO_TYPE_PING_KEY]];
    
}

- (IBAction) sliderValueChanged:(UISlider *)sender {
    self.lblCaptureValue.text = [NSString stringWithFormat:@"%.1f Sec", [sender value]];
    self.silderValue = [sender value];
}



-(IBAction)resetSoundPressed:(id)sender
{
    [[NSUserDefaults standardUserDefaults] setObject:[NSString stringWithFormat:@"%d", ConnectSystemSoundIDNNewsFlash] forKey:AUDIO_TYPE_CONNECT_KEY];
    [[NSUserDefaults standardUserDefaults] setObject:[NSString stringWithFormat:@"%d", DisconnectSystemSoundIDNoir] forKey:AUDIO_TYPE_DISCONNECT_KEY];
    [[NSUserDefaults standardUserDefaults] setObject:[NSString stringWithFormat:@"%d", PingSystemSoundIDUpdate] forKey:AUDIO_TYPE_PING_KEY];
    
    self.txtConnectSound.text = [NSString stringWithFormat:@"%d", ConnectSystemSoundIDNNewsFlash];
    self.txtDisconnectSound.text = [NSString stringWithFormat:@"%d", DisconnectSystemSoundIDNoir];
    self.txtPingSound.text = [NSString stringWithFormat:@"%d", PingSystemSoundIDUpdate];
}

-(IBAction)infoButtonPressed:(id)sender
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString: @"http://iphonedevwiki.net/index.php/AudioServices"]];
}

-(IBAction)installWatchAppPressed:(id)sender
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://www.mypebblefaces.com/apps/5953/5827/"]];
}

-(BOOL)isSoundIdValid:(NSString *)soundID
{
    if((![soundID isEqualToString:@""]) &&  ([soundID integerValue] > 999) && ([soundID integerValue] < 2000)  )
        return YES;
    
    return NO;
}

-(void)viewWillDisappear:(BOOL)animated
{
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithFloat:self.silderValue] forKey:CAPTURE_DELAY_KEY];

    [[NSUserDefaults standardUserDefaults] setBool:self.switchSounds.isOn forKey:SOUND_ENABLED_KEY];
    
    if([self isSoundIdValid:txtPingSound.text])
    {
        [[NSUserDefaults standardUserDefaults] setObject:txtPingSound.text forKey:AUDIO_TYPE_PING_KEY];
    }
    else
    {
        [[NSUserDefaults standardUserDefaults] setObject:[NSString stringWithFormat:@"%d", PingSystemSoundIDUpdate] forKey:AUDIO_TYPE_PING_KEY];

    }
    
    if([self isSoundIdValid:txtConnectSound.text])
    {
            [[NSUserDefaults standardUserDefaults] setObject:txtConnectSound.text forKey:AUDIO_TYPE_CONNECT_KEY];
    }
    else
    {
         [[NSUserDefaults standardUserDefaults] setObject:[NSString stringWithFormat:@"%d", ConnectSystemSoundIDNNewsFlash] forKey:AUDIO_TYPE_CONNECT_KEY];
    }
    
    if([self isSoundIdValid:txtDisconnectSound.text])
    {
        [[NSUserDefaults standardUserDefaults] setObject:txtDisconnectSound.text forKey:AUDIO_TYPE_DISCONNECT_KEY];
    }
    else
    {
        [[NSUserDefaults standardUserDefaults] setObject:[NSString stringWithFormat:@"%d", DisconnectSystemSoundIDNoir] forKey:AUDIO_TYPE_DISCONNECT_KEY];

    }
    
    
    [[NSUserDefaults standardUserDefaults] synchronize];
    
}


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
