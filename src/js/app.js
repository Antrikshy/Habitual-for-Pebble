Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL("");
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode the user's preferences
  var configData = JSON.parse(decodeURIComponent(e.response));
  
  // Parse into dict
  var dict = {
    'Habit1': configData.habit1.toString(),
    'Habit2': configData.habit2.toString(),
    'Habit3': configData.habit3.toString(),
    'Habit4': configData.habit4.toString(),
    'Habit5': configData.habit5.toString()
  };
  
  // Send to watchface
  Pebble.sendAppMessage(dict, function() {
    console.log('Config data sent successfully!');
  }, function(e) {
    console.log('Error sending config data!');
  });
});
