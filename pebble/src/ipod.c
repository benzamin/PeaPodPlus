#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "main_menu.h"
#include "library_menus.h"
#include "marquee_text.h"
#include "ipod_state.h"
#include "now_playing.h"
//#include "stopwatch.h"

#define MY_UUID { 0x24, 0xCA, 0x78, 0x2C, 0xB3, 0x1F, 0x49, 0x04, 0x83, 0xE9, 0xCA, 0x51, 0x9C, 0x60, 0x10, 0x97 }
PBL_APP_INFO(MY_UUID,
             "Peapod", "Katharine Berry",
             1, 0, /* App version */
             RESOURCE_ID_APP_ICON,
             APP_INFO_STANDARD_APP);

Window window;
static AppMessageCallbacksNode app_callbacks; 
static void app_in_received(DictionaryIterator *received, void *context);

void handle_init(AppContextRef ctx) {
    g_app_context = ctx;
    window_init(&window, "iPod");
    window_stack_push(&window, true /* Animated */);
    main_menu_init(&window);
    //init_library_menus();
    //ipod_state_init();
    resource_init_current_app(&APP_RESOURCES);
}



void handle_timer(AppContextRef app_ctx, AppTimerHandle handle, uint32_t cookie) {

    marquee_text_layer_tick();
    now_playing_animation_tick();
	//stopwatch_handle_timer(app_ctx, handle, cookie);
}

void tick_handler(AppContextRef app_ctx, PebbleTickEvent *event) {
    ipod_state_tick();
    now_playing_tick();
}

static void app_in_received(DictionaryIterator *received, void* context) {
    Tuple* tuple = dict_find(received, FIND_PHONE_PLAY_SOUND_KEY);
    if(tuple) 
	{
		vibes_short_pulse();
    }
}
static void app_in_dropped(void *context, AppMessageResult reason)
{
    //displayErrorMessage(appMessageResultToString(reason));
}

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        .timer_handler = &handle_timer,
        .tick_info = (PebbleAppTickInfo){
            .tick_units = SECOND_UNIT,
            .tick_handler = tick_handler,
        },
        .messaging_info = (PebbleAppMessagingInfo){
            .buffer_sizes = {
                .inbound = 124,
                .outbound = 256
            }
        }
    };
	  //  app_callbacks = (AppMessageCallbacksNode){
      //  .callbacks = {
            //.in_received = app_in_received,
			//.out_sent = app_out_sent,
            //.out_failed = app_out_failed,
            //.in_dropped = app_in_dropped,
        //}
    //};
    //app_message_register_callbacks(&app_callbacks);
    app_event_loop(params, &handlers);
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