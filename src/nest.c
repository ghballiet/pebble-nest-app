// @name nest.c
// @auth github.com/ballet
// @DECs main stuff for interacting with nest

#include "nest.h"

Nest nest;
TextLayer nest_temp_text;
TextLayer nest_away_text;
TextLayer nest_debug_text;

void nest_increase_temp(ClickRecognizerRef recognizer, Window *window) {
	nest.state.temperature++;
	nest_text_update();
}

void nest_decrease_temp(ClickRecognizerRef recognizer, Window *window) {
	nest.state.temperature--;
	nest_text_update();
}

void nest_set_temp(ClickRecognizerRef recognizer, Window *window) {
	nest_set_debug("Set!");
	nest_send_temp();
}

void nest_toggle_away(ClickRecognizerRef recognizer, Window *window) {
	nest_set_debug("Toggle away!");

	if(nest.state.auto_away != 0)
		nest.state.auto_away = 0;

	if(nest.state.manual_away != 0)
		nest.state.manual_away = 0;
	else
		nest.state.manual_away = 1;

	nest_set_away();
}

void nest_http_success(int32_t cookie, int http_status, DictionaryIterator* data, void* ctx) {
	nest_set_debug("Request succeeded!");
	if(cookie != NEST_COOKIE) return;

	Tuple *temp = dict_find(data, NEST_KEY_TEMPERATURE);
	nest.state.temperature = temp->value->int16;

	Tuple *auto_away = dict_find(data, NEST_KEY_AUTO_AWAY);
	nest.state.auto_away = auto_away->value->int16;

	Tuple *manual_away = dict_find(data, NEST_KEY_MANUAL_AWAY);
	nest.state.manual_away = manual_away->value->int16;

	nest_text_update();
	nest_set_debug("");
}

void nest_http_failure(int32_t cookie, int http_status, void* ctx) {
	nest_set_debug("Request failed.");
}

void nest_http_reconnect(void* ctx) {
	nest_set_debug("Reconnecting...");
}

void nest_set_away() {
	nest_set_debug("Setting away...");
	DictionaryIterator *body;
	HTTPResult result = http_out_get(NEST_API_URI, NEST_COOKIE, &body);
	if(result != HTTP_OK) {
		nest_set_debug("ERR: http_out_get");
		return;
	}

	int away = nest.state.auto_away + nest.state.manual_away;

	dict_write_int32(body, 2, away);

	if(http_out_send() != HTTP_OK) {
		nest_set_debug("ERR: http_out_send");
		return;
	}
}

void nest_send_temp() {
	nest_set_debug("Sending...");
	DictionaryIterator *body;
	HTTPResult result = http_out_get(NEST_API_URI, NEST_COOKIE, &body);
	if(result != HTTP_OK) {
		nest_set_debug("ERR: http_out_get");
		return;
	}

	dict_write_int32(body, 1, nest.state.temperature);

	if(http_out_send() != HTTP_OK) {
		nest_set_debug("ERR: http_out_send");
		return;
	}
}

void nest_request_temp() {
	nest_set_debug("Requesting...");
	DictionaryIterator *body;
	HTTPResult result = http_out_get(NEST_API_URI, NEST_COOKIE, &body);
	if(result != HTTP_OK) {
		nest_set_debug("ERR: http_out_get");
		return;
	}

	if(http_out_send() != HTTP_OK) {
		nest_set_debug("ERR: http_out_send");
		return;
	}
}

void nest_set_text(char* msg) {
	text_layer_set_text(&nest_temp_text, msg);
}

void nest_set_away_text(char *msg) {
	text_layer_set_text(&nest_away_text, msg);
}

void nest_set_debug(char *msg) {
	text_layer_set_text(&nest_debug_text, msg);
}

void nest_text_update() {
	nest_set_text(itoa(nest.state.temperature));

	if(nest.state.auto_away == 1)
		nest_set_away_text("AUTO AWAY");
	else if(nest.state.manual_away == 1)
		nest_set_away_text("AWAY");
	else
		nest_set_away_text("");
}

void nest_button_config(ClickConfig **config, Window *window) {
	// configure select
	config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) nest_set_temp;
	config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) nest_toggle_away;

	// configure up
	config[BUTTON_ID_UP]->click.handler = (ClickHandler) nest_increase_temp;
	config[BUTTON_ID_UP]->click.repeat_interval_ms = NEST_BUTTON_REPEAT_INTERVAL;

	// configure down
	config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) nest_decrease_temp;
	config[BUTTON_ID_DOWN]->click.repeat_interval_ms = NEST_BUTTON_REPEAT_INTERVAL;
}

void nest_text_init(Window *window) {
	// temperature
	text_layer_init(&nest_temp_text, window->layer.frame);
	text_layer_set_text_color(&nest_temp_text, COLOR_TEXT_FG);
	text_layer_set_background_color(&nest_temp_text, COLOR_TEXT_BG);
	text_layer_set_font(&nest_temp_text, fonts_get_system_font(FONT_TEMP));
	layer_set_frame(&nest_temp_text.layer, GRect(0, 10, 144, 168-10));
	text_layer_set_text_alignment(&nest_temp_text, GTextAlignmentCenter);
	layer_add_child(&window->layer, &nest_temp_text.layer);

	// away
	text_layer_init(&nest_away_text, window->layer.frame);
	text_layer_set_text_color(&nest_away_text, COLOR_TEXT_FG);
	text_layer_set_background_color(&nest_away_text, COLOR_TEXT_BG);
	text_layer_set_font(&nest_away_text, fonts_get_system_font(FONT_DEBUG));
	layer_set_frame(&nest_away_text.layer, GRect(0, 80, 144, 168-80));
	text_layer_set_text_alignment(&nest_away_text, GTextAlignmentCenter);
	layer_add_child(&window->layer, &nest_away_text.layer);

	// debug
	text_layer_init(&nest_debug_text, window->layer.frame);
	text_layer_set_text_color(&nest_debug_text, COLOR_TEXT_FG);
	text_layer_set_background_color(&nest_debug_text, COLOR_TEXT_BG);
	text_layer_set_font(&nest_debug_text, fonts_get_system_font(FONT_DEBUG));
	layer_set_frame(&nest_debug_text.layer, GRect(0, 100, 144, 168-100));
	text_layer_set_text_alignment(&nest_debug_text, GTextAlignmentCenter);
	layer_add_child(&window->layer, &nest_debug_text.layer);
}

void nest_init(Window *window, AppContextRef ctx) {
	nest_text_init(window);
	nest_set_text("");
	nest_set_debug("Loading...");

	// register callbacks
	http_register_callbacks((HTTPCallbacks) {.failure=nest_http_failure,.success=nest_http_success,.reconnect=nest_http_reconnect}, (void*)ctx);
	nest_request_temp();
}
