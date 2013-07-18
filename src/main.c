// @name main.c
// @auth github.com/ghballiet
// @desc entry point for nest watchapp (the good stuff)

#include "config.h"
#include "nest.h"

// uses specific httpebble uuid
#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }
PBL_APP_INFO(MY_UUID,
			 "Nest", "Glen Balliet",
			 1, 0,
			 DEFAULT_MENU_ICON,
			 APP_INFO_STANDARD_APP);

Window window;

void handle_init(AppContextRef ctx) {
	window_init(&window, "Nest");
	window_stack_push(&window, true);
	window_set_background_color(&window, COLOR_APP_BG);

	// attach button click - defined in buttons.c
	window_set_click_config_provider(&window, (ClickConfigProvider) nest_button_config);

	// initialize nest stuff
	nest_init(&window, ctx);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
	  .init_handler = &handle_init,
	  .messaging_info = {
		  .buffer_sizes = {
			  .inbound = 124,
			  .outbound = 256
		  }
	  }
  };
  app_event_loop(params, &handlers);
}
