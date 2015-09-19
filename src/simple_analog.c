#include "simple_analog.h"
#define COLORS   true
#include <pebble.h>

#define KEY_BACKGROUND_COLOR 0
#define KEY_SECONDS_ENABLED 1
#define KEY_DATE_ENABLED 2
#define KEY_DATE_FORMAT 3
static Window *window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer;
static TextLayer *s_day_label, *s_num_label, *s_twelve_label, *s_one_label, *s_eleven_label, *s_two_label, *s_three_label, *s_four_label,
                  *s_five_label, *s_six_label, *s_seven_label, *s_eight_label, *s_nine_label, *s_ten_label;

static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPath *s_minute_arrow, *s_hour_arrow;
static char s_num_buffer[4], s_day_buffer[6];
static bool date_enabled = true;
static bool seconds_enabled = true;



static void bg_update_proc(Layer *layer, GContext *ctx) { //Background and Pointer colour
  graphics_context_set_fill_color(ctx, GColorPastelYellow);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorBlack);
  
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_draw_filled(ctx, s_tick_paths[i]);
  }
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  int16_t second_hand_length = bounds.size.w / 2;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  if (seconds_enabled == true) {
	  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
	  GPoint second_hand = {
		.x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
		.y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
	  };
  
	  // second hand
	  graphics_context_set_stroke_color(ctx, GColorRed);
	  graphics_draw_line(ctx, second_hand, center);
  }
  // minute/hour hand
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  text_layer_set_text(s_day_label, s_day_buffer);

  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  text_layer_set_text(s_num_label, s_num_buffer);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *background_color_t = dict_find(iter, KEY_BACKGROUND_COLOR);
  Tuple *seconds_enabled_t = dict_find(iter, KEY_SECONDS_ENABLED);
  Tuple *date_enabled_t = dict_find(iter, KEY_DATE_ENABLED);
  Tuple *date_format_t = dict_find(iter, KET_DATE_FORMAT);

  if (background_color_t) {
    int background_color = background_color_t->value->int32;

    persist_write_int(KEY_BACKGROUND_COLOR, background_color);

    bg_update_proc(background_color);
  }

  if (seconds_enabled_t) {
    seconds_enabled = seconds_enabled_t->value->int8;

    persist_write_int(KEY_SECONDS_ENABLED, seconds_enabled);

    hands_update_proc();
	
  if (date_enabled_t) {
	date_enabled = date_enabled_t->value->int8;
	
	persist_write_int(KEY_DATE_ENABLED, date_enabled);
	
	update_time();
  }
  if (date_format_t) {
	date_format = date_format_t->value->int8;

	persist_write_int(KEY_DATE_FORMAT, date_format);

	
  }
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);

  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);
  
  if (date_enabled == true) {
	  s_day_label = text_layer_create(GRect(46, 105, 27, 24));
	  text_layer_set_text(s_day_label, s_day_buffer);
	  text_layer_set_background_color(s_day_label, GColorClear);
	  text_layer_set_text_color(s_day_label, GColorBlack);
	  text_layer_set_font(s_day_label, fonts_get_system_font(FONT_KEY_GOTHIC_24));

	  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label));

	  s_num_label = text_layer_create(GRect(73, 105, 18, 24));
	  text_layer_set_text(s_num_label, s_num_buffer);
	  text_layer_set_background_color(s_num_label, GColorClear);
	  text_layer_set_text_color(s_num_label, GColorBlack);
	  text_layer_set_font(s_num_label, fonts_get_system_font(FONT_KEY_GOTHIC_24));

	  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label));
  }
  //Adding Numbers 
  //12
  s_twelve_label = text_layer_create(GRect(65, 12, 18, 20));
  text_layer_set_text(s_twelve_label, "12");
  text_layer_set_background_color(s_twelve_label, GColorClear);
  text_layer_set_text_color(s_twelve_label, GColorBlack);
  text_layer_set_font(s_twelve_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_twelve_label));
  
  //1
  s_one_label = text_layer_create(GRect(102, 12, 18, 20));
  text_layer_set_text(s_one_label, "1");
  text_layer_set_background_color(s_one_label, GColorClear);
  text_layer_set_text_color(s_one_label, GColorBlack);
  text_layer_set_font(s_one_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_one_label));
  
  //two
  s_two_label = text_layer_create(GRect(120, 35, 18, 20));
  text_layer_set_text(s_two_label, "2");
  text_layer_set_background_color(s_two_label, GColorClear);
  text_layer_set_text_color(s_two_label, GColorBlack);
  text_layer_set_font(s_two_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_two_label));
  
  //three
  s_three_label = text_layer_create(GRect(120, 72, 18, 20));
  text_layer_set_text(s_three_label, "3");
  text_layer_set_background_color(s_three_label, GColorClear);
  text_layer_set_text_color(s_three_label, GColorBlack);
  text_layer_set_font(s_three_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_three_label));
  
  //four
  s_four_label = text_layer_create(GRect(120, 108, 18, 20));
  text_layer_set_text(s_four_label, "4");
  text_layer_set_background_color(s_four_label, GColorClear);
  text_layer_set_text_color(s_four_label, GColorBlack);
  text_layer_set_font(s_four_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_four_label));
  
  //five
  s_five_label = text_layer_create(GRect(102, 135, 18, 20));
  text_layer_set_text(s_five_label, "5");
  text_layer_set_background_color(s_five_label, GColorClear);
  text_layer_set_text_color(s_five_label, GColorBlack);
  text_layer_set_font(s_five_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_five_label));
  
  //six
  s_six_label = text_layer_create(GRect(68, 135, 18, 20));
  text_layer_set_text(s_six_label, "6");
  text_layer_set_background_color(s_six_label, GColorClear);
  text_layer_set_text_color(s_six_label, GColorBlack);
  text_layer_set_font(s_six_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_six_label));
  
  //seven
  s_seven_label = text_layer_create(GRect(35, 135, 18, 20));
  text_layer_set_text(s_seven_label, "7");
  text_layer_set_background_color(s_seven_label, GColorClear);
  text_layer_set_text_color(s_seven_label, GColorBlack);
  text_layer_set_font(s_seven_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_seven_label));
  
  //eight
  s_eight_label = text_layer_create(GRect(18, 108, 18, 20));
  text_layer_set_text(s_eight_label, "8");
  text_layer_set_background_color(s_eight_label, GColorClear);
  text_layer_set_text_color(s_eight_label, GColorBlack);
  text_layer_set_font(s_eight_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_eight_label));
  
   //nine
  s_nine_label = text_layer_create(GRect(18, 72, 18, 20));
  text_layer_set_text(s_nine_label, "9");
  text_layer_set_background_color(s_nine_label, GColorClear);
  text_layer_set_text_color(s_nine_label, GColorBlack);
  text_layer_set_font(s_nine_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_nine_label));
  
  //ten
  s_ten_label = text_layer_create(GRect(18, 35, 18, 20));
  text_layer_set_text(s_ten_label, "10");
  text_layer_set_background_color(s_ten_label, GColorClear);
  text_layer_set_text_color(s_ten_label, GColorBlack);
  text_layer_set_font(s_ten_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_ten_label));
  
  //eleven 
  s_eleven_label = text_layer_create(GRect(32, 12, 18, 20));
  text_layer_set_text(s_eleven_label, "11");
  text_layer_set_background_color(s_eleven_label, GColorClear);
  text_layer_set_text_color(s_eleven_label, GColorBlack);
  text_layer_set_font(s_eleven_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_eleven_label));
  
  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);
}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
  layer_destroy(s_date_layer);

  text_layer_destroy(s_day_label);
  text_layer_destroy(s_num_label);

  layer_destroy(s_hands_layer);
}

static void init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  s_day_buffer[0] = '\0';
  s_num_buffer[0] = '\0';

  // init hand paths
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    s_tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
  }

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_destroy(s_tick_paths[i]);
  }

  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
