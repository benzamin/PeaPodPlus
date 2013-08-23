#include "now_playing.h"
#include "pebble_os.h"
#include "pebble_fonts.h"
#include "pebble_app.h"
#include "marquee_text.h"
#include "ipod_state.h"
#include "progress_bar.h"
#include "common.h"

static Window batteryWindow;

TextLayer percentage_layer;
TextLayer status_layer;
TextLayer text_battery_layer;
Layer battery_layer;

static int8_t batteryPercent;

static AppMessageCallbacksNode app_callbacks;

static void window_unload(Window* window);
static void window_load(Window* window);

static void app_in_received(DictionaryIterator *received, void *context);
static void app_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context);
void battery_layer_update_callback(Layer *me, GContext* ctx);
static void request_battery_data();

static const char *batteryStatus[] = {
	"Unknown", 
    "In Use", 
    "Charging", 
    "Fully Charged"
};

void init_battery_status() {
    window_init(&batteryWindow, "Phone Battery Status");
    window_set_window_handlers(&batteryWindow, (WindowHandlers){
        .unload = window_unload,
        .load = window_load,
    });
    window_stack_push(&batteryWindow, true);
}

static void window_load(Window* window) {

    // Text labels
	text_layer_init(&percentage_layer, GRect(10, 50, 134 /* width */, 35 /* height */));
    text_layer_set_text(&percentage_layer, "Claculating...");
	text_layer_set_font(&percentage_layer,  fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(&percentage_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), &percentage_layer.layer);
	
	text_layer_init(&status_layer, GRect(10, 95, 134 /* width */, 15 /* height */));
    text_layer_set_text(&status_layer, "Battery Status:");
    text_layer_set_text_alignment(&status_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), &status_layer.layer);

	
	text_layer_init(&text_battery_layer, GRect(10, 110, 134, 30));
	text_layer_set_text_alignment(&text_battery_layer, GTextAlignmentCenter);
	text_layer_set_font(&text_battery_layer,  fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_get_root_layer(window), &text_battery_layer.layer);
	text_layer_set_text(&text_battery_layer, "Loading...");


	layer_init(&battery_layer, GRect(0, 0, 144, 55));
	battery_layer.update_proc = &battery_layer_update_callback;
	layer_add_child(window_get_root_layer(window), &battery_layer);

	batteryPercent = 100;
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

static void window_unload(Window* window) {
    app_message_deregister_callbacks(&app_callbacks);
}

void battery_layer_update_callback(Layer *me, GContext* ctx) {
	//add outer boundery
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_draw_rect(ctx, GRect(20,10,104,42));
	
	//draw the positive side of battery
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(123,25,5,12), 0, GCornerNone);

	//draw the percent fill
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_context_set_fill_color(ctx, GColorBlack);

	graphics_fill_rect(ctx, GRect(22, 12, batteryPercent, 38), 0, GCornerNone);
	
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

		char *percentText;
		percentText = itoa(batteryPercent);;
		percentText = strcat(percentText, "%");
		text_layer_set_text(&percentage_layer, percentText);
		
		int8_t battery_state = (int8_t)(tuple->value->data[1]);
		text_layer_set_text(&text_battery_layer, batteryStatus[battery_state]);
				
		
    }
}
static void app_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
	text_layer_set_text(&status_layer, "Can't Find Phone :(");
	text_layer_set_text(&text_battery_layer, "");
}
