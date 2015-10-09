Pebble.addEventListener('ready', function(){
    console.log('PebbleKit JS ready!');
});
//shows the launch of the website configuration page
Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://logibearnz.github.io/website/';
    console.log('Showing configuration page: ' + url);
    Pebble.openURL(url);
});

//Listening function for when values are returned  
Pebble.addEventListener('webviewclosed', function(e) {
  
  var configData = JSON.parse(decodeURIComponent(e.response));
  //prints values returned
  console.log('Configuration page returned: ' + JSON.stringify(configData));

  if (configData.backgroundColor) {
    console.log("dfff")
    Pebble.sendAppMessage({
      
      backgroundColor: parseInt(configData.backgroundColor, 16),
      secondsEnabled: configData.secondsEnabled,
      dateEnabled: configData.dateEnabled,
      dateFormat: configData.dateFormat
      
       
    }, function() {
      console.log('Send successful!');
    }, function() {
      console.log('Send failed!');
    });
  }
   console.log(configData.dateEnabled, "test");
});


