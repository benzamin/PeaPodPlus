/*#include "ping_phone.h"
#include "pebble_fonts.h"
#include "pebble_app.h"
#include "common.h"

static Window window;
static ActionBarLayer action_bar;
TextLayer Click_to_play_layer;
TextLayer Sound_Played_layer;

// Action bar icons
static HeapBitmap icon_play;
static HeapBitmap icon_volume_up;
static HeapBitmap icon_volume_down;

static AppMessageCallbacksNode app_callbacks;

static void click_config_provider(ClickConfig **config, void *context);
static void window_unload(Window* window);
static void window_load(Window* window);
static void clicked_up(ClickRecognizerRef recognizer, void *context);
static void clicked_select(ClickRecognizerRef recognizer, void *context);
static void long_clicked_select(ClickRecognizerRef recognizer, void *context);
static void clicked_down(ClickRecognizerRef recognizer, void *context);

static void send_play_sound_notify(int8_t value);

static void app_in_received(DictionaryIterator *received, void *context);
static void app_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context);


void show_ping_phone() {
    window_init(&window, "Ping My Phone");
    window_set_window_handlers(&window, (WindowHandlers){
        .unload = window_unload,
        .load = window_load,
    });
    window_stack_push(&window, true);
}

static void window_load(Window* window) {
    // Load bitmaps for action bar icons.
    heap_bitmap_init(&icon_play, RESOURCE_ID_ICON_PLAY);
	heap_bitmap_init(&icon_volume_up, RESOURCE_ID_ICON_VOLUME_UP);
    heap_bitmap_init(&icon_volume_down, RESOURCE_ID_ICON_VOLUME_DOWN);
	
    // Action bar
    action_bar_layer_init(&action_bar);
    action_bar_layer_add_to_window(&action_bar, window);
    action_bar_layer_set_click_config_provider(&action_bar, click_config_provider);

    // Set default icon set.
    action_bar_layer_set_icon(&action_bar, BUTTON_ID_SELECT, &icon_play.bmp);
	    action_bar_layer_set_icon(&action_bar, BUTTON_ID_DOWN, &icon_volume_down.bmp);
    action_bar_layer_set_icon(&action_bar, BUTTON_ID_UP, &icon_volume_up.bmp);
    
    // Text labels
	text_layer_init(&Click_to_play_layer, GRect(10, 65, 100 , 20 
    text_layer_set_text(&Click_to_play_layer, "Press to Play >");
    text_layer_set_text_alignment(&Click_to_play_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), &Click_to_play_layer.layer);
	
	text_layer_init(&Sound_Played_layer, GRect(10, 100, 100));
    text_layer_set_text(&Sound_Played_layer, "");
    text_layer_set_text_alignment(&Sound_Played_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), &Sound_Played_layer.layer);

    
    app_callbacks = (AppMessageCallbacksNode){
        .callbacks = {
            .in_received = app_in_received,
			.out_failed = app_out_failed,
        }
    };
    app_message_register_callbacks(&app_callbacks);
}

static void window_unload(Window* window) {
    action_bar_layer_remove_from_window(&action_bar);
    app_message_deregister_callbacks(&app_callbacks);
    
    // deinit action bar icons
    heap_bitmap_deinit(&icon_play);
	heap_bitmap_deinit(&icon_volume_up);
    heap_bitmap_deinit(&icon_volume_down);

}

static void click_config_provider(ClickConfig **config, void* context) {
    config[BUTTON_ID_DOWN]->click.handler = clicked_down;
    config[BUTTON_ID_UP]->click.handler = clicked_up;
    config[BUTTON_ID_SELECT]->click.handler = clicked_select;
    config[BUTTON_ID_SELECT]->long_click.handler = long_clicked_select;
}

static void clicked_up(ClickRecognizerRef recognizer, void *context) {
	send_play_sound_notify(64);
}
static void clicked_select(ClickRecognizerRef recognizer, void *context) {
    send_play_sound_notify(0);
	text_layer_set_text(&Sound_Played_layer, "");
}
static void clicked_down(ClickRecognizerRef recognizer, void *context) {
	send_play_sound_notify(-64);
}
static void long_clicked_select(ClickRecognizerRef recognizer, void *context) {

}

static void send_play_sound_notify(int8_t val) {
    DictionaryIterator *iter;
    ipod_message_out_get(&iter);
    if(!iter) return;
    dict_write_int8(iter, FIND_PHONE_PLAY_SOUND_KEY, val);
    app_message_out_send();
    app_message_out_release();
}

static void app_in_received(DictionaryIterator *received, void* context) {
    Tuple* tuple = dict_find(received, FIND_PHONE_PLAY_SOUND_KEY);
    if(tuple) 
	{
		text_layer_set_text(&Sound_Played_layer, "Sound Played!");
    }
}
static void app_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
	text_layer_set_text(&Sound_Played_layer, "Not Connected :(");
}
*/