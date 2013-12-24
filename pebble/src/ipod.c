
#include "ipod.h"
#include "main_menu.h"
#include "library_menus.h"
#include "marquee_text.h"
#include "ipod_state.h"
#include "now_playing.h"

#define MY_UUID { 0x24, 0xCA, 0x78, 0x2C, 0xB3, 0x1F, 0x49, 0x04, 0x83, 0xE9, 0xCA, 0x51, 0x9C, 0x60, 0x10, 0x97 }


Window *window;

void handle_timer(void *data) {
    
    marquee_text_layer_tick();
    now_playing_animation_tick();
	//stopwatch_handle_timer(app_ctx, handle, cookie);
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init-handle tick in");
    ipod_state_tick();
    now_playing_tick();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init-handle tick Out");
}


static void init() {
    //resource_init_current_app(&APP_RESOURCES);
	
	window = window_create();
  	window_set_background_color(window, GColorWhite);
  	window_set_fullscreen(window, true);
    
    
    main_menu_init(window);
    init_library_menus();
    ipod_state_init();
    
    
	//init_library_menus();
    //ipod_state_init();
	
  	/*window_set_window_handlers(window, (WindowHandlers) {
    	.load = window_load,
    	.unload = window_unload
  });*/

	//register app message
  	const int inbound_size = 124;
  	const int outbound_size = 256;
  	app_message_open(inbound_size, outbound_size);
	
	//register timer
	app_timer_register(100, handle_timer, NULL);
	
	//register tick handle
	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
	
  	window_stack_push(window, true);
}

static void deinit() {
	
  	window_destroy(window);
}


	
int main(void) {
  init();
  app_event_loop();
  deinit();
}


static char *appMessageResultToString(AppMessageResult reason)
{
    switch (reason)
    {
        case APP_MSG_BUFFER_OVERFLOW:
            return "buffer overflow";
        default:
            return "unknown error";
    }
}