#include "battery_status.h"
#include "common.h"

static Window *batteryWindow;

static ActionBarLayer *action_bar;
TextLayer *Click_to_play_layer;

// Action bar icons declaration

static GBitmap *icon_play = NULL;
static GBitmap *icon_volume_up = NULL;
static GBitmap *icon_volume_down = NULL;

TextLayer *percentage_layer;
TextLayer *status_layer;
TextLayer *text_battery_layer;
Layer *battery_layer;
char *percentText;
static int8_t batteryPercent;

static void click_config_provider(void *ctx);
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
	
	  batteryWindow = window_create();
	  window_set_background_color(batteryWindow, GColorWhite);
	  window_set_fullscreen(batteryWindow, true);
	  window_set_window_handlers(batteryWindow, (WindowHandlers) {
		.load = window_load_b,
		.unload = window_unload_b
	  });
		
	  window_stack_push(batteryWindow, true);
}

static void window_load_b(Window* window) {

	//action bar
	// Load bitmaps for action bar icons.
    icon_play = gbitmap_create_with_resource(RESOURCE_ID_ICON_PLAY);///
    icon_volume_up = gbitmap_create_with_resource(RESOURCE_ID_ICON_VOLUME_UP);///
    icon_volume_down = gbitmap_create_with_resource(RESOURCE_ID_ICON_VOLUME_DOWN);///
	
    // Action bar Init
    action_bar = action_bar_layer_create();
    action_bar_layer_add_to_window(action_bar, window);
    action_bar_layer_set_click_config_provider(action_bar, click_config_provider);

    // Set default icon set.
    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, icon_play);///
    action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, icon_volume_down);///
    action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, icon_volume_up);///
    
    // Text labels
    Click_to_play_layer = text_layer_create(GRect(0, 67, 120 /* width */, 20 /* height */));
    text_layer_set_text(Click_to_play_layer, "Ping Phone >");
    text_layer_set_text_alignment(Click_to_play_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(Click_to_play_layer));

	
    // Text labels
    percentage_layer = text_layer_create(GRect(10, 40, 110 /* width */, 27 /* height */));
    //text_layer_set_text(&percentage_layer, "Calculating...");
	text_layer_set_font(percentage_layer,  fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(percentage_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(percentage_layer));
	
    status_layer = text_layer_create(GRect(10, 105, 110 /* width */, 15 /* height */));
    text_layer_set_text(status_layer, "Battery Status:");
    text_layer_set_text_alignment(status_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(status_layer));

	
    text_battery_layer = text_layer_create(GRect(0, 120, 120, 28));
	text_layer_set_text_alignment(text_battery_layer, GTextAlignmentCenter);
	text_layer_set_font(text_battery_layer,  fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_battery_layer));
	text_layer_set_text(text_battery_layer, "");


    battery_layer = layer_create(GRect(0, 0, 144, 55));
	//battery_layer.update_proc = &battery_layer_update_callback;
	layer_add_child(window_get_root_layer(window), battery_layer);
	layer_set_update_proc(battery_layer, battery_layer_update_callback);

	batteryPercent = 0;
	layer_mark_dirty(battery_layer);
//	GContext* ctx = app_get_current_graphics_context();
	
	app_message_register_inbox_received(app_in_received);
   app_message_register_outbox_failed(app_out_failed);

   const uint32_t inbound_size = 128;
   const uint32_t outbound_size = 256;
   app_message_open(inbound_size, outbound_size);
	
	request_battery_data();
}

static void window_unload_b(Window* window) {
    app_message_deregister_callbacks();
	
}

static void click_config_provider(void* ctx) {
    
	const uint16_t repeat_interval_ms = 50;
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) clicked_select);
	window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler)long_clicked_select, NULL);
  	window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) clicked_up);
  	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) clicked_down);
	
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
	text_layer_set_text(Click_to_play_layer, "");
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
    app_message_outbox_send();

}

static void request_battery_data() {
    DictionaryIterator *iter;
    ipod_message_out_get(&iter);
    if(!iter) return;
    dict_write_int8(iter, GET_BATTERY_STATUS_KEY, 1);
    app_message_outbox_send();
}

static void app_in_received(DictionaryIterator *received, void* context) {
    Tuple* tuple = dict_find(received, GET_BATTERY_STATUS_KEY);
    if(tuple) 
	{		
		batteryPercent = (int8_t)(tuple->value->data[0]);
		layer_mark_dirty(battery_layer);

		percentText = itoa(batteryPercent);
		percentText = strcat(percentText, "%");
		text_layer_set_text(percentage_layer, percentText);

		int8_t battery_state = (int8_t)(tuple->value->data[1]);
		text_layer_set_text(text_battery_layer, batteryStatus[battery_state]);
		
    }
	
	Tuple* tuple2 = dict_find(received, FIND_PHONE_PLAY_SOUND_KEY);
    if(tuple2) 
	{
		text_layer_set_text(Click_to_play_layer, "Sound Played!");
    }
}
static void app_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
	text_layer_set_text(Click_to_play_layer, "Not Connected :(");
	layer_set_hidden(text_layer_get_layer(status_layer), true);
}
