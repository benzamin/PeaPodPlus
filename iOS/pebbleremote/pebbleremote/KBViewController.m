//
//  KBViewController.m
//  pebbleremote
//
//  Created by Katharine Berry on 25/05/2013.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import "KBViewController.h"
#import "KBiPodRemote.h"

@interface KBViewController () {
    KBiPodRemote *remote;
}
@property (nonatomic) UIImagePickerController *imagePickerController;

@end

@implementation KBViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    remote = [[KBiPodRemote alloc] initWithViewControllerReference:self];
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
    self.imagePickerController = nil;
    [self dismissViewControllerAnimated:YES completion:NULL];
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
            [self takePhoto];
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
