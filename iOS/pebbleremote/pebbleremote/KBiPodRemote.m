//
//  KBiPodRemote.m
//  pebbleremote
//
//  Created by Katharine Berry on 25/05/2013.
//  Copyright (c) 2013 Katharine Berry. All rights reserved.
//

#import "KBiPodRemote.h"
#import <PebbleKit/PebbleKit.h>
#import <MediaPlayer/MediaPlayer.h>
#import "KBPebbleImage.h"
#import <EventKit/EventKit.h>
#import <EventKitUI/EventKitUI.h>


#define IPOD_UUID { 0x24, 0xCA, 0x78, 0x2C, 0xB3, 0x1F, 0x49, 0x04, 0x83, 0xE9, 0xCA, 0x51, 0x9C, 0x60, 0x10, 0x97 }

#define IPOD_RECONNECT_KEY @(0xFEFF)
#define IPOD_REQUEST_LIBRARY_KEY @(0xFEFE)
#define IPOD_REQUEST_OFFSET_KEY @(0xFEFB)
#define IPOD_LIBRARY_RESPONSE_KEY @(0xFEFD)
#define IPOD_NOW_PLAYING_KEY @(0xFEFA)
#define IPOD_REQUEST_PARENT_KEY @(0xFEF9)
#define IPOD_PLAY_TRACK_KEY @(0xFEF8)
#define IPOD_NOW_PLAYING_RESPONSE_TYPE_KEY @(0xFEF7)
#define IPOD_ALBUM_ART_KEY @(0xFEF6)
#define IPOD_CHANGE_STATE_KEY @(0xFEF5)
#define IPOD_CURRENT_STATE_KEY @(0xFEF4)
#define IPOD_SEQUENCE_NUMBER_KEY @(0xFEF3)


#define MAX_LABEL_LENGTH 20
#define MAX_RESPONSE_COUNT 15
#define MAX_OUTGOING_SIZE 105 // This allows some overhead.

#define NOTE_MAX_CHARACTER_LENGTH 698
#define EVENTS_MAX_CHARACTER_LENGTH 498


typedef enum {
    NowPlayingTitle,
    NowPlayingArtist,
    NowPlayingAlbum,
    NowPlayingTitleArtist,
    NowPlayingNumbers,
} NowPlayingType;


@interface KBiPodRemote () {
    PBWatch *our_watch;
    MPMusicPlayerController *music_player;
    uint32_t last_sequence_number;
    int eventAndReminderCount;
    KBViewController *kbVC;
}

@property (nonatomic, strong) EKEventStore *eventStore;
@property (nonatomic, strong) NSMutableDictionary *eventsAndRemindersDict;


- (void)setWatch:(PBWatch*)watch;
- (void)watch:(PBWatch*)watch receivedMessage:(NSDictionary*)message;
- (void)watch:(PBWatch*)watch wantsLibraryData:(NSDictionary*)request;
- (void)pushLibraryResults:(NSArray*)results withOffset:(NSInteger)offset toWatch:(PBWatch*)watch type:(MPMediaGrouping)type;
- (void)musicItemChanged:(MPMediaItem*)item;
- (void)pushNowPlayingItemToWatch:(PBWatch*)watch detailed:(BOOL)detailed;
- (void)sendStringArray:(NSArray *)stringArray withKey:(id)key;
-(void)sendString:(NSString*)string withKey:(id)key;
- (void)PushBatteryStatusToWatch;

@end

@implementation KBiPodRemote

@synthesize message_queue;

- (id)initWithViewControllerReference:(KBViewController*)vc
{
    self = [super init];
    if (self) {
        kbVC = vc;
        self.message_queue = [[KBPebbleMessageQueue alloc] init];
        self.eventStore = [[EKEventStore alloc] init];
        eventAndReminderCount = 0;
        self.eventsAndRemindersDict = [[NSMutableDictionary alloc] init];
        [[PBPebbleCentral defaultCentral] setDelegate:self];
        [self setWatch:[[PBPebbleCentral defaultCentral] lastConnectedWatch]];
        music_player = [MPMusicPlayerController iPodMusicPlayer];
        //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(musicItemChanged:) name:MPMusicPlayerControllerNowPlayingItemDidChangeNotification object:music_player];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(musicStateChanged:) name:MPMusicPlayerControllerPlaybackStateDidChangeNotification object:music_player];
        [music_player beginGeneratingPlaybackNotifications];
        
    }
    return self;
}

