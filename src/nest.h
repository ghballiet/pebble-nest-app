// @name nest.h
// @auth github.com/ghballiet
// @desc header file for nest code

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "config.h"
#include "util.h"
#include "http.h"

#ifndef NEST_H
#define NEST_H

#define NEST_KEY_TEMPERATURE 1
#define NEST_KEY_AUTO_AWAY 2
#define NEST_KEY_MANUAL_AWAY 3
#define NEST_KEY_LEAF 4
#define NEST_KEY_TIME_TO_TARGET 5

#define NEST_COOKIE 1095938223
#define NEST_BUTTON_REPEAT_INTERVAL 100

typedef struct {
	char* mode;
	int temperature;
	int humidity;
	bool ac;
	bool heat;
	bool alt_heat;
	bool fan;
	int auto_away;
	int manual_away;
	int leaf;
} NestState;

typedef struct {
	char* mode;
	int temperature;
	int time_to_target;
} NestTarget;

typedef struct {
	bool online;
	char* last_connection;
	char* wan_ip;
	char* local_ip;
	char* mac_addr;
} NestNetwork;

typedef struct {
	NestState state;
	NestTarget target;
	NestNetwork network;
	char* serial_number;
	char scale;
} Nest;

extern Nest nest;
extern TextLayer nest_temp_text;

void nest_increase_temp(ClickRecognizerRef recognizer, Window *window);
void nest_decrease_temp(ClickRecognizerRef recognizer, Window *window);
void nest_set_temp(ClickRecognizerRef recognizer, Window *window);
void nest_toggle_away(ClickRecognizerRef recognizer, Window *window);
void nest_http_success(int32_t cookie, int http_status, DictionaryIterator* data, void* ctx);
void nest_http_failure(int32_t cookie, int http_status, void* ctx);
void nest_http_reconnect(void* ctx);
void nest_set_away();
void nest_send_temp();
void nest_request_temp();
void nest_set_text(char* msg);
void nest_set_debug(char* msg);
void nest_text_update();
void nest_button_config(ClickConfig **config, Window *window);
void nest_text_init(Window *window);
void nest_init(Window *window, AppContextRef ctx);

#endif
