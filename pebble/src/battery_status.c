#include "battery_status.h"
#include "pebble_os.h"
#include "pebble_fonts.h"
#include "pebble_app.h"
#include "common.h"

static Window batteryWindow;

static ActionBarLayer action_bar;
TextLayer Click_to_play_layer;

// Action bar icons declaration
static HeapBitmap icon_play;
static HeapBitmap icon_volume_up;
static HeapBitmap icon_volume_down;

TextLayer percentage_layer;
TextLayer status_layer;
TextLayer text_battery_layer;
Layer battery_layer;
char *percentText;
static int8_t batteryPercent;

static AppMessageCallbacksNode app_callbacks;

static void click_config_provider(ClickConfig **config, void *context);
static void clicked_up(ClickRecognizerRef recognizer, void *context);
static void clicked_select(ClickRecognizerRef recognizer, void *context);
static void long_clicked_select(ClickRecognizerRef recognizer, void *context);
static void clicked_down(ClickRecognizerRef recognizer, void *context);

static void window_unload_b(Window* window);
static void window_load_b(Window* window);

static void app_in_received(DictionaryIterator *received, void *context);
static void app_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context);
void battery_layer_update_callback(Layer *me, GContext* ctx);
static void request_battery_data();
static void send_play_sound_notify(int8_t val);

static const char *batteryStatus[] = {
	"Unknown", 
    "In Use", 
    "Charging", 
    "Fully Charged"
};

void init_battery_status() {
    window_init(&batteryWindow, "Phone Battery Status");
    window_set_window_handlers(&batteryWindow, (WindowHandlers){
        .unload = window_unload_b,
        .load = window_load_b,
    });
    window_stack_push(&batteryWindow, true);
}

static void window_load_b(Window* window) {

	//action bar
	// Load bitmaps for action bar icons.
    heap_bitmap_init(&icon_play, RESOURCE_ID_ICON_PLAY);
	heap_bitmap_init(&icon_volume_up, RESOURCE_ID_ICON_VOLUME_UP);
    heap_bitmap_init(&icon_volume_down, RESOURCE_ID_ICON_VOLUME_DOWN);
	
    // Action bar Init
    action_bar_layer_init(&action_bar);
    action_bar_layer_add_to_window(&action_bar, window);
    action_bar_layer_set_click_config_provider(&action_bar, click_config_provider);

    // Set default icon set.
    action_bar_layer_set_icon(&action_bar, BUTTON_ID_SELECT, &icon_play.bmp);
	    action_bar_layer_set_icon(&action_bar, BUTTON_ID_DOWN, &icon_volume_down.bmp);
    action_bar_layer_set_icon(&action_bar, BUTTON_ID_UP, &icon_volume_up.bmp);
    
    // Text labels
	text_layer_init(&Click_to_play_layer, GRect(0, 67, 120 /* width */, 20 /* height */));
    text_layer_set_text(&Click_to_play_layer, "Ping Phone >");
    text_layer_set_text_alignment(&Click_to_play_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), &Click_to_play_layer.layer);

	
    // Text labels
	text_layer_init(&percentage_layer, GRect(10, 40, 110 /* width */, 27 /* height */));
    //text_layer_set_text(&percentage_layer, "Calculating...");
	text_layer_set_font(&percentage_layer,  fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(&percentage_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), &percentage_layer.layer);
	
	text_layer_init(&status_layer, GRect(10, 105, 110 /* width */, 15 /* height */));
    text_layer_set_text(&status_layer, "Battery Status:");
    text_layer_set_text_alignment(&status_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), &status_layer.layer);

	
	text_layer_init(&text_battery_layer, GRect(0, 120, 120, 28));
	text_layer_set_text_alignment(&text_battery_layer, GTextAlignmentCenter);
	text_layer_set_font(&text_battery_layer,  fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_get_root_layer(window), &text_battery_layer.layer);
	text_layer_set_text(&text_battery_layer, "");


	layer_init(&battery_layer, GRect(0, 0, 144, 55));
	battery_layer.update_proc = &battery_layer_update_callback;
	layer_add_child(window_get_root_layer(window), &battery_layer);

	batteryPercent = 0;
	layer_mark_dirty(&battery_layer);
//	GContext* ctx = app_get_current_graphics_context();
	
		
    app_callbacks = (AppMessageCallbacksNode){
        .callbacks = {
            .in_received = app_in_received,
			.out_failed = app_out_failed,
        }
    };
    app_message_register_callbacks(&app_callbacks);
	
	request_battery_data();
}

static void window_unload_b(Window* window) {
    app_message_deregister_callbacks(&app_callbacks);
	
}

static void click_config_provider(ClickConfig **config, void* context) {
    config[BUTTON_ID_DOWN]->click.handler = clicked_down;
    config[BUTTON_ID_UP]->click.handler = clicked_up;
    config[BUTTON_ID_SELECT]->click.handler = clicked_select;
    config[BUTTON_ID_SELECT]->long_click.handler = long_clicked_select;
}

void battery_layer_update_callback(Layer *me, GContext* ctx) {
	//add outer boundery
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_draw_rect(ctx, GRect(10,5,104,42));
	
	//draw the positive side of battery
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(113,20,5,12), 0, GCornerNone);

	//draw the percent fill
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_context_set_fill_color(ctx, GColorBlack);

	graphics_fill_rect(ctx, GRect(12, 7, batteryPercent, 38), 0, GCornerNone);
	
}
static void clicked_up(ClickRecognizerRef recognizer, void *context) {
	send_play_sound_notify(64);
}
static void clicked_select(ClickRecognizerRef recognizer, void *context) {
    send_play_sound_notify(0);
	text_layer_set_text(&Click_to_play_layer, "");
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

static void request_battery_data() {
    DictionaryIterator *iter;
    ipod_message_out_get(&iter);
    if(!iter) return;
    dict_write_int8(iter, GET_BATTERY_STATUS_KEY, 1);
    app_message_out_send();
    app_message_out_release();
}

static void app_in_received(DictionaryIterator *received, void* context) {
    Tuple* tuple = dict_find(received, GET_BATTERY_STATUS_KEY);
    if(tuple) 
	{		
		batteryPercent = (int8_t)(tuple->value->data[0]);
		layer_mark_dirty(&battery_layer);

		percentText = itoa(batteryPercent);
		percentText = strcat(percentText, "%");
		text_layer_set_text(&percentage_layer, percentText);

		int8_t battery_state = (int8_t)(tuple->value->data[1]);
		text_layer_set_text(&text_battery_layer, batteryStatus[battery_state]);				
		
    }
	
	Tuple* tuple2 = dict_find(received, FIND_PHONE_PLAY_SOUND_KEY);
    if(tuple2) 
	{
		text_layer_set_text(&Click_to_play_layer, "Sound Played!");
    }
}
static void app_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
	text_layer_set_text(&Click_to_play_layer, "Not Connected :(");
	layer_set_hidden(&status_layer.layer, true);
}