- (void)musicItemChanged:(MPMediaItem *)item {
    [self pushNowPlayingItemToWatch:our_watch detailed:YES];
}

- (void)musicStateChanged:(MPMusicPlaybackState)state {
    [self pushCurrentStateToWatch:our_watch];
}

- (void)pushCurrentStateToWatch:(PBWatch *)watch {
    uint16_t current_time = (uint16_t)[music_player currentPlaybackTime];
    uint16_t total_time = (uint16_t)[[[music_player nowPlayingItem] valueForProperty:MPMediaItemPropertyPlaybackDuration] doubleValue];
    uint8_t metadata[] = {
        [music_player playbackState],
        [music_player shuffleMode],
        [music_player repeatMode],
        total_time >> 8, total_time & 0xFF,
        current_time >> 8, current_time & 0xFF
    };
    NSLog(@"Current state: %@", [NSData dataWithBytes:metadata length:7]);
    [message_queue enqueue:@{IPOD_CURRENT_STATE_KEY: [NSData dataWithBytes:metadata length:7]}];

}

- (void)pushNowPlayingItemToWatch:(PBWatch *)watch detailed:(BOOL)detailed {
    MPMediaItem *item = [music_player nowPlayingItem];
    NSString *title = [item valueForProperty:MPMediaItemPropertyTitle];
    NSString *artist = [item valueForProperty:MPMediaItemPropertyArtist];
    NSString *album = [item valueForProperty:MPMediaItemPropertyAlbumTitle];
    if(!title) title = @"";
    if(!artist) artist = @"";
    if(!album) album = @"";
    if(!detailed) {
        NSString *value;
        if(!item) {
            value = @"Nothing playing.";
        } else {
            value = [NSString stringWithFormat:@"%@ - %@", title, artist, nil];
        }
        if([value length] > MAX_OUTGOING_SIZE) {
            value = [value substringToIndex:MAX_OUTGOING_SIZE];
        }
        [message_queue enqueue:@{IPOD_NOW_PLAYING_KEY: value, IPOD_NOW_PLAYING_RESPONSE_TYPE_KEY: @(NowPlayingTitleArtist)}];
        NSLog(@"Now playing: %@", value);
    } else {
        NSLog(@"Pushing everything.");
        [self pushCurrentStateToWatch:watch];
        [message_queue enqueue:@{IPOD_NOW_PLAYING_KEY: title, IPOD_NOW_PLAYING_RESPONSE_TYPE_KEY: @(NowPlayingTitle)}];
        [message_queue enqueue:@{IPOD_NOW_PLAYING_KEY: artist, IPOD_NOW_PLAYING_RESPONSE_TYPE_KEY:@(NowPlayingArtist)}];
        [message_queue enqueue:@{IPOD_NOW_PLAYING_KEY: album, IPOD_NOW_PLAYING_RESPONSE_TYPE_KEY: @(NowPlayingAlbum)}];
        
        // Get and send the artwork.
        MPMediaItemArtwork *artwork = [item valueForProperty:MPMediaItemPropertyArtwork];
        if(artwork) {
            UIImage* image = [artwork imageWithSize:CGSizeMake(64, 64)];
            if(!image) {
                [message_queue enqueue:@{IPOD_ALBUM_ART_KEY: [NSNumber numberWithUint8:255]}];
            }
            else {
                NSData *bitmap = [KBPebbleImage ditheredBitmapFromImage:image withHeight:64 width:64];
                size_t length = [bitmap length];
                uint8_t j = 0;
                for(size_t i = 0; i < length; i += MAX_OUTGOING_SIZE-1) {
                    NSMutableData *outgoing = [[NSMutableData alloc] initWithCapacity:MAX_OUTGOING_SIZE];
                    [outgoing appendBytes:&j length:1];
                    [outgoing appendData:[bitmap subdataWithRange:NSMakeRange(i, MIN(MAX_OUTGOING_SIZE-1, length - i))]];
                    [message_queue enqueue:@{IPOD_ALBUM_ART_KEY: outgoing}];
                    ++j;
                }
            }
        }
    }
}

- (void)pebbleCentral:(PBPebbleCentral *)central watchDidConnect:(PBWatch *)watch isNew:(BOOL)isNew {
    [self setWatch:watch];
}

