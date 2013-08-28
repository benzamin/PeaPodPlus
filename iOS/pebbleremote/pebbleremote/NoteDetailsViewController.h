//
//  NoteDetailsViewController.h
//  pebbleremote
//
//  Created by Benzamin on 8/29/13.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface NoteDetailsViewController : UIViewController

@property(nonatomic, assign) int index;
@property(nonatomic, strong) IBOutlet UITextField *txtTitle;
@property(nonatomic, strong) IBOutlet UITextView *txtDetails;

@end
