//
//  NotesListViewController.h
//  pebbleremote
//
//  Created by Benzamin on 8/28/13.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "KBViewController.h"

@interface NotesListViewController : UIViewController <UITabBarControllerDelegate, UITableViewDataSource>

@property (nonatomic, strong) NSMutableArray *displayedObjects;
@property (nonatomic, strong) IBOutlet UITableView *tableView;

@end
