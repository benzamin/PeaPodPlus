#include "camera_capture.h"
#include "common.h"

static Window *cameraWindow;
static ActionBarLayer *action_bar;
TextLayer *capture_layer;
TextLayer *switch_flash_layer;
TextLayer *switch_camera_layer;

static GBitmap *icon_catpure = NULL;
static GBitmap *icon_switch_camera = NULL;
static GBitmap *icon_switch_flash = NULL;



static void click_config_provider(void *context);
static void window_unload(Window* window);
static void window_load(Window* window);
static void clicked_up(ClickRecognizerRef recognizer, void *context);
static void clicked_select(ClickRecognizerRef recognizer, void *context);
static void long_clicked_select(ClickRecognizerRef recognizer, void *context);
static void clicked_down(ClickRecognizerRef recognizer, void *context);

static void send_operate_camera(int8_t value);

static void app_in_received(DictionaryIterator *received, void *context);
static void app_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context);


void show_camera_capture(){	
	  cameraWindow = window_create();
	  window_set_background_color(cameraWindow, GColorWhite);
	  window_set_fullscreen(cameraWindow, true);
	  window_set_window_handlers(cameraWindow, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	  });
		
	  window_stack_push(cameraWindow, true);
}

static void window_load(Window* window) {
    // Load bitmaps for action bar icons.
    icon_catpure = gbitmap_create_with_resource(RESOURCE_ID_ICON_CAMERA_CAPTURE);///
    icon_switch_camera = gbitmap_create_with_resource(RESOURCE_ID_ICON_CAMERA_SWITCH);///
    icon_switch_flash = gbitmap_create_with_resource(RESOURCE_ID_ICON_CAMERA_FLASH);///
    
	
    // Action bar
    action_bar = action_bar_layer_create();
    action_bar_layer_add_to_window(action_bar, window);
    action_bar_layer_set_click_config_provider(action_bar, click_config_provider);

    // Set default icon set.
    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, icon_catpure);///
    action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, icon_switch_flash);///
    action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, icon_switch_camera);///
    

	// Text labels
    switch_camera_layer = text_layer_create(GRect(0, 25, 124 /* width */, 20 /* height */));
    text_layer_set_text(switch_camera_layer, "Switch Camera >");
    text_layer_set_text_alignment(switch_camera_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(switch_camera_layer));
	
    capture_layer = text_layer_create(GRect(0, 65, 124 /* width */, 20 /* height */));
    text_layer_set_text(capture_layer, "Capture Image >");
    text_layer_set_text_alignment(capture_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(capture_layer));
	
    switch_flash_layer = text_layer_create(GRect(0, 100, 124 /* width */, 20 /* height */));
    text_layer_set_text(switch_flash_layer, "Switch Flash >" );
    text_layer_set_text_alignment(switch_flash_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(switch_flash_layer));

    
	app_message_register_inbox_received(app_in_received);
   	app_message_register_outbox_failed(app_out_failed);

   const uint32_t inbound_size = 128;
   const uint32_t outbound_size = 256;
   app_message_open(inbound_size, outbound_size);
	
	send_operate_camera(1);
}

static void window_unload(Window* window) {
	send_operate_camera(0);
    action_bar_layer_remove_from_window(action_bar);
    app_message_deregister_callbacks();

}

static void click_config_provider(void* context) {
    
	const uint16_t repeat_interval_ms = 50;
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) clicked_select);
	window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler)long_clicked_select, NULL);
  	window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) clicked_up);
  	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) clicked_down);
	
}


static void clicked_up(ClickRecognizerRef recognizer, void *context) {
	send_operate_camera(64);
	text_layer_set_text(switch_camera_layer, "Camera: Switching...");
}
static void clicked_select(ClickRecognizerRef recognizer, void *context) {
    send_operate_camera(127);
	text_layer_set_text(capture_layer, "Capturing Image...");
}
static void clicked_down(ClickRecognizerRef recognizer, void *context) {
	send_operate_camera(32);
	text_layer_set_text(switch_flash_layer, "Flash: Switching...");
}
static void long_clicked_select(ClickRecognizerRef recognizer, void *context) {

}

static void send_operate_camera(int8_t val) {
    DictionaryIterator *iter;
    ipod_message_out_get(&iter);
    if(!iter) return;
    dict_write_int8(iter, CAMERA_CAPTURE_KEY, val);
    app_message_outbox_send();
}

static void app_in_received(DictionaryIterator *received, void* context) {
    Tuple* tuple = dict_find(received, CAMERA_CAPTURE_KEY);
    if(tuple) 
	{		
		//layer_set_hidden(&switch_camera_layer.layer, false);
		//layer_set_hidden(&switch_flash_layer.layer, false);
		if(tuple->value->data[0] == 255) //error performing a operation
		{
			text_layer_set_text(capture_layer, "Open Phone App!");
		}
		else if (tuple->value->data[0] == 0)
		{
			text_layer_set_text(switch_flash_layer, "Flash: Auto");
		}
		else if (tuple->value->data[0] == 1)
		{
			text_layer_set_text(switch_flash_layer, "Flash: Off");
		}
		else if (tuple->value->data[0] == 2)
		{
			text_layer_set_text(switch_flash_layer, "Flash: On");
		}
		else if (tuple->value->data[0] == 3)
		{
			text_layer_set_text(switch_flash_layer, "Flash: None");
		}
		else if (tuple->value->data[0] == 64)
		{
			text_layer_set_text(switch_camera_layer, "Camera: Rear");
		}
		else if (tuple->value->data[0] == 65)
		{
			text_layer_set_text(switch_camera_layer, "Camera: Front");
		}
		
		else if (tuple->value->data[0] == 127)
		{
			text_layer_set_text(capture_layer, "Captured & Saved!");
		}
		
		else
		{
			text_layer_set_text(capture_layer, "Error!Check Phone");
		}
    }
}
static void app_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
		text_layer_set_text(capture_layer, "Not Connected :(");
		layer_set_hidden(text_layer_get_layer(switch_camera_layer), true);
		layer_set_hidden(text_layer_get_layer(switch_flash_layer), true);
}
