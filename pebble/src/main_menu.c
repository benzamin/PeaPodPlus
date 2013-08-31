#include "main_menu.h"
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "common.h"
#include "library_menus.h"
//#include "ping_phone.h"
#include "ipod.h"
#include "ipod_state.h"
#include "peapod_menu.h"
#include "notes_menu.h"
#include "pebble-cal.h"
#include "battery_status.h"
//#include "stopwatch.h"
#include "camera_capture.h"

static void open_peapod_menu(int index, void* context);
//static void ping_my_phone(int index, void* context);
static void open_notes_menu(int index, void* context);
static void open_calender(int index, void* context);
static void open_battery_status(int index, void* context);
//static void open_stopwatch(int index, void* context);
static void open_camera_capture(int index, void* context);

static SimpleMenuItem main_menu_items[] = {
    {
        .title = "Peapod",
        .callback = open_peapod_menu,
    },
	{
        .title = "Notes",
        .callback = open_notes_menu,
    },
	{
        .title = "Calender",
        .callback = open_calender,
    },
	{
        .title = "Volume+Battery",
        .callback = open_battery_status,
    },
	{
        .title = "Camera Control",
        .callback = open_camera_capture,
    },
};

static SimpleMenuSection section = {
    .items = main_menu_items,
    .num_items = ARRAY_LENGTH(main_menu_items),
	.title = "Main Menu"
};

static SimpleMenuLayer main_menu_layer;


void main_menu_init(Window* window) {
    simple_menu_layer_init(&main_menu_layer, GRect(0, 0, 144, 152), window, &section, 1, NULL);
    layer_add_child(window_get_root_layer(window), simple_menu_layer_get_layer(&main_menu_layer));
}

static void open_peapod_menu(int index, void* context) {
	init_library_menus();
    ipod_state_init();
    peapod_menu_init();
}
//static void ping_my_phone(int index, void* context) {
//    show_ping_phone();
//}

static void open_notes_menu(int index, void* context) {
    notes_menu_init();
}
static void open_calender(int index, void* context)
{
	pebble_cal_init();
}
static void open_battery_status(int index, void* context)
{
	init_battery_status();
}
/*
static void open_stopwatch(int index, void* context)
{
	init_stopwatch_window();
}
*/
static void open_camera_capture(int index, void* context)
{
	show_camera_capture();
}
