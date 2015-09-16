Pebble.addEventListener('ready', function(){
    console.log('PebbleKit JS ready!');
});
Pebble.addEventListener('showConfiguration', function() {
    var url = '0.0.0.0:8080';
    console.log('Showing configuration page: ' + url);
    Pebble.openURL(url);
});
Pebble.addEventListener('webviewclosed', function(e){
    var configData = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration page returned: ' + JSON.stringify(configData));
    
    if (configData.backgroundColor) {
        pebble.sendAppMessage({
	    backgroundColor: parseInt(configData.backgroundColor, 16),
            secondsEnabled: configData.secondsenbled,
	    dateEnabled: configData.dateenabled
        }, function() {
           console.log('Send succesful!');
        }, function() {
           console.log('Send failed!');
        };)
    }
});


