#include <pebble.h>

#define HABIT_STRING_MAX_LENGTH 50
#define NUM_OF_HABITS 5

#define HABIT1_PERSIST_KEY 0
#define HABIT2_PERSIST_KEY 1
#define HABIT3_PERSIST_KEY 2
#define HABIT4_PERSIST_KEY 3
#define HABIT5_PERSIST_KEY 4

#define PLACEHOLDER_HABIT "Add custom text here using the Pebble app"

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_habit_layer;

static unsigned char s_current_habit = HABIT1_PERSIST_KEY;

static void update_time(struct tm *tick_time) {
  // Write hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void rotate_habit() {
  // Intermediary buffer to hold new habit string
  static char new_habit[HABIT_STRING_MAX_LENGTH];
  
  // Find next set habit, treating storage as circular buffer
  int i = s_current_habit;
  do {
    s_current_habit = (s_current_habit + 1) % NUM_OF_HABITS;
    if (persist_exists(s_current_habit)) {
      // Write habit to watchface
      persist_read_string(s_current_habit, new_habit, sizeof(new_habit));
      text_layer_set_text(s_habit_layer, new_habit); 
      return;
    }
  } while (s_current_habit != i);
  
  // If nothing found, set to placeholder text
  text_layer_set_text(s_habit_layer, PLACEHOLDER_HABIT);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  rotate_habit();
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
  text_layer_set_font(s_habit_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_habit_layer, GTextAlignmentCenter);

  // Default text on habit layer
  text_layer_set_text(s_habit_layer, PLACEHOLDER_HABIT);

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
  rotate_habit();
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_habit_layer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Catch and persist user's habit strings
    
  Tuple *h1 = dict_find(iterator, MESSAGE_KEY_Habit1);
  if (strlen(h1->value->cstring) > 0) {
    persist_write_string(HABIT1_PERSIST_KEY, h1->value->cstring);
  }
  else {
    persist_delete(HABIT1_PERSIST_KEY);
  }
  
  Tuple *h2 = dict_find(iterator, MESSAGE_KEY_Habit2);
  if (strlen(h2->value->cstring) > 0) {
    persist_write_string(HABIT2_PERSIST_KEY, h2->value->cstring);
  } else {
    persist_delete(HABIT2_PERSIST_KEY);
  }
  
  Tuple *h3 = dict_find(iterator, MESSAGE_KEY_Habit3);
  if (strlen(h3->value->cstring) > 0) {
    persist_write_string(HABIT3_PERSIST_KEY, h3->value->cstring);
  } else {
    persist_delete(HABIT3_PERSIST_KEY);
  }
  
  Tuple *h4 = dict_find(iterator, MESSAGE_KEY_Habit4);
  if (strlen(h4->value->cstring) > 0) {
    persist_write_string(HABIT4_PERSIST_KEY, h4->value->cstring);
  } else {
    persist_delete(HABIT4_PERSIST_KEY);
  }
  
  Tuple *h5 = dict_find(iterator, MESSAGE_KEY_Habit5);
  if (strlen(h5->value->cstring) > 0) {
    persist_write_string(HABIT5_PERSIST_KEY, h5->value->cstring);
  } else {
    persist_delete(HABIT5_PERSIST_KEY);
  }
  
  // Rotate habits to prevent an old one from persisting until next rotation
  s_current_habit = HABIT5_PERSIST_KEY; // Hacky way to cycle habits in order
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
  
  // Largest expected inbox and outbox message sizes
  const uint32_t inbox_size = 250;
  const uint32_t outbox_size = 0;
  
  // Open AppMessage
  app_message_open(inbox_size, outbox_size);
  
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
