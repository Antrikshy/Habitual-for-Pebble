Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL("https://s3-us-west-1.amazonaws.com/antrikshyprojects/Habitual/habitual_config.html");
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode the user's preferences
  var options = JSON.parse(decodeURIComponent(e.response));
  
  // Send to watchface
  Pebble.sendAppMessage(options, function() {
    console.log('Config data sent successfully!');
  }, function(e) {
    console.log('Error sending config data!\n' + JSON.stringify(e));
  });
});
