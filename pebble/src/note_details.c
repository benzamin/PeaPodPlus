/*#include "note_details.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "common.h"

Window detailWindow;

TextLayer fullReminderTitleTextLayer;

#define MAX_NOTE_TEXT_LENGTH 512
static char noteString[MAX_NOTE_TEXT_LENGTH];
static AppMessageCallbacksNode app_callbacks;

// Methods
static void requestNoteDetails1();
static void window_unload1(Window* window);
static void window_load1(Window* window);
//////> temp code
static void messageSentSuccessfullyCallback(DictionaryIterator *sent, void *context);
static void messageSentWithErrorCallback(DictionaryIterator *failed, AppMessageResult reason, void *context);
static void messageReceivedWithErrorCallback(void *context, AppMessageResult reason);
//////<
static void app_in_received(DictionaryIterator *received, void *context);


void show_note_details(int index) {
    window_init(&detailWindow, "Note");
    window_set_window_handlers(&detailWindow, (WindowHandlers){
        .unload = window_unload1,
        .load = window_load1,
    });
    window_stack_push(&detailWindow, true);
}

static void window_load1(Window* window) 
{
   	text_layer_init(&fullReminderTitleTextLayer, GRect(0, 0, 144, 152));
	//text_layer_set_text(&fullReminderTitleTextLayer, noteString);
	text_layer_set_text(&fullReminderTitleTextLayer, "noteString");
    text_layer_set_text_alignment(&fullReminderTitleTextLayer, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(window), &fullReminderTitleTextLayer.layer);
    
    app_callbacks = (AppMessageCallbacksNode){
        .callbacks = {
            .in_received = app_in_received,
			 .out_failed = messageSentWithErrorCallback,
                .in_dropped = messageReceivedWithErrorCallback,
			.out_sent = messageSentSuccessfullyCallback,
        }
    };
   	app_message_register_callbacks(&app_callbacks);
	
	requestNoteDetails1();
}

static void window_unload1(Window* window) {
    app_message_deregister_callbacks(&app_callbacks);
}

 
static void messageSentSuccessfullyCallback(DictionaryIterator *sent, void *context)
{
	text_layer_set_text(&fullReminderTitleTextLayer, "Sent message succesfully");
}

static void messageReceivedWithErrorCallback(void *context, AppMessageResult reason)
{
    text_layer_set_text(&fullReminderTitleTextLayer, "Recived with Error");
}
static void messageSentWithErrorCallback(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
    text_layer_set_text(&fullReminderTitleTextLayer, "failed to send");
}
static void requestNoteDetails1() {
	 DictionaryIterator *iter;
    ipod_message_out_get(&iter);
    if(!iter) return;
    dict_write_int8(iter, GET_SPECIFIC_NOTE_KEY, 2);
    app_message_out_send();
    app_message_out_release();
}

static void app_in_received(DictionaryIterator *received, void* context) {
    //Tuple* tuple = dict_find(received, GET_SPECIFIC_NOTE_KEY);
	char *tmp = "here is a sample text. I am struggling for days with this. I can't find a way to send a msg? \n";
	static int count = 0;
    if(tuple && (count  != 1))
	{
		count = 1;
		text_layer_set_text(&fullReminderTitleTextLayer, noteString);
	     //size_t offset = tuple->value->data[0] * (MAX_INCOMING_SIZE-1);
         //memcpy(noteString + offset, tuple->value->data + 1, tuple->length - 1);
		//memcpy(noteString, tmp, strlen(tmp));
         //layer_mark_dirty(&fullReminderTitleTextLayer.layer);
    }
}
*/