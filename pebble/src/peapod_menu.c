#include "peapod_menu.h"
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "common.h"
#include "library_menus.h"
#include "now_playing.h"
#include "ipod.h"

static Window window;

static void open_now_playing(int index, void* context);
static void open_artist_list(int index, void* context);
static void open_album_list(int index, void* context);
static void open_playlist_list(int index, void* context);
static void open_composer_list(int index, void* context);
static void open_genre_list(int index, void* context);
static void window_load(Window* window);
static void window_unload(Window* window);

static SimpleMenuItem main_menu_items[] = {
    {
        .title = "Now Playing",
        .callback = open_now_playing,
    },
    {
        .title = "Playlists",
        .callback = open_playlist_list,
    },
    {
        .title = "Artists",
        .callback = open_artist_list,
    },
    {
        .title = "Albums",
        .callback = open_album_list,
    },
    {
        .title = "Composers",
        .callback = open_composer_list,
    },
    {
        .title = "Genres",
        .callback = open_genre_list,
    },

};

static SimpleMenuSection section = {
    .items = main_menu_items,
    .num_items = ARRAY_LENGTH(main_menu_items),
	.title = "Peapod Menu"
};

static SimpleMenuLayer main_menu_layer;


void peapod_menu_init() {
	
	window_init(&window, "Peapod");
    window_set_window_handlers(&window, (WindowHandlers){
        .unload = window_unload,
        .load = window_load,
    });
    window_stack_push(&window, true);
}

static void window_load(Window* window) {
	simple_menu_layer_init(&main_menu_layer, GRect(0, 0, 144, 152), window, &section, 1, NULL);
    layer_add_child(window_get_root_layer(window), simple_menu_layer_get_layer(&main_menu_layer));
}

static void window_unload(Window* window) {
	set_peapod_running(false);
}

static void open_now_playing(int index, void* context) {
    show_now_playing();
}
static void open_artist_list(int index, void* context) {
    display_library_view(MPMediaGroupingAlbumArtist);
}
static void open_album_list(int index, void* context) {
    display_library_view(MPMediaGroupingAlbum);
}
static void open_playlist_list(int index, void* context) {
    display_library_view(MPMediaGroupingPlaylist);
}
static void open_genre_list(int index, void* context) {
    display_library_view(MPMediaGroupingGenre);
}
static void open_composer_list(int index, void* context) {
    display_library_view(MPMediaGroupingComposer);
}