- (void)pebbleCentral:(PBPebbleCentral*)central watchDidDisconnect:(PBWatch*)watch
{
    if([[NSUserDefaults standardUserDefaults] boolForKey:SOUND_ENABLED_KEY])
    {
        AudioServicesPlayAlertSound([[[NSUserDefaults standardUserDefaults] objectForKey:AUDIO_TYPE_DISCONNECT_KEY] intValue]);
    }
    NSLog(@"Watch disconnected %@",[watch name]);
}

- (void)setWatch:(PBWatch *)watch {
    NSLog(@"Have a watch.");
    if(![watch isConnected]) {
        NSLog(@"Not connected.");
        return;
    }
    [watch appMessagesGetIsSupported:^(PBWatch *watch, BOOL isAppMessagesSupported) {
        NSLog(@"Useful watch %@ connected.", [watch name]);
        last_sequence_number = 0;
        our_watch = watch;
        message_queue.watch = watch;
        // Send a message to make sure it's awake and that we have a session.
        uint8_t uuid[] = IPOD_UUID;
        [watch appMessagesSetUUID:[NSData dataWithBytes:uuid length:16]];
        [watch appMessagesPushUpdate:@{IPOD_RECONNECT_KEY: @(1)} onSent:nil];
        [watch appMessagesAddReceiveUpdateHandler:^BOOL(PBWatch *watch, NSDictionary *update) {
            [self watch:watch receivedMessage:update];
            return YES;
        }];
    }];
    if([[NSUserDefaults standardUserDefaults] boolForKey:SOUND_ENABLED_KEY])
    {
        AudioServicesPlayAlertSound([[[NSUserDefaults standardUserDefaults] objectForKey:AUDIO_TYPE_CONNECT_KEY] intValue]);
    }

}

