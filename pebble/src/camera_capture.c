#include "camera_capture.h"
#include "pebble_os.h"
#include "pebble_fonts.h"
#include "pebble_app.h"
#include "common.h"

static Window cameraWindow;
static ActionBarLayer action_bar;
TextLayer capture_layer;
TextLayer switch_flash_layer;
TextLayer switch_camera_layer;

// Action bar icons
static HeapBitmap icon_catpure;
static HeapBitmap icon_switch_camera;
static HeapBitmap icon_switch_flash;

static AppMessageCallbacksNode app_callbacks;

static void click_config_provider(ClickConfig **config, void *context);
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
    window_init(&cameraWindow, "Ping My Phone");
    window_set_window_handlers(&cameraWindow, (WindowHandlers){
        .unload = window_unload,
        .load = window_load,
    });
    window_stack_push(&cameraWindow, true);
}

static void window_load(Window* window) {
    // Load bitmaps for action bar icons.
    heap_bitmap_init(&icon_catpure, RESOURCE_ID_ICON_CAMERA_CAPTURE);
	heap_bitmap_init(&icon_switch_camera, RESOURCE_ID_ICON_CAMERA_SWITCH);
    heap_bitmap_init(&icon_switch_flash, RESOURCE_ID_ICON_CAMERA_FLASH);
	
    // Action bar
    action_bar_layer_init(&action_bar);
    action_bar_layer_add_to_window(&action_bar, window);
    action_bar_layer_set_click_config_provider(&action_bar, click_config_provider);

    // Set default icon set.
    action_bar_layer_set_icon(&action_bar, BUTTON_ID_SELECT, &icon_catpure.bmp);
	action_bar_layer_set_icon(&action_bar, BUTTON_ID_DOWN, &icon_switch_flash.bmp);
    action_bar_layer_set_icon(&action_bar, BUTTON_ID_UP, &icon_switch_camera.bmp);

	// Text labels
	text_layer_init(&switch_camera_layer, GRect(0, 25, 124 /* width */, 20 /* height */));
    text_layer_set_text(&switch_camera_layer, "Switch Camera >");
    text_layer_set_text_alignment(&switch_camera_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), &switch_camera_layer.layer);
	
	text_layer_init(&capture_layer, GRect(0, 65, 124 /* width */, 20 /* height */));
    text_layer_set_text(&capture_layer, "Capture Image >");
    text_layer_set_text_alignment(&capture_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), &capture_layer.layer);
	
	text_layer_init(&switch_flash_layer, GRect(0, 100, 124 /* width */, 20 /* height */));
    text_layer_set_text(&switch_flash_layer, "Switch Flash >" );
    text_layer_set_text_alignment(&switch_flash_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), &switch_flash_layer.layer);

    
    app_callbacks = (AppMessageCallbacksNode){
        .callbacks = {
            .in_received = app_in_received,
			.out_failed = app_out_failed,
        }
    };
    app_message_register_callbacks(&app_callbacks);
	send_operate_camera(1);
}

static void window_unload(Window* window) {
	send_operate_camera(0);
    action_bar_layer_remove_from_window(&action_bar);
    app_message_deregister_callbacks(&app_callbacks);
    
    // deinit action bar icons
    heap_bitmap_deinit(&icon_catpure);
	heap_bitmap_deinit(&icon_switch_camera);
    heap_bitmap_deinit(&icon_switch_flash);

}

static void click_config_provider(ClickConfig **config, void* context) {
    config[BUTTON_ID_DOWN]->click.handler = clicked_down;
    config[BUTTON_ID_UP]->click.handler = clicked_up;
    config[BUTTON_ID_SELECT]->click.handler = clicked_select;
    config[BUTTON_ID_SELECT]->long_click.handler = long_clicked_select;
}

static void clicked_up(ClickRecognizerRef recognizer, void *context) {
	send_operate_camera(64);
	text_layer_set_text(&switch_camera_layer, "Camera: Switching...");
}
static void clicked_select(ClickRecognizerRef recognizer, void *context) {
    send_operate_camera(127);
	text_layer_set_text(&capture_layer, "Capturing Image...");
}
static void clicked_down(ClickRecognizerRef recognizer, void *context) {
	send_operate_camera(32);
	text_layer_set_text(&switch_flash_layer, "Flash: Switching...");
}
static void long_clicked_select(ClickRecognizerRef recognizer, void *context) {

}

static void send_operate_camera(int8_t val) {
    DictionaryIterator *iter;
    ipod_message_out_get(&iter);
    if(!iter) return;
    dict_write_int8(iter, CAMERA_CAPTURE_KEY, val);
    app_message_out_send();
    app_message_out_release();
}

static void app_in_received(DictionaryIterator *received, void* context) {
    Tuple* tuple = dict_find(received, CAMERA_CAPTURE_KEY);
    if(tuple) 
	{		
		if(tuple->value->data[0] == 255) //error performing a operation
		{
			text_layer_set_text(&capture_layer, "Error! Check Phone");
		}
		else if (tuple->value->data[0] == 0)
		{
			text_layer_set_text(&switch_flash_layer, "Flash: Auto");
		}
		else if (tuple->value->data[0] == 1)
		{
			text_layer_set_text(&switch_flash_layer, "Flash: Off");		
		}
		
		else if (tuple->value->data[0] == 2)
		{
			text_layer_set_text(&switch_flash_layer, "Flash: On");		
		}
		else if (tuple->value->data[0] == 3)
		{
			text_layer_set_text(&switch_flash_layer, "Flash: None");		
		}
		else if (tuple->value->data[0] == 64)
		{
			text_layer_set_text(&switch_camera_layer, "Camera: Rear");
		}
		else if (tuple->value->data[0] == 65)
		{
			text_layer_set_text(&switch_camera_layer, "Camera: Front");		
		}
		
		else if (tuple->value->data[0] == 127)
		{
			text_layer_set_text(&capture_layer, "Captured & Saved!");
		}
		
		else
		{
			text_layer_set_text(&capture_layer, "Error!Check Phone");
		}
    }
}
static void app_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
		text_layer_set_text(&capture_layer, "Error! Check Phone");
}
