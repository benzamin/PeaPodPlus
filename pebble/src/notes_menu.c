#include "notes_menu.h"
#include "common.h"
//#include "note_details.h"
	
Window notewindow;
Window detailWindow;

TextLayer errorTextLayer;
TextLayer fullReminderTitleTextLayer;
ScrollLayer scroll_layer;

static AppMessageCallbacksNode app_callbacks;

SimpleMenuLayer menuLayer;
SimpleMenuSection sections[1];

#define MAX_NOTES 5
#define MAX_NOTE_TITLE_LENGTH 15
static char menuList[MAX_NOTES][MAX_NOTE_TITLE_LENGTH];
SimpleMenuItem menuItems[MAX_NOTES];

#define MAX_NOTE_TEXT_LENGTH 699
static char noteString[MAX_NOTE_TEXT_LENGTH];

// Methods
static void messageSentSuccessfullyCallback(DictionaryIterator *sent, void *context);
static void messageSentWithErrorCallback(DictionaryIterator *failed, AppMessageResult reason, void *context);
static void messageReceivedSuccessfullyCallback(DictionaryIterator *received, void *context);
static void messageReceivedWithErrorCallback(void *context, AppMessageResult reason);
static void requestNotes();
static void requestNoteDetails(int index);
static void addMenuItem(int index, char *title);
static char *appMessageResultToString(AppMessageResult reason);
static void simpleMenuItemSelectedCallback(int index, void *context);
static void createMenu(int numElements);
static void window_unload(Window* window);
static void window_load(Window* window);

/* DEBUG */

static void displayErrorMessage(char *errorMessage)
{
    text_layer_set_text(&errorTextLayer, errorMessage);
    layer_set_hidden(&errorTextLayer.layer, false);
}

/* INIT */
void notes_menu_init() {
	  window = window_create();
	  window_set_background_color(window, GColorWhite);
	  window_set_fullscreen(window, true);
	  window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	  });
		
	  window_stack_push(notewindow, true);
}