- (void)watch:(PBWatch *)watch receivedMessage:(NSDictionary *)message {
    NSLog(@"Received message: %@", message);
    uint32_t sequence_number = [message[IPOD_SEQUENCE_NUMBER_KEY] uint32Value];
    if(sequence_number == 0xFFFFFFFF) {
        NSLog(@"Reset sequence numbers.");
        last_sequence_number = 0;
    } else {
        if(sequence_number <= last_sequence_number) {
            NSLog(@"Discarding duplicate message.");
            return;
        }
        last_sequence_number = sequence_number;
    }
    if(message[IPOD_PLAY_TRACK_KEY])
    {
        [self watch:watch playTrackFromMessage:message];
    }
    else if(message[IPOD_REQUEST_LIBRARY_KEY])
    {
        if(message[IPOD_REQUEST_PARENT_KEY])
        {
            [self watch:watch wantsSubList:message];
        }
        else
        {
            [self watch:watch wantsLibraryData:message];
        }
    }
    else if(message[IPOD_NOW_PLAYING_KEY])
    {
        [self pushNowPlayingItemToWatch:watch detailed:[message[IPOD_NOW_PLAYING_KEY] boolValue]];
    }
    else if(message[IPOD_CHANGE_STATE_KEY])
    {
        [self changeState:[message[IPOD_CHANGE_STATE_KEY] integerValue]];
    }
    else if(message[FIND_PHONE_PLAY_SOUND_KEY])
    {
        
       NSInteger state = [message[FIND_PHONE_PLAY_SOUND_KEY] integerValue];
        switch(state) {
            case 0:
            {
                if([[NSUserDefaults standardUserDefaults] boolForKey:SOUND_ENABLED_KEY])
                {
                    AudioServicesPlayAlertSound([[[NSUserDefaults standardUserDefaults] objectForKey:AUDIO_TYPE_PING_KEY] intValue]);
                }
                [message_queue enqueue:@{FIND_PHONE_PLAY_SOUND_KEY: [NSNumber numberWithUint8:255]}];
                break;
            }
            case 64:
            {
                 MPMusicPlayerController *app_music_player = [MPMusicPlayerController applicationMusicPlayer];
                [app_music_player setVolume:[app_music_player volume] + 0.0625];
                break;
            }
            case -64:
            {
                MPMusicPlayerController *app_music_player = [MPMusicPlayerController applicationMusicPlayer];
                [app_music_player setVolume:[app_music_player volume] - 0.0625];
                break;
            }
        }
        
        //[self performSelector:@selector(wakemeUp) withObject:nil afterDelay:7];
    }
    else if(message[GET_NOTES_LIST_KEY])
    {
        NSArray *notesArray = [[NSUserDefaults standardUserDefaults] arrayForKey:NOTE_KEY];
        NSMutableArray *titleArray = [[NSMutableArray alloc] init];
        int count = [notesArray count];
        if(count >5) count = 5;
        for (int i = 0; i < count; i++)
        {
            NSString *note = [notesArray objectAtIndex:i];
            NSString *substring = nil;
            NSRange breakString = [note rangeOfString:@"#^#"];
            if(breakString.location != NSNotFound) {
                substring = [note substringToIndex:breakString.location];
            }
            [titleArray addObject:(([substring length] > 15) ? [substring substringToIndex:15] : substring)];//allowing max 15 characters note Title to prevent buffer overflow
        }
        [self sendStringArray:titleArray withKey:GET_NOTES_LIST_KEY];
    }
    else if(message[GET_SPECIFIC_NOTE_KEY])
    {
        NSArray *notesArray = [[NSUserDefaults standardUserDefaults] arrayForKey:NOTE_KEY];

            NSString *substring = [notesArray objectAtIndex:[message[GET_SPECIFIC_NOTE_KEY] integerValue]];
            NSRange breakString = [substring rangeOfString:@"#^#"];
            if(breakString.location != NSNotFound) {
                substring = [substring substringFromIndex:breakString.location];
                 substring = [substring stringByReplacingOccurrencesOfString:@"#^#" withString:@""];
            }
        NSString *finalsString = (([substring length] > NOTE_MAX_CHARACTER_LENGTH) ? [substring substringToIndex:NOTE_MAX_CHARACTER_LENGTH] : substring);
        [self sendString:finalsString withKey:GET_SPECIFIC_NOTE_KEY];
        NSLog(@"Wrote note %d:%@",[message[GET_SPECIFIC_NOTE_KEY] integerValue], substring);
    }
    else if(message[GET_BATTERY_STATUS_KEY])
    {
        [self PushBatteryStatusToWatch];
    }
    else if(message[GET_EVENTS_REMINDERS_KEY])
    {
        [self checkEventStoreAccessForCalendar:YES];
    }
    else if(message[CAMERA_CAPTURE_KEY])
    {
        if(([UIApplication sharedApplication].applicationState == UIApplicationStateInactive) ||
            ([UIApplication sharedApplication].applicationState == UIApplicationStateBackground))
        {
            [message_queue enqueue:@{CAMERA_CAPTURE_KEY: [NSNumber numberWithUint8:255]}];
            return;
        }
        [kbVC initiateCamera];
        [kbVC operateCamera:[message[CAMERA_CAPTURE_KEY] integerValue]];
    }
    
    
    
    //check if this message is not camera capture key, that means user in other page and we can free the imagePickerController
    if(!(message[CAMERA_CAPTURE_KEY]))
    {
        [kbVC removeCameraWindow];
    }
    
    //NSLog(@"%@",[our_watch friendlyDescription]);
    
}
-(void)sendString:(NSString*)string withKey:(id)key
{
    NSData *note = [string dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:YES];
    size_t length = [note length];
    uint8_t j = 0;
    for(size_t i = 0; i < length; i += MAX_OUTGOING_SIZE-1) {
        NSMutableData *outgoing = [[NSMutableData alloc] initWithCapacity:MAX_OUTGOING_SIZE];
        [outgoing appendBytes:&j length:1];
        [outgoing appendData:[note subdataWithRange:NSMakeRange(i, MIN(MAX_OUTGOING_SIZE-1, length - i))]];
        [message_queue enqueue:@{key: outgoing}];
        ++j;
    }
}

- (void)sendStringArray:(NSArray *)stringArray withKey:(id)key
{
    if (!our_watch)
    {
        NSLog(@"No watch to send the Notes List");
    }
    
    NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
    
    [stringArray enumerateObjectsUsingBlock:^(NSString *string, NSUInteger idx, BOOL *stop) {
        dictionary[[NSNumber numberWithInt8:(int8_t)idx]] = string;
    }];
    
    [our_watch appMessagesPushUpdate:dictionary onSent:^(PBWatch *watch, NSDictionary *update, NSError *error) {
        NSLog(@"Dictionary %@ sent with No error:%@", dictionary, error);
        
        if (error)
        {
            NSLog(@"%@",[error description]);
        }
    }];
}

