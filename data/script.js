function req(url, success, postdata) {
  var xhttp = new XMLHttpRequest();
  xhttp.open(postdata ? "POST" : "GET", url, true);
  if (postdata)
    xhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4)
    if (success)
      success(this);
	}
   var str = [];
    for (var key in postdata) {
         if (postdata.hasOwnProperty(key)) {
               str.push(encodeURIComponent(key) + "=" + encodeURIComponent(postdata[key]))                  
               //console.log(key + " -> " + postdata[key]);
         }
    }

  if(postdata == "POST")
  	xhttp.send(str.join("&"));
  else
  	xhttp.send();
}


// Software reset. Press if some bugs with FTP connection for example.
function sendForm(e){
     var xhttp = new XMLHttpRequest();
    // получаем значение поля key
    var result = confirm("Reset ESP?");
    if(result){
	    xhttp.open("GET", "/reset", false);
	    xhttp.send();
    }
}
 
var sendButton = document.search.res;
sendButton.addEventListener("click", sendForm);





// Temperature chart

var chartT = new Highcharts.Chart({
  chart:{ renderTo : 'chart-temperature1' },
  title: { text: 'Home temperature plot.' },
  series: [{
    name: 'First 18b20',
    data: []
  },
  {name: 'Second 18b20',
   data: [],
   color: '#9e0519'
  }],
  plotOptions: {
    line: { animation: false,
      dataLabels: { enabled: true }
    },
    series: { color: '#059e8a' }
  },
  xAxis: { type: 'datetime',
    dateTimeLabelFormats: { second: '%H:%M:%S' }
  },
  yAxis: {
    title: { text: 'Temperature (Celsius)' }
    //title: { text: 'Temperature (Fahrenheit)' }
  },
  credits: { enabled: false }
});


// Function-getter of temperature data from ESP.
// After sending of GET-request, ESP get info from NTP-server and save average measurement + data to RAM

setInterval(function() {
	var highchartWindow = 48;
	req('/temperature2', function(a){
		//console.log(a);
		if(a.status == 200 && a.readyState == 4){
			var x = ((new Date()).getTime() + 7200000),
			y = parseFloat(a.responseText);
			document.getElementById("temp2").innerHTML = a.responseText;
		}
		if(chartT.series[1].data.length > highchartWindow){
			chartT.series[1].addPoint([x,y], true, true, true);
		} else {
			chartT.series[1].addPoint([x, y], true, false, true);
		}
	}, false);

	req('/temperature1', function(a){
		//console.log(a);
		if(a.status == 200 && a.readyState == 4){
			var x = ((new Date()).getTime() + 7200000),
			y = parseFloat(a.responseText);
			document.getElementById("temp1").innerHTML = a.responseText;
		}
		if(chartT.series[0].data.length > highchartWindow){
			chartT.series[0].addPoint([x,y], true, true, true);
		} else {
			chartT.series[0].addPoint([x, y], true, false, true);
		}
	}, false);
}, 100000000);


var colorPicker = new iro.ColorPicker("#colorPicker", {
  // Set the size of the color picker
  width: 500,
  // Set the initial color to pure red
  color: "#f00",

  padding: 3,

  margin: 20,

  borderWidth: 2,

  borderColor: "#000000"
});

function colorChangeCallback(color){
	//alert(color.hexString);		// rgbString
	//req('/rgbColor', color.hexString, true);
	var xhttp = new XMLHttpRequest();
	xhttp.open("POST", '/rgbColor', true);
	xhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	xhttp.send(color.hexString);
	if(xhttp.readyState == 4){
		//zbs
	}
}

colorPicker.on("color:change", colorChangeCallback);