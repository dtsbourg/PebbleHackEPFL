#include "pebble.h"

static Window *window;
static Layer *layer;
static GBitmap *image;
static TextLayer *time_layer;

static void layer_update_callback(Layer *me, GContext* ctx) {

  GRect bounds = image->bounds;

  graphics_draw_bitmap_in_rect(ctx, image, (GRect) { .origin = { 10, 45 }, .size = bounds.size});
}

  static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {

  static char time_text[] = "00:00"; // Needs to be static because it's used by the system later
  
  strftime(time_text, sizeof(time_text), "%T", tick_time);
  text_layer_set_text(time_layer, time_text);
}

int main(void) {
  window = window_create();

  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  time_layer =text_layer_create(GRect(50, 0, 144-40 /* width */, 168-100 /* height */));
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_minute_tick(current_time, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));

  //Init the layer for display
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer=layer_create(bounds);
  layer_set_update_proc(layer, layer_update_callback);
  layer_add_child(window_layer, layer);

  image = gbitmap_create_with_resource(RESOURCE_ID_HACKERS);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();

  gbitmap_destroy(image);
  
  text_layer_destroy(time_layer);
  window_destroy(window);
  layer_destroy(layer);
}