static void window_load(Window* window) {
	    // Error text layer
    text_layer_init(&errorTextLayer,  GRect(0, 0, 144, 152));
    text_layer_set_text_color(&errorTextLayer, GColorBlack);
    text_layer_set_background_color(&errorTextLayer, GColorWhite);
    text_layer_set_font(&errorTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(&errorTextLayer, GTextAlignmentCenter);
    text_layer_set_text(&errorTextLayer, "text");
    layer_add_child(window_get_root_layer(window), &errorTextLayer.layer);
    layer_set_hidden(&errorTextLayer.layer, true);
	
	
	app_message_register_inbox_received(messageReceivedSuccessfullyCallback);
   app_message_register_inbox_dropped(messageReceivedWithErrorCallback);
   app_message_register_outbox_sent(messageSentSuccessfullyCallback);
   app_message_register_outbox_failed(messageSentWithErrorCallback);

   const uint32_t inbound_size = 128;
   const uint32_t outbound_size = 256;
   app_message_open(inbound_size, outbound_size);
	
	// Start
    requestNotes();
}
static void window_unload(Window* window) 
{
	app_message_deregister_callbacks(&app_callbacks);
}

/* MESSAGE HANDLING */

static void requestNotes()
{
    DictionaryIterator *iter;
    ipod_message_out_get(&iter);
    if(!iter) return;
    dict_write_int8(iter, GET_NOTES_LIST_KEY, 5);
    app_message_outbox_send();

}

static void messageSentSuccessfullyCallback(DictionaryIterator *sent, void *context)
{

}

static void messageSentWithErrorCallback(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
    displayErrorMessage("Not Connected :(");
}

static void messageReceivedSuccessfullyCallback(DictionaryIterator *received, void *context)
{
	const int vert_scroll_text_padding = 20;
	
	Tuple* tuple = dict_find(received, GET_SPECIFIC_NOTE_KEY);
    if(tuple)
	{
	    size_t offset = tuple->value->data[0] * (MAX_INCOMING_SIZE-1);
        memcpy(noteString + offset, tuple->value->data + 1, tuple->length - 1);
        
		// Trim text layer and scroll content to fit text box
  		GSize max_size = text_layer_get_max_used_size(app_get_current_graphics_context(), &fullReminderTitleTextLayer);
  		text_layer_set_size(&fullReminderTitleTextLayer, max_size);
  		scroll_layer_set_content_size(&scroll_layer, GSize(144, max_size.h + vert_scroll_text_padding));
		//layer_mark_dirty(&fullReminderTitleTextLayer.layer);
		
		return;
    }
	

    static int initialized = 0;

    if (initialized == 1)
    {
        displayErrorMessage("Error!");
        return;
    }

    Tuple *firstElement = dict_read_first(received);

    int index = 0;

    if (firstElement)
    {
        for(int i = 0; i <= MAX_NOTES ; i ++)
        {
            Tuple *nextElement = dict_find(received, i);

            if (!nextElement)
            {
                continue;
            }

            char *text = (char *)nextElement->value;

            //addMenuItem(i, text);
			memcpy(menuList[i], text, (nextElement->length) < (MAX_NOTE_TITLE_LENGTH - 1) ? (nextElement->length) : MAX_NOTE_TITLE_LENGTH - 1 );

            index++;
        }    
    }
	else
	{
		displayErrorMessage("No Notes.");
		return;
	}

    createMenu(index);
}

static void messageReceivedWithErrorCallback(void *context, AppMessageResult reason)
{
    displayErrorMessage(appMessageResultToString(reason));
}

/* MENU */

static void addMenuItem(int index, char *title)
{
    menuItems[index] = (SimpleMenuItem){.title = title, .subtitle = NULL, .icon = NULL, .callback = simpleMenuItemSelectedCallback};
}

static void createMenu(int numElements)
{
	for(int i = 0; i < numElements ; i ++) 
		
	{
		addMenuItem(i, menuList[i]);
	}
    // Menu
    sections[0] = (SimpleMenuSection){.items = menuItems, .num_items = numElements, .title = "Notes"};

    simple_menu_layer_init(&menuLayer,
                           GRect(0, 0, 144, 152),
                           &notewindow,
                           sections,
                           1,
                           NULL);

    layer_add_child(&notewindow.layer, simple_menu_layer_get_layer(&menuLayer));
}

static void simpleMenuItemSelectedCallback(int index, void *context)
{
	//first remove existing text
	memcpy(noteString, "Loading..." , 10);
	memset(noteString + 10, 0 , MAX_NOTE_TEXT_LENGTH-10);
	
	const char *noteSelected = ((SimpleMenuItem)menuItems[index]).title;	
	 window_init(&detailWindow, noteSelected);
    window_stack_push(&detailWindow, true);

	const GRect max_text_bounds = GRect(0, 0, 144, 2000);

    text_layer_init(&fullReminderTitleTextLayer, max_text_bounds);
    text_layer_set_text_color(&fullReminderTitleTextLayer, GColorBlack);
    text_layer_set_background_color(&fullReminderTitleTextLayer, GColorWhite);
    text_layer_set_text_alignment(&fullReminderTitleTextLayer, GTextAlignmentLeft);
	text_layer_set_font(&fullReminderTitleTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text(&fullReminderTitleTextLayer, noteString);
    //layer_add_child(&detailWindow.layer, &fullReminderTitleTextLayer.layer);
		
  	//set up scroll layer
  	scroll_layer_init(&scroll_layer, detailWindow.layer.bounds);
 	scroll_layer_set_click_config_onto_window(&scroll_layer, &detailWindow);
	// Set the initial max size
  	scroll_layer_set_content_size(&scroll_layer, max_text_bounds.size);

  	// Add the layers for display
  	scroll_layer_add_child(&scroll_layer, &fullReminderTitleTextLayer.layer);

  	layer_add_child(&detailWindow.layer, &scroll_layer.layer);

	//now request Phone to deiver the note
	requestNoteDetails(index);
}

static void requestNoteDetails(int index) {
	 DictionaryIterator *iter;
    ipod_message_out_get(&iter);
    if(!iter) return;
    dict_write_int8(iter, GET_SPECIFIC_NOTE_KEY, index);
    app_message_outbox_send();
}

/* Helpers */

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