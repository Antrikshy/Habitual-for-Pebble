Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL("https://s3-us-west-1.amazonaws.com/antrikshyprojects/Habitual/habitual_config.html");
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode the user's preferences
  var configData = JSON.parse(decodeURIComponent(e.response));
  
  // Parse into dict
  var dict = {
    'Habit1': configData.Habit1,
    'Habit2': configData.Habit2,
    'Habit3': configData.Habit3,
    'Habit4': configData.Habit4,
    'Habit5': configData.Habit5
  };
  
  // Send to watchface
  Pebble.sendAppMessage(dict, function() {
    console.log('Config data sent successfully!');
  }, function(e) {
    console.log('Error sending config data!\n' + JSON.stringify(e));
  });
});