-(void)PushBatteryStatusToWatch
{
    [[UIDevice currentDevice] setBatteryMonitoringEnabled:YES];
    int levelInt = roundf([[UIDevice currentDevice] batteryLevel]*100);
    int8_t batteryLevel = (int8_t)levelInt;
    int8_t state = (int8_t)[[UIDevice currentDevice] batteryState];
    uint8_t metadata[] = {
        batteryLevel,
        state
    };
    NSLog(@"Current battery status: %@ , percent:%zd, State:%zd", [NSData dataWithBytes:metadata length:2], batteryLevel, state);
    [message_queue enqueue:@{GET_BATTERY_STATUS_KEY: [NSData dataWithBytes:metadata length:2]}];
    [[UIDevice currentDevice] setBatteryMonitoringEnabled:NO];
}
/*
-(void)wakemeUp
{
    [our_watch appMessagesLaunch:^(PBWatch *watch, NSError *error) {
        if(error) {
            NSLog(@"Error sending wakeUp Call: %@", error);
        }
    }];
    [self performSelector:@selector(wakemeUp1) withObject:nil afterDelay:7];
}
-(void)wakemeUp1
{
    [message_queue enqueue:@{FIND_PHONE_PLAY_SOUND_KEY: [NSNumber numberWithUint8:255]}];
    NSLog(@"sending vibe");
}
*/
- (void)changeState:(NSInteger)state {
    switch(state) {
        case 0:
            if([music_player playbackState] == MPMusicPlaybackStatePlaying) [music_player pause];
            else [music_player play];
            //[self performSelector:@selector(pushCurrentStateToWatch:) withObject:our_watch afterDelay:0.1];
            break;
        case 1:
            [music_player skipToNextItem];
            [self pushNowPlayingItemToWatch:our_watch detailed:YES];
            break;
        case -1:
            if([music_player currentPlaybackTime] < 3) {
                [music_player skipToPreviousItem];
                [self pushNowPlayingItemToWatch:our_watch detailed:YES];
            } else {
                [music_player skipToBeginning];
                [self performSelector:@selector(pushCurrentStateToWatch:) withObject:our_watch afterDelay:0.1];
            }
            break;
        case 64:
            [music_player setVolume:[music_player volume] + 0.0625];
            break;
        case -64:
            [music_player setVolume:[music_player volume] - 0.0625];
            break;
    }
}

- (void)watch:(PBWatch*)watch playTrackFromMessage:(NSDictionary *)message {
    MPMediaItemCollection *queue = [self getCollectionFromMessage:message][0];
    MPMediaItem *track = [queue items][[message[IPOD_PLAY_TRACK_KEY] int16Value]];
    [music_player setQueueWithItemCollection:queue];
    [music_player setNowPlayingItem:track];
    [music_player play];
    [self pushNowPlayingItemToWatch:watch detailed:YES];
}

- (void)watch:(PBWatch *)watch wantsLibraryData:(NSDictionary *)request {
    NSUInteger request_type = [request[IPOD_REQUEST_LIBRARY_KEY] unsignedIntegerValue];
    NSUInteger offset = [request[IPOD_REQUEST_OFFSET_KEY] integerValue];
    MPMediaQuery *query = [[MPMediaQuery alloc] init];
    [query setGroupingType:request_type];
    [query addFilterPredicate:[MPMediaPropertyPredicate predicateWithValue:@(MPMediaTypeMusic) forProperty:MPMediaItemPropertyMediaType]];
    NSArray* results = [query collections];
    [self pushLibraryResults:results withOffset:offset toWatch:watch type:request_type];
}

