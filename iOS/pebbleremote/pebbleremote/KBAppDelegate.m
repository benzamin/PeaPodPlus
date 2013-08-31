//
//  KBAppDelegate.m
//  pebbleremote
//
//  Created by Katharine Berry on 25/05/2013.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import "KBAppDelegate.h"
#import "KBViewController.h"
#import "KBiPodRemote.h"

@implementation KBAppDelegate

@synthesize navigationController, remote;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    CGRect rect = [[UIScreen mainScreen] bounds];
    UIWindow *window = [[UIWindow alloc] initWithFrame:rect];
    [self setWindow:window];
    
    //  By default, UITableViewController populates its own 'view' property
    //  with a new instance of UITableView, and makes itself the table view
    //  instance's data source and delegate.
    //
    //MyListController *tableViewController = [[MyListController alloc]
     //                                        initWithStyle:UITableViewStylePlain];
    
    KBViewController *kbVC = [[KBViewController alloc] initWithNibName:@"KBViewController" bundle:nil];
    
    //  UINavigationController is initialized with the view controller that
    //  manages its initial, or root, view. The navigation controller sets its
    //  own view property to point to the view of the view controller at the
    //  top of its internal stack of view controllers.
    //
    UINavigationController *navController = [[UINavigationController alloc]
                                             initWithRootViewController:kbVC];
    
    [self setNavigationController:navController];
    [self.window setRootViewController:self.navigationController];
    
    [window addSubview:[navController view]];
    [window makeKeyAndVisible];
    
    remote = [[KBiPodRemote alloc] initWithViewControllerReference:kbVC];
    kbVC.remote = remote;
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
