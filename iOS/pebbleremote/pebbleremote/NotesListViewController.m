//
//  NotesListViewController.m
//  pebbleremote
//
//  Created by Benzamin on 8/28/13.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import "NotesListViewController.h"
#import "NoteDetailsViewController.h"

@interface NotesListViewController ()

@end

@implementation NotesListViewController

@synthesize displayedObjects, tableView;

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
    [self setTitle:@"Pebble Notes"];
    
    [[self tableView] setRowHeight:54.0];
    
    //  Configure the Edit button
    //[[self navigationItem] setLeftBarButtonItem:[self editButtonItem]];
    
    //  Configure the Add button
    UIBarButtonItem *addButton = [[UIBarButtonItem alloc]
                                  initWithBarButtonSystemItem:UIBarButtonSystemItemAdd
                                  target:self
                                  action:@selector(add)];
    
    [[self navigationItem] setRightBarButtonItem:addButton];
    [[self navigationItem] setRightBarButtonItems:[NSArray arrayWithObjects:[self editButtonItem],addButton, nil] animated:YES];
    
    [self refreshNoteArray];
}

//  Lazily initializes array of displayed objects
//
- (void)refreshNoteArray
{
    [displayedObjects removeAllObjects];
    displayedObjects = nil;
    NSArray *notesArray = [[NSUserDefaults standardUserDefaults] arrayForKey:NOTE_KEY];
    displayedObjects = [[NSMutableArray alloc] initWithArray:notesArray];
    
}


#pragma mark -
#pragma mark UIViewController

//  Override inherited method to automatically refresh table view's data
//
- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [self refreshNoteArray];
    [[self tableView] reloadData];
}

-(void)viewWillDisappear:(BOOL)animated
{
    [[NSUserDefaults standardUserDefaults] setObject:displayedObjects forKey:NOTE_KEY];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

//  Override inherited method to enable/disable Edit button
//
- (void)setEditing:(BOOL)editing
          animated:(BOOL)animated
{
    [super setEditing:editing
             animated:animated];
    [[self tableView] setEditing:editing animated:animated];
    
    UIBarButtonItem *editButton = [[[self navigationItem] rightBarButtonItems] objectAtIndex:1];
    //UIBarButtonItem *editButton = [[self navigationItem] leftBarButtonItem];
    [editButton setEnabled:!editing];
    
    
}

- (void)add
{
    NoteDetailsViewController *controller = [[NoteDetailsViewController alloc] init];
    
    controller.index = [displayedObjects count];
    
    [[self navigationController] pushViewController:controller
                                           animated:YES];
}


#pragma mark -
#pragma mark UITableViewDelegate Protocol
//
//  The table view's delegate is notified of runtime events, such as when
//  the user taps on a given row, or attempts to add, remove or reorder rows.

//  Notifies the delegate when the user selects a row.
//
- (void)tableView:(UITableView *)tableView
didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    NoteDetailsViewController *controller = [[NoteDetailsViewController alloc] init];
    
    controller.index = [indexPath row];
    
    [[self navigationController] pushViewController:controller
                                           animated:YES];
}

#pragma mark -
#pragma mark UITableViewDataSource Protocol

//  Returns the number of rows in the current section.
//
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[self displayedObjects] count];
}

//  Return YES to allow the user to reorder table view rows
//
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    return YES;
}

//  Invoked when the user drags one of the table view's cells. Mirror the
//  change in the user interface by updating the array of displayed objects.
//
- (void) tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)sourceIndexPath toIndexPath:(NSIndexPath *)targetIndexPath
{
    NSUInteger sourceIndex = [sourceIndexPath row];
    NSUInteger targetIndex = [targetIndexPath row];
    
    if (sourceIndex != targetIndex)
    {
        [[self displayedObjects] exchangeObjectAtIndex:sourceIndex
                                     withObjectAtIndex:targetIndex];
    }
}

//  Update array of displayed objects by inserting/removing objects as necessary.
//
- (void) tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete)
    {
        [[self displayedObjects] removeObjectAtIndex:[indexPath row]];
        
        //  Animate deletion
        NSArray *indexPaths = [NSArray arrayWithObject:indexPath];
        [[self tableView] deleteRowsAtIndexPaths:indexPaths
                                withRowAnimation:UITableViewRowAnimationFade];
    }

}

// Return a cell containing the text to display at the provided row index.
//
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [[self tableView] dequeueReusableCellWithIdentifier:@"MyCell"];
    
    if (cell == nil)
    {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle
                                      reuseIdentifier:@"MyCell"];
        
        [cell setAccessoryType:UITableViewCellAccessoryDisclosureIndicator];
        
        UIFont *titleFont = [UIFont fontWithName:@"Georgia-BoldItalic" size:18.0];
        [[cell textLabel] setFont:titleFont];
        
        UIFont *detailFont = [UIFont fontWithName:@"Georgia" size:16.0];
        [[cell detailTextLabel] setFont:detailFont];
    }
    
    NSUInteger index = [indexPath row];
    
    NSString *note = [NSString stringWithFormat:@"%@", [displayedObjects objectAtIndex:index]];
    NSString *title = nil;
    NSString *detailText = nil;
    NSRange breakString = [note rangeOfString:@"#^#"];
    if(breakString.location != NSNotFound) {
        title = [note substringToIndex:breakString.location];
        detailText = [note substringFromIndex:breakString.location];
        detailText = [detailText stringByReplacingOccurrencesOfString:@"#^#" withString:@""];
    }
    [[cell textLabel] setText:(title == nil || [title length] < 1 ? @"?" : title)];
        
    [[cell detailTextLabel] setText:detailText];

    return cell;
}




- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