- (NSArray*)getCollectionFromMessage:(NSDictionary*)request {
    // Find what we're subsetting by iteratively grabbing the sets.
    MPMediaItemCollection *collection = nil;
    MPMediaGrouping parent_type;
    uint16_t parent_index;
    NSString *persistent_id;
    NSString *id_prop;
    NSData *data = request[IPOD_REQUEST_PARENT_KEY];
    uint8_t *bytes = (uint8_t*)[data bytes];
    for(uint8_t i = 0; i < bytes[0]; ++i) {
        parent_type = bytes[i*3+1];
        parent_index = *(uint16_t*)&bytes[i*3+2];
        NSLog(@"Parent type: %d", parent_type);
        NSLog(@"Parent index: %d", parent_index);
        NSLog(@"i: %d", i);
        MPMediaQuery *query = [[MPMediaQuery alloc] init];
        [query setGroupingType:parent_type];
        [query addFilterPredicate:[MPMediaPropertyPredicate predicateWithValue:@(MPMediaTypeMusic) forProperty:MPMediaItemPropertyMediaType]];
        if(collection) {
            [query addFilterPredicate:[MPMediaPropertyPredicate predicateWithValue:persistent_id forProperty:id_prop]];
        }
        if(parent_index >= [[query collections] count]) {
            NSLog(@"Out of bounds: %d", parent_index);
            return nil;
        }
        collection = [query collections][parent_index];
        id_prop = [MPMediaItem persistentIDPropertyForGroupingType:parent_type];
        persistent_id = [[collection representativeItem] valueForProperty:id_prop];
    }
    
    // Complete the lookup
    NSUInteger request_type = [request[IPOD_REQUEST_LIBRARY_KEY] unsignedIntegerValue];
    if(request_type == MPMediaGroupingTitle) {
        return @[collection];
    } else {
        NSLog(@"Got persistent ID: %@", persistent_id);
        MPMediaQuery *query = [[MPMediaQuery alloc] init];
        [query setGroupingType:request_type];
        [query addFilterPredicate:[MPMediaPropertyPredicate predicateWithValue:persistent_id forProperty:id_prop]];
        [query addFilterPredicate:[MPMediaPropertyPredicate predicateWithValue:@(MPMediaTypeMusic) forProperty:MPMediaItemPropertyMediaType]];
        return [query collections];
    }
}

- (void)watch:(PBWatch*)watch wantsSubList:(NSDictionary*)request {
    NSArray *results = [self getCollectionFromMessage:request];
    MPMediaGrouping request_type = [request[IPOD_REQUEST_LIBRARY_KEY] integerValue];
    uint16_t offset = [request[IPOD_REQUEST_OFFSET_KEY] uint16Value];
    if(request_type == MPMediaGroupingTitle) {
        results = [results[0] items];
    }
    [self pushLibraryResults:results withOffset:offset toWatch:watch type:request_type];
}

- (void)pushLibraryResults:(NSArray *)results withOffset:(NSInteger)offset toWatch:(PBWatch *)watch type:(MPMediaGrouping)type {
    NSArray* subset;
    if(offset < [results count]) {
        NSInteger count = MAX_RESPONSE_COUNT;
        if([results count] <= offset + MAX_RESPONSE_COUNT) {
            count = [results count] - offset;
        }
        subset = [results subarrayWithRange:NSMakeRange(offset, count)];
    }
    NSMutableData *result = [[NSMutableData alloc] init];
    // Response format: header of one byte containing library data type, two bytes containing
    // the total number of results, and two bytes containing our current offset. Little endian.
    // This is followed by a sequence of entries, which consist of one length byte followed by UTF-8 data
    // (pascal style)
    uint8_t type_byte = (uint8_t)type;
    uint16_t metabytes[] = {[results count], offset};
    // Include the type of library
    [result appendBytes:&type_byte length:1];
    [result appendBytes:metabytes length:4];
    for (MPMediaItemCollection* item in subset) {
        NSString *value;
        if([item isKindOfClass:[MPMediaPlaylist class]]) {
            value = [item valueForProperty:MPMediaPlaylistPropertyName];
        } else {
            value = [[item representativeItem] valueForProperty:[MPMediaItem titlePropertyForGroupingType:type]];
        }
        if([value length] > MAX_LABEL_LENGTH) {
            value = [value substringToIndex:MAX_LABEL_LENGTH];
        }
        NSData *value_data = [value dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:YES];
        uint8_t length = [value_data length];
        if([result length] + length > MAX_OUTGOING_SIZE) break;
        [result appendBytes:&length length:1];
        [result appendData:value_data];
        NSLog(@"Value: %@", value);
    }
    // Send it!
    [watch appMessagesPushUpdate:@{IPOD_LIBRARY_RESPONSE_KEY: result} onSent:^(PBWatch *watch, NSDictionary *update, NSError *error) {
        if(error) {
            NSLog(@"Error sending library response: %@", error);
        }
    }];
    NSLog(@"Sent message: %@", result);
}


#pragma mark -
#pragma mark Access Calendar

