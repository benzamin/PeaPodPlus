//
//  KBViewController.m
//  pebbleremote
//
//  Created by Katharine Berry on 25/05/2013.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import "KBViewController.h"
#import "KBiPodRemote.h"
#import "NotesListViewController.h"
#import "SetingsViewController.h"


@interface KBViewController () {
    KBiPodRemote *remote;
}
@property (nonatomic) UIImagePickerController *imagePickerController;

@end

@implementation KBViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self setTitle:@"Peapod+"];
    
    if(([[NSUserDefaults standardUserDefaults] objectForKey:NOTE_KEY]) == nil)
    {
        NSMutableArray *arr = [[NSMutableArray alloc] init];
        [arr addObject:[NSString stringWithFormat:@"Pebble Note 1#^#1This is a test Note for shwoing in Pebble!"]];
        [arr addObject:[NSString stringWithFormat:@"Pebble Note 2#^#2This is a test Note for shwoing in Pebble!"]];
        [arr addObject:[NSString stringWithFormat:@"Pebble Note 3#^#3This is a test Note for shwoing in Pebble!"]];
        [[NSUserDefaults standardUserDefaults] setObject:arr forKey:NOTE_KEY];
        [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithFloat:0.0] forKey:CAPTURE_DELAY_KEY];
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:SOUND_ENABLED_KEY];
        [[NSUserDefaults standardUserDefaults] setObject:[NSString stringWithFormat:@"%d", ConnectSystemSoundIDNNewsFlash] forKey:AUDIO_TYPE_CONNECT_KEY];
        [[NSUserDefaults standardUserDefaults] setObject:[NSString stringWithFormat:@"%d", DisconnectSystemSoundIDNoir] forKey:AUDIO_TYPE_DISCONNECT_KEY];
        [[NSUserDefaults standardUserDefaults] setObject:[NSString stringWithFormat:@"%d", PingSystemSoundIDUpdate] forKey:AUDIO_TYPE_PING_KEY];
        
        
        
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    
    
	// Do any additional setup after loading the view, typically from a nib.
    remote = [[KBiPodRemote alloc] initWithViewControllerReference:self];
    
}

-(IBAction)showNotesView:(id)sender
{
    NotesListViewController *nVC = [[NotesListViewController alloc] init];
    [self.navigationController pushViewController:nVC animated:YES];
}

-(IBAction)showSettingsView:(id)sender
{
    SetingsViewController *sVC = [[SetingsViewController alloc] init];
    [self.navigationController pushViewController:sVC animated:YES];
}

#pragma mark
#pragma mark Camera Functions

-(void)initiateCamera
{
    if (![UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
    {
        // There is not a camera on this device, so notify watch about that
        [remote.message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:255]}];
        return;
    }
    
    if(self.imagePickerController == nil)
    {
        UIImagePickerController *imagePickerController = [[UIImagePickerController alloc] init];
        imagePickerController.modalPresentationStyle = UIModalPresentationCurrentContext;
        imagePickerController.sourceType = UIImagePickerControllerSourceTypeCamera;
        imagePickerController.delegate = self;
        
        imagePickerController.showsCameraControls = NO;
        
        self.imagePickerController = imagePickerController;
        [self presentViewController:self.imagePickerController animated:YES completion:nil];
    }
    
}

-(void) removeCameraWindow
{
    if(self.imagePickerController != nil)
    {
        self.imagePickerController = nil;
        [self dismissViewControllerAnimated:YES completion:NULL];
    }
}
-(void)operateCamera:(NSInteger)operationKey
{
    //Up>Rear/front 64/-64
    //select>take picture 128
    //Down>flash on/off/auto 1/-1/0
    //255 - error code
    // 128 capture successfull code
    
    switch (operationKey) {
            
        case 0:
        {
            [self removeCameraWindow];
            break;
        }

        case 1:
        {

            [self initiateCamera];
            break;
        }
        case 32:
        {
            [self setFlashMode];
            break;
        }
        case 64:
        {
            [self setCameraDeviceFrontOrRear];
            break;
        }
        case 127:
        {
            float timeout = [[[NSUserDefaults standardUserDefaults] objectForKey:CAPTURE_DELAY_KEY] floatValue];
            if(timeout <= 0.1f) timeout = 0.1f;
            [self performSelector:@selector(takePhoto) withObject:nil afterDelay:timeout];
            break;
        }
            
        default:
            break;
    }
}


-(void)setFlashMode
{
    if ( [UIImagePickerController isFlashAvailableForCameraDevice:self.imagePickerController.cameraDevice] )
    {
        if (self.imagePickerController.cameraFlashMode == UIImagePickerControllerCameraFlashModeAuto)
        {
            [self.imagePickerController setCameraFlashMode:UIImagePickerControllerCameraFlashModeOn];
            [remote.message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:2]}];
        }
        else if (self.imagePickerController.cameraFlashMode == UIImagePickerControllerCameraFlashModeOn)
        {
            [self.imagePickerController setCameraFlashMode:UIImagePickerControllerCameraFlashModeOff];
            [remote.message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:1]}];
        }
        else if (self.imagePickerController.cameraFlashMode == UIImagePickerControllerCameraFlashModeOff)
        {
            [self.imagePickerController setCameraFlashMode:UIImagePickerControllerCameraFlashModeAuto];
            [remote.message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:0]}];
        }
    }
    else
    {
        [remote.message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:3]}];
    }
    
}
-(void)setCameraDeviceFrontOrRear
{
    
    if(self.imagePickerController.cameraDevice == UIImagePickerControllerCameraDeviceRear)
    {
        if([UIImagePickerController isCameraDeviceAvailable: UIImagePickerControllerCameraDeviceFront])
        {
            [self.imagePickerController setCameraDevice:UIImagePickerControllerCameraDeviceFront];
            [remote.message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:65]}];
        }
        else
        {
            [remote.message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:64]}];
        }
        
    }
    else
    {
        [self.imagePickerController setCameraDevice:UIImagePickerControllerCameraDeviceRear];
        [remote.message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:64]}];
    }
    
}



- (void)takePhoto
{
    [self.imagePickerController takePicture];
}

#pragma mark - UIImagePickerControllerDelegate

// This method is called when an image has been chosen from the library or taken from the camera.
- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    UIImage *image = [info valueForKey:UIImagePickerControllerOriginalImage];
    
    UIImageWriteToSavedPhotosAlbum(image, self, @selector(image:didFinishSavingWithError:contextInfo:), NULL);
    
}

- (void)image:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo
{
    if(error)
    {
        //send error code
        [remote.message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:255]}];
    }
    else
    {
        //tell watch it was a success
        [remote.message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:127]}];
    }
}
#pragma mark
#pragma View and Memory


-(void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    // Check whether we are authorized to access Calendar
    [remote checkEventStoreAccessForCalendar:NO];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
