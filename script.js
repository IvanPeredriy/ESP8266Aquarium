
setInterval(function sendTime( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", '/setTime', true);
  xhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  var x = new Date().getTime() + 7200000;
  xhttp.onreadystatechange = function() {
  if (this.readyState != 4) return;
	}
  xhttp.send(x);
}, 1000);

// Set just temperature data. There are two ds18b20, which duplicate each other.
setInterval(function() {

    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("temp1").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/temperature1", true);
    xhttp.send();
}, 30000);

setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("temp2").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/temperature2", true);
    xhttp.send();
}, 30000);

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


function loadThermoData(numOfDevice){
	var xhttp = new XMLHttpRequest();
	if(numOfDevice == 1){
  		if (this.readyState == 4 && this.status == 200) {
	          y = parseFloat(this.responseText);
		}
		xhttp.open("GET", "/restoreLast1", true);
  		xhttp.send();
	}
	else if(numOfDevice == 2){
  		if (this.readyState == 4 && this.status == 200) {
	          y = parseFloat(this.responseText);
		}
		xhttp.open("GET", "/restoreLast2", true);
  		xhttp.send();
	}
}


var thermo1 = new Array(loadThermoData(1));
var thermo2 = new Array(loadThermoData(2));

// Charts of temperature. They didn't save yet.

var chartT = new Highcharts.Chart({
  chart:{ renderTo : 'chart-temperature1' },
  title: { text: 'Home temperature plot.' },
  series: [{
    name: 'First 18b20',
    data: []
  },
  {name: 'Second 18b20',
   data: []
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

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var x = ((new Date()).getTime() + 7200000),
          y = parseFloat(this.responseText);
      //console.log(this.responseText);
      if(chartT.series[0].data.length > 40) {
        chartT.series[0].addPoint([x, y], true, true, true);
      } else {
        chartT.series[0].addPoint([x, y], true, false, true);
      }
    }
  };
  xhttp.open("GET", "/temperature1", true);
  xhttp.send();
//}, 1200000 ) ;
}, 60000 );

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var x = ((new Date()).getTime() + 7200000),
          y = parseFloat(this.responseText);
      //console.log(this.responseText);
      if(chartT.series[1].data.length > 40) {
        chartT.series[1].addPoint([x, y], true, true, true);
      } else {
        chartT.series[1].addPoint([x, y], true, false, true);
      }
    }
  };
  xhttp.open("GET", "/temperature2", true);
  xhttp.send();
//}, 1200000 ) ;
}, 60000 );