// Check the authorization status of our application for Calendar
-(void)checkEventStoreAccessForCalendar:(BOOL)push
{
    EKAuthorizationStatus status = [EKEventStore authorizationStatusForEntityType:EKEntityTypeEvent];
    
    switch (status)
    {
            // Update our UI if the user has granted access to their Calendar
        case EKAuthorizationStatusAuthorized:
        {
            [self accessGrantedForCalendar:push];
            break;
        }
        // Prompt the user for access to Calendar if there is no definitive answer
        case EKAuthorizationStatusNotDetermined:
        {
            [self requestCalendarAccess:push];
            break;
        }
        // Display a message if the user has denied or restricted access to Calendar
        case EKAuthorizationStatusDenied:
        case EKAuthorizationStatusRestricted:
        {
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Privacy Warning" message:@"Permission was not granted for Calendar, check Settings>Privacy>Calender/Reminder."
                                                           delegate:nil
                                                  cancelButtonTitle:@"OK"
                                                  otherButtonTitles:nil];
            [alert show];
            if(push)[self sendString:@"Permission was not granted for Calendar, go to Settings>Privacy>Calender and Settings>Privacy>Rreminders to grant Persission. Also check for Settings>Mail,Contacts,Calenders>Calenders section to set the Default Calendar" withKey:GET_EVENTS_REMINDERS_KEY];
        }
            break;
        default:
            break;
    }
}


// Prompt the user for access to their Calendar
-(void)requestCalendarAccess:(BOOL)push
{
    [self.eventStore requestAccessToEntityType:EKEntityTypeEvent completion:^(BOOL granted, NSError *error)
     {
         if (granted)
         {
             KBiPodRemote * __weak weakSelf = self;
             // Let's ensure that our code will be executed from the main queue
             dispatch_async(dispatch_get_main_queue(), ^{
                 // The user has granted access to their Calendar; let's populate our UI with all events occuring in the next 24 hours.
                 [weakSelf accessGrantedForCalendar:push];
             });
         }
         else
         {
             if(push)[self sendString:@"Permission was not granted for Calendar, go to Settings>Privacy>Calender and Settings>Privacy>Rreminders to grant Persission. Also check for Settings>Mail,Contacts,Calenders>Calenders section to set the Default Calendar" withKey:GET_EVENTS_REMINDERS_KEY];
         }
     }];
}


