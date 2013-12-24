#ifndef ipod_common_h
#define ipod_common_h

#include "pebble.h"

#define IPOD_RECONNECT_KEY 0xFEFF
#define IPOD_REQUEST_LIBRARY_KEY 0xFEFE
#define IPOD_REQUEST_OFFSET_KEY 0xFEFB
#define IPOD_LIBRARY_RESPONSE_KEY 0xFEFD
#define IPOD_NOW_PLAYING_KEY 0xFEFA
#define IPOD_REQUEST_PARENT_KEY 0xFEF9
#define IPOD_PLAY_TRACK_KEY 0xFEF8
#define IPOD_NOW_PLAYING_RESPONSE_TYPE_KEY 0xFEF7
#define IPOD_ALBUM_ART_KEY 0xFEF6
#define IPOD_STATE_CHANGE_KEY 0xFEF5
#define IPOD_CURRENT_STATE_KEY 0xFEF4
#define IPOD_SEQUENCE_NUMBER_KEY 0xFEF3
	
#define FIND_PHONE_PLAY_SOUND_KEY 0xFEE1
#define GET_BATTERY_STATUS_KEY 0xFEE2
	
#define GET_NOTES_LIST_KEY 0xFEE3
#define GET_SPECIFIC_NOTE_KEY 0xFEE4
#define GET_EVENTS_REMINDERS_KEY 0xFEE5
#define CAMERA_CAPTURE_KEY 0xFEE6
	
#define MAX_INCOMING_SIZE 105 

typedef enum {
    MPMediaGroupingTitle,
    MPMediaGroupingAlbum,
    MPMediaGroupingArtist,
    MPMediaGroupingAlbumArtist,
    MPMediaGroupingComposer,
    MPMediaGroupingGenre,
    MPMediaGroupingPlaylist,
    MPMediaGroupingPodcastTitle,
} MPMediaGrouping;

typedef enum {
    MPMusicPlaybackStateStopped,
    MPMusicPlaybackStatePlaying,
    MPMusicPlaybackStatePaused,
    MPMusicPlaybackStateInterrupted,
    MPMusicPlaybackStateSeekingForward,
    MPMusicPlaybackStateSeekingBackward
} MPMusicPlaybackState;

typedef enum {
    MPMusicRepeatModeDefault, // the user's preference for repeat mode
    MPMusicRepeatModeNone,
    MPMusicRepeatModeOne,
    MPMusicRepeatModeAll
} MPMusicRepeatMode;

typedef enum {
    MPMusicShuffleModeDefault, // the user's preference for shuffle mode
    MPMusicShuffleModeOff,
    MPMusicShuffleModeSongs,
    MPMusicShuffleModeAlbums
} MPMusicShuffleMode;

typedef enum {
    NowPlayingTitle,
    NowPlayingArtist,
    NowPlayingAlbum,
    NowPlayingTitleArtist,
    NowPlayingNumbers,
} NowPlayingType;

AppMessageResult ipod_message_out_get(DictionaryIterator **iter_out);
void reset_sequence_number();
char* itoa(int i);
//extern AppContextRef g_app_context;
void itoa1(int i, char* a);
void itoa2(int i, char* a);
time_t get_pebble_time();
void format_lap(time_t time, char* buffer);
#endif
