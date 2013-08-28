//
//  NoteDetailsViewController.m
//  pebbleremote
//
//  Created by Benzamin on 8/29/13.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import "NoteDetailsViewController.h"
#import "KBViewController.h"

@interface NoteDetailsViewController ()


@end

@implementation NoteDetailsViewController

@synthesize index, txtDetails, txtTitle;

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
    
    [self setTitle:@"Note Details"];

    // Do any additional setup after loading the view from its nib.
    NSArray *notesArray = [[NSUserDefaults standardUserDefaults] arrayForKey:NOTE_KEY];
    
    if([notesArray count] > [self index])
    {
        NSString *note = [NSString stringWithFormat:@"%@", [notesArray objectAtIndex:index]];
        NSString *title = nil;
        NSString *detailText = nil;
        NSRange breakString = [note rangeOfString:@"#^#"];
        if(breakString.location != NSNotFound) {
            title = [note substringToIndex:breakString.location];
            detailText = [note substringFromIndex:breakString.location];
            detailText = [detailText stringByReplacingOccurrencesOfString:@"#^#" withString:@""];
        }
        [[self txtTitle] setText:title];
        [[self txtDetails] setText:detailText];
    }


}

-(void)viewWillDisappear:(BOOL)animated
{
    NSString *finalString = [NSString stringWithFormat:@"%@#^#%@", [self txtTitle].text, [self txtDetails].text];
    if(![[self txtTitle].text isEqualToString:@""] && ![[self txtDetails].text isEqualToString:@""])
    {
        NSMutableArray *notesArray =  [[NSMutableArray alloc] initWithArray:[[NSUserDefaults standardUserDefaults] arrayForKey:NOTE_KEY]];
        if([notesArray count] > [self index])
        {
            [notesArray replaceObjectAtIndex:index withObject:finalString];

        }
        else
        {
            [notesArray addObject:finalString];
        }
        [[NSUserDefaults standardUserDefaults] setObject:notesArray forKey:NOTE_KEY];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    
}

/*
#define MAX_LENGTH 20

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
    if (textField.text.length >= MAX_LENGTH && range.length == 0)
    {
    	return NO; // return NO to not change text
    }
    else
    {return YES;}
}*/

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
