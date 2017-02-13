var first_update = true;
var prev_data = {};
var total_data = {};
var timer;
var prev_time = 0;
var total_time = 0;
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function ()
{
	if (this.readyState == 4 && this.status == 200)
	{
		var d = new Date();
		var current_time;
		if (first_update == true)
		{
			prev_time = d.getTime();
			current_time = prev_time;
		}
		else
		{
			current_time = d.getTime();
		}
		total_time += current_time - prev_time;
		var json_obj = JSON.parse(this.responseText);
		for (var mac in json_obj)
		{
			if (json_obj.hasOwnProperty(mac))
			{
				var interface_down = json_obj[mac].session["down"];
				var interface_up = json_obj[mac].session["up"];
				if (first_update == true)
				{
					if (prev_data[mac] == null)
					{
						prev_data[mac] = {};
					}
					if (total_data[mac] == null)
					{
						total_data[mac] = {};
						total_data[mac]["down"] = 0;
						total_data[mac]["up"] = 0;
					}
					prev_data[mac]["down"] = interface_down;
					prev_data[mac]["up"] = interface_up;
					continue;
				}
				total_data[mac]["down"] += interface_down - prev_data[mac]["down"];
				total_data[mac]["up"] += interface_up - prev_data[mac]["up"];
				var current_down = total_data[mac]["down"];
				var current_up = total_data[mac]["up"];
				var down_id = "mac_" + mac + "_down";
				var up_id = "mac_" + mac + "_up";
				var total_id = "mac_" + mac + "_total";
				var element = document.getElementById(down_id);
				if (element)
				{
					element.innerHTML = transfer2str(current_down);
				}
				element = document.getElementById(up_id);
				if (element)
				{
					element.innerHTML = transfer2str(current_up);
				}
				element = document.getElementById(total_id);
				if (element)
				{
					element.innerHTML = transfer2str(current_down + current_up);
				}
				prev_data[mac]["down"] = interface_down;
				prev_data[mac]["up"] = interface_up;
			}
		}
		var element = document.getElementById("time");
		if (element)
		{
			element.innerHTML = formatTime(total_time);
		}
		if (first_update == true)
		{
			first_update = false;
		}
		prev_time = current_time;
	}
};

function init()
{
	document.getElementById("start_button").disabled = false;
	document.getElementById("pause_button").disabled = true;
	document.getElementById("reset_button").disabled = true;
};

function starttimer()
{
	first_update = true;
	timer = setInterval(LoadData, 1000);
	document.getElementById("start_button").disabled = true;
	document.getElementById("pause_button").disabled = false;
	document.getElementById("reset_button").disabled = true;
};

function pausetimer()
{
	clearInterval(timer);
	document.getElementById("start_button").disabled = false;
	document.getElementById("pause_button").disabled = true;
	document.getElementById("reset_button").disabled = false;
	document.getElementById("start_button").value = "Continue";
};

function resetstats()
{
	first_update = true;
	total_time = 0;
	for (mac in prev_data)
	{
		prev_data[mac]["down"] = 0;
		prev_data[mac]["up"] = 0;
		total_data[mac]["down"] = 0;
		total_data[mac]["up"] = 0;
		var down_id = "mac_" + mac + "_down";
		var up_id = "mac_" + mac + "_up";
		var total_id = "mac_" + mac + "_total";
		var element = document.getElementById(down_id);
		if (element)
		{
			element.innerHTML = transfer2str(0);
		}
		element = document.getElementById(up_id);
		if (element)
		{
			element.innerHTML = transfer2str(0);
		}
		element = document.getElementById(total_id);
		if (element)
		{
			element.innerHTML = transfer2str(0);
		}
	}
	document.getElementById("start_button").value = "Start";
	document.getElementById("time").innerHTML = formatTime(0);
};

function pad(num, size)
{
	var s = "0000" + num;
	return s.substr(s.length - size);
}

function formatTime(time)
{
	var h = m = s = ms = 0;
	var newTime = '';
	h = Math.floor(time / (60 * 60 * 1000));
	time = time % (60 * 60 * 1000);
	m = Math.floor(time / (60 * 1000));
	time = time % (60 * 1000);
	s = Math.floor(time / 1000);
	newTime = pad(h, 2) + ':' + pad(m, 2) + ':' + pad(s, 2);
	return newTime;
}

function transfer2str(value)
{
	unit = "";
	if (value >= 1024 * 1024 * 1024 * 1024 * 1024)
	{
		scale = 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0;
		unit = "PB";
	}
	else if (value >= 1024 * 1024 * 1024 * 1024)
	{
		scale = 1024.0 * 1024.0 * 1024.0 * 1024.0;
		unit = "TB";
	}
	else if (value >= 1024 * 1024 * 1024)
	{
		scale = 1024.0 * 1024.0 * 1024.0;
		unit = "GB";
	}
	else if (value >= 1024 * 1024)
	{
		scale = 1024.0 * 1024.0;
		unit = "MB";
	}
	else if (value >= 1024)
	{
		scale = 1024.0;
		unit = "KB";
	}
	else
	{
		scale = 1.0;
		unit = "b";
	}
	value /= scale;
	return value.toFixed(2).replace(/\.?0*$/, '') + " " + unit;
}

function LoadData()
{
	xhttp.open("GET", "/stats.json", true);
	xhttp.setRequestHeader("Access-Control-Allow-Origin", "*");
	xhttp.setRequestHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
	xhttp.setRequestHeader("contentType", "application/json; charset=utf-8");
	xhttp.overrideMimeType("application/json");
	xhttp.send();
} 
