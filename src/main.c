#include <pebble.h>

#define HABIT_STRING_MAX_LENGTH 50
#define NUM_OF_HABITS 5

#define HABIT1_PERSIST_KEY 0
#define HABIT2_PERSIST_KEY 1
#define HABIT3_PERSIST_KEY 2
#define HABIT4_PERSIST_KEY 3
#define HABIT5_PERSIST_KEY 4

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_habit_layer;

static unsigned char s_current_habit = 0;

static void update_time(struct tm *tick_time) {
  // Write hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void rotate_habit() {
  char new_habit[HABIT_STRING_MAX_LENGTH];
  
  // If no habit set at this index, end
  if (!persist_exists(s_current_habit)) return;
  
  // Else, read habit from storage
  persist_read_string(s_current_habit++, new_habit, sizeof(new_habit));
  
  // Rotate habit index if necessary
  if (s_current_habit >= NUM_OF_HABITS) {
    s_current_habit = 0;
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  
  // TODO: Rotate habit every 5 minutes
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create TextLayer elements
  s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(30, 30), bounds.size.w, 50));
  s_habit_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(75, 75), bounds.size.w, 50));
  
  // Position TextLayer elements in window
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_background_color(s_habit_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_habit_layer, GColorWhite);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  text_layer_set_font(s_habit_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_habit_layer, GTextAlignmentCenter);

  // Default text on habit layer
  text_layer_set_text(s_habit_layer, "Add custom text here using the Pebble app");

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_habit_layer));
  
  // Get and display current time at load
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  update_time(tick_time);
  
  // Subscribe to tick timer for future time updates
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  rotate_habit();
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_habit_layer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Retrieve and persist user's habit strings
  
  // TODO: Delete for empty strings
  
  Tuple *h1 = dict_find(iterator, MESSAGE_KEY_Habit1);
  if (h1) {
    persist_write_string(HABIT1_PERSIST_KEY, h1->value->cstring);
  }
  
  Tuple *h2 = dict_find(iterator, MESSAGE_KEY_Habit2);
  if (h2) {
    persist_write_string(HABIT2_PERSIST_KEY, h2->value->cstring);
  }
  
  Tuple *h3 = dict_find(iterator, MESSAGE_KEY_Habit3);
  if (h3) {
    persist_write_string(HABIT3_PERSIST_KEY, h3->value->cstring);
  }
  
  Tuple *h4 = dict_find(iterator, MESSAGE_KEY_Habit4);
  if (h4) {
    persist_write_string(HABIT4_PERSIST_KEY, h4->value->cstring);
  }
  
  Tuple *h5 = dict_find(iterator, MESSAGE_KEY_Habit5);
  if (h5) {
    persist_write_string(HABIT5_PERSIST_KEY, h5->value->cstring);
  }
  
  // Rotate habits to prevent an old one from persisting until next rotation
  rotate_habit();
}

static void init() {
  // Prep main window
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  
  // Prep AppMessage receive
  app_message_register_inbox_received(inbox_received_callback);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