// This method is called when the user has granted permission to Calendar
-(void)accessGrantedForCalendar:(BOOL)push
{
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setTimeStyle:NSDateFormatterShortStyle];
    [dateFormatter setDateStyle:NSDateFormatterMediumStyle];
    NSLocale *frLocale = [NSLocale currentLocale];
    [dateFormatter setLocale:frLocale];
    
    [dateFormatter setDoesRelativeDateFormatting:YES];
    //[format setDateFormat:@"MMM dd, yyyy; HH:mm"];//TODO: can be customizable throu user settings
    
    // Fetch all events happening in the next 24 hours and put them into eventsList
    NSMutableArray * eventsList = [self fetchEvents];
    for(EKEvent *ekEv in eventsList)
    {
        NSString * event =  [NSString stringWithFormat:@"Event: %@%@\nTime: %@", ekEv.title, (ekEv.location ? [NSString stringWithFormat:@" in %@.", ekEv.location] : @"."), [dateFormatter stringFromDate:ekEv.startDate]];
        [self.eventsAndRemindersDict setObject:event forKey:ekEv.startDate];
        
    }
    eventAndReminderCount ++;
    if(push)[self sortAndSendEventsReminders];

    //Lets handle reminders now
    __weak KBiPodRemote *weakSelf = self;
    
    [self.eventStore requestAccessToEntityType:EKEntityTypeReminder completion:^(BOOL granted, NSError *error)
     {
         if (!granted)
         {
             if(push)[self sendString:@"Permission was not granted for Calendar, go to Settings>Privacy>Calender and Settings>Privacy>Rreminders to grant Persission. Also check for Settings>Mail,Contacts,Calenders>Calenders section to set the Default Calendar" withKey:GET_EVENTS_REMINDERS_KEY];
             return;
         }
         if (error)
         {
             if(push)[self sendString:@"Error getting Calender events and Reminders. Please check Settings>Privacy>Calender and Settings>Privacy>Rreminders to grant Persission. Also check for Settings>Mail,Contacts,Calenders>Calenders section to set the Default Calendar" withKey:GET_EVENTS_REMINDERS_KEY];
             return;
         }
         
         [weakSelf.eventStore fetchRemindersMatchingPredicate:[weakSelf.eventStore predicateForIncompleteRemindersWithDueDateStarting:nil
                                                            ending:nil
                                                            calendars:@[[weakSelf.eventStore defaultCalendarForNewReminders]]]
                                                   completion:^(NSArray *reminders)
          {
              dispatch_async(dispatch_get_main_queue(), ^{
                  if (reminders.count == 0)
                  {
                      NSLog(@"No reminders found");
                      return;
                  }
                  
                  NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
                  [dateFormatter setTimeStyle:NSDateFormatterShortStyle];
                  [dateFormatter setDateStyle:NSDateFormatterMediumStyle];
                  NSLocale *frLocale = [NSLocale currentLocale];
                  [dateFormatter setLocale:frLocale];
                  
                  [dateFormatter setDoesRelativeDateFormatting:YES];
                  //[format setDateFormat:@"MMM dd, yyyy; HH:mm"];//TODO: can be customizable throu user settings

                  for(EKReminder *ev in reminders)
                  {
                      NSDate *reminderDate;
                      NSString *dateString;
                      if(ev.startDateComponents)
                      {
                          reminderDate = [[NSCalendar currentCalendar] dateFromComponents:ev.startDateComponents];
                          dateString = [dateFormatter stringFromDate:reminderDate];
                      }
                      else
                      {
                          dateString = @"Not set";
                      }
                      NSString * reminder =  [NSString stringWithFormat:@"Reminder: %@%@\nTime: %@", ev.title, (ev.location ? [NSString stringWithFormat:@" in %@.", ev.location] : @"."), dateString];
                      [self.eventsAndRemindersDict setObject:reminder forKey:(ev.startDateComponents ? reminderDate : [[NSDate date] dateByAddingTimeInterval: ((31*24*3600) + rand())])];
                  }
                  eventAndReminderCount ++;
                  if(push)[self sortAndSendEventsReminders];
                  
                  
              });
          }];
     }];
    
    
  
}
- (void)setDoesRelativeDateFormatting:(BOOL)b
{
    
}
-(void)sortAndSendEventsReminders
{
    if(eventAndReminderCount < 2)
    {
        return;
    }
     //Finally write the response to the watch
     NSArray * keys = [self.eventsAndRemindersDict allKeys];
     
     // sort it
     NSArray * sorted_keys = [keys sortedArrayUsingSelector:@selector(compare:)];
     
    NSString *responseString = [[NSString alloc] init];
     // now, access the values in order
     for (NSDate * key in sorted_keys)
     {
         // get value
         NSString * value = [NSString stringWithFormat:@"______________\n%@\n",[self.eventsAndRemindersDict objectForKey:key]];
         responseString = [responseString stringByAppendingString:value];
     }
    NSString *finalsString = (([responseString length] > EVENTS_MAX_CHARACTER_LENGTH) ? [responseString substringToIndex:EVENTS_MAX_CHARACTER_LENGTH] : responseString);
    [self sendString:finalsString withKey:GET_EVENTS_REMINDERS_KEY];
    eventAndReminderCount = 0;
    [self.eventsAndRemindersDict removeAllObjects];
    
     NSLog(@"\n%@",responseString);
    
    
}

// Fetch all events happening in the next 24 hours
- (NSMutableArray *)fetchEvents
{
    NSDate *startDate = [NSDate date];
    
    //Create the end date components
    NSDateComponents *tomorrowDateComponents = [[NSDateComponents alloc] init];
    tomorrowDateComponents.day = 30;
	
    NSDate *endDate = [[NSCalendar currentCalendar] dateByAddingComponents:tomorrowDateComponents
                                                                    toDate:startDate
                                                                   options:0];
	// We will only search the default calendar for our events
	NSArray *calendarArray = [NSArray arrayWithObjects:self.eventStore.defaultCalendarForNewEvents, self.eventStore.defaultCalendarForNewReminders,nil];
    
    // Create the predicate
	NSPredicate *predicate = [self.eventStore predicateForEventsWithStartDate:startDate
                                                                      endDate:endDate
                                                                    calendars:calendarArray];
	
	// Fetch all events that match the predicate
	NSMutableArray *events = [NSMutableArray arrayWithArray:[self.eventStore eventsMatchingPredicate:predicate]];
    
	return events;
    
}

@end
