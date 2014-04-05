#include <pebble.h>
#include "cl_util.h"

#define APP_TITLE_HEIGHT 16
#define HEADER_HEIGHT 16
#define CELL_HEIGHT 20
#define TOTAL_PINS 8

//Keys
enum
{
	PIN_OFF = 0,
	PIN_ON = 1,
	PIN_EVENT = 2
};

//Pin states
enum
{
	STATE_OFF = 0,
	STATE_ON = 1,
	STATE_AWAITING_ON = -1,
	STATE_AWAITING_OFF = -2
};

//UI elements
static Window *window;
static MenuLayer *menu_layer;

//State
static int pin_states[TOTAL_PINS];	//-1 awaiting on, -2 awaiting off, 1 on, 0 off

/****************************** App Message callbacks ***************************************/

void process_tuple(Tuple *t)
{
	int key = t->key;
	int value = t->value->int32;

	switch(key)
	{
	case PIN_EVENT:
		//Toggle achieved
		switch(pin_states[value])
		{
		case STATE_AWAITING_ON:
			pin_states[value] = STATE_ON;
			break;
		case STATE_AWAITING_OFF:
			pin_states[value] = STATE_OFF;
			break;
		}
		break;
	default:
		cl_applog("Unknown key!");
		break;
	}

	menu_layer_reload_data(menu_layer);
}

/*
 * In received handler
 */
void in_received_handler(DictionaryIterator *iter, void *context) 
{
	Tuple *t = dict_read_first(iter);
	if(t)
	{
		process_tuple(t);
	}

	while(t)
	{
		t = dict_read_next(iter);
		if(t)
		{
			process_tuple(t);
		}
	}
}

/******************************* MenuLayer Functions *********************************/

/*
 * Get Header height callback
 */
int16_t get_header_height(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
	return HEADER_HEIGHT;
}

/*
 *Draw Header callback
 */
void draw_header(GContext *ctx, Layer *cell_layer, uint16_t section_index, void *callback_context)
{
	menu_cell_basic_header_draw(ctx, cell_layer, "Spark Core Pins");
}

/*
 * Get cell height
 */
int16_t get_cell_height(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
	return CELL_HEIGHT;
}

/*
 * Draw a MenuLayer row
 */
void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{	
	int index = cell_index->row;	
	
	//Draw empty LED
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_draw_circle(ctx, GPoint(15, 9), 9);
	
	graphics_context_set_text_color(ctx, GColorBlack);
	graphics_context_set_fill_color(ctx, GColorBlack);
	
	//If on, show LED on
	switch(pin_states[index])
	{
	case STATE_AWAITING_ON:	//Waiting for response
		graphics_draw_circle(ctx, GPoint(15, 9), 4);
		break;
	case STATE_ON:		//On
		graphics_fill_circle(ctx, GPoint(15, 9), 4);
		break;
	}	
	
	//Draw text
	graphics_context_set_text_color(ctx, GColorBlack);
	char* text = malloc(sizeof("Pin D0: WAITING"));
	switch(pin_states[index])
	{
	case STATE_OFF:
		snprintf(text, sizeof("Pin D0: OFF"), "Pin D%d: OFF", index);
		break;
	case STATE_ON:
		snprintf(text, sizeof("Pin D0: ON"), "Pin D%d: ON", index);
		break;
	default:
		snprintf(text, sizeof("Pin D0: WAITING"), "Pin D%d: WAITING", index);
		break;
	}
	graphics_draw_text(ctx, text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(30, -3, 144, 30), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
	free(text);
}

/*
 * Get number of sections callback
 */
uint16_t get_num_sections(MenuLayer *menu_layer, void *callback_context)
{
	return 1;
}

/*
 * Get the number of rows in the MenuLayer
 */
uint16_t get_num_rows(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
	return TOTAL_PINS;
}

/*
 * Select click callback
 */
void select_single_click(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) 
{
	int index = cell_index->row;
	
	//Send to Core
	if(pin_states[index] == STATE_OFF)
	{
		//Awaiting ON
		cl_send_int(PIN_ON, index);
		pin_states[index] = STATE_AWAITING_ON;
	}
	else if(pin_states[index] == STATE_ON)
	{
		//Awaiting OFF
		cl_send_int(PIN_OFF, index);
		pin_states[index] = STATE_AWAITING_OFF;
	}
	
	//Finally
	menu_layer_reload_data(menu_layer);
}

/******************************** Window Lifecycle ***********************************/

static void window_load(Window *window) 
{
	//Menu Layer
	menu_layer = menu_layer_create(GRect(0, 0, 144, 168 - APP_TITLE_HEIGHT));
	menu_layer_set_click_config_onto_window(menu_layer, window);
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
		.get_header_height = (MenuLayerGetHeaderHeightCallback) get_header_height,
		.draw_header = (MenuLayerDrawHeaderCallback) draw_header,
		.get_cell_height = (MenuLayerGetCellHeightCallback) get_cell_height,
		.draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
		.get_num_sections = (MenuLayerGetNumberOfSectionsCallback) get_num_sections,
		.get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) get_num_rows,
		.select_click = (MenuLayerSelectCallback) select_single_click
	});
		
	layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));
}

static void window_unload(Window *window) 
{
	//Free menu layer
	menu_layer_destroy(menu_layer);
}

/***************************** App Liecycle ******************************************/

static void init(void) 
{
	cl_set_debug(true);

	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	//AppMessage
	int 
		in = app_message_inbox_size_maximum(),
		out = app_message_outbox_size_maximum();
	cl_init_app_message(in, out, in_received_handler);
	app_log(APP_LOG_LEVEL_INFO, "C", 0, "I/O: %d/%d", in, out);
	
	//Responiveness+
	app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
	
	window_stack_push(window, true);
}

static void deinit(void) 
{
	//Save power
	app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL);
	
	window_destroy(window);
}

int main(void) 
{
	init();
	app_event_loop();
	deinit();
}