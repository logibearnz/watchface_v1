(function() {
    loadOptions();
    submitHandler();
})();

function submitHandler(){
    var $submitButton = $('#submitButton');
        $submitButton.on('click', function() {
            console.log('Submit');
	    var return_to = getQueryParam('return_to', 'pebblejs://close#');
	    document.location = return_to + encodeURIComponent(JSON.stringify(getAndStoreConfigData()));
});

}

function getAndStoreConfigData() {
	var $backgroundColorPicker = $('#backgroundColorPicker');
	var $secondstoggle = $('#secondstoggle');
	var $datetoggle = $('#datetoggle');

	var options = {
		backgroundColor: $backgroundColorPicker.val(),
		secondsenabled: $secondstoggle[0].checked,
		dateenabled: $datetoggle[0].checked
	};

	localStorage.backgroundColor = options.backgroundColor;
	localStorage.secondsenabled = options.secondsenabled;
	localStorage.dateenabled = options.dateenabled;

	console.log('Got options ' + JSON.stringify(options));
	return options;
}

function loadOptions(){
    var $backgroundColorPicker = $('#backgroundColorPicker');
    var $secondstoggle = $('#secondstoggle');
    var $datetoggle = $('#datetoggle');
    
    if (localStorage.backgroundColor) {
        $backgroundColorPicker[0].value = localStorage.backgroundColor;
        $secondstoggle[0].checked = localStorage.secondsenabled === 'true';
	$datetoggle[0].checked = localStorage.dateenabled === 'true';
    }
}


function getQueryParam(variable, defaultValue) {
  // Find all URL parameters
  var query = location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i++) {
    var pair = vars[i].split('=');

    // If the query variable parameter is found, decode it to use and return it for use
    if (pair[0] === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return defaultValue || false;
}


