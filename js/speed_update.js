var xhttp = new XMLHttpRequest();

var timer = setInterval(LoadData, 1000);


xhttp.onreadystatechange = function()
{
	if (this.readyState == 4 && this.status == 200)
	{
		var json_obj = JSON.parse(this.responseText);
		for (var mac in json_obj)
		{
			if (json_obj.hasOwnProperty(mac))
			{
				var up_id = "mac_" + mac + "_up";
				var down_id = "mac_" + mac + "_down";

				var down_speed = json_obj[mac].speed["down"];
				var up_speed = json_obj[mac].speed["up"];

				var element=document.getElementById(up_id);
                if (element)
                {
                    element.innerHTML = value2str(up_speed);
                }
                
				element=document.getElementById(down_id);
                if (element)
                {
                    element.innerHTML = value2str(down_speed);
                }
			}
		}

	}
};

function value2str(value)
{
	unit = "";

	if (value >= 1024 * 1024 * 1024 * 1024 * 1024)
	{
		scale = 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0;
		unit = "Pb/s";
	}
	else if (value >= 1024 * 1024 * 1024 * 1024)
	{
		scale = 1024.0 * 1024.0 * 1024.0 * 1024.0;
		unit = "Tb/s";
	}
	else if (value >= 1024 * 1024 * 1024)
	{
		scale = 1024.0 * 1024.0 * 1024.0;
		unit = "Gb/s";
	}
	else if (value >= 1024 * 1024)
	{
		scale = 1024.0 * 1024.0;
		unit = "Mb/s";
	}
	else if (value >= 1024)
	{
		scale = 1024.0;
		unit = "kb/s";
	}
	else
	{
		scale = 1.0;
		unit = "b/s";
	}

	value /= scale;
    
    if (unit == "b/s")
    {
        return value.toFixed(0) + " " + unit;
    }
    else
    {
        return value.toFixed(1).replace(/\.?0*$/, '') + " " + unit;
    }
}

function LoadData()
{
	xhttp.open("GET", "/speed.json", true);
	xhttp.setRequestHeader("Access-Control-Allow-Origin", "*");
	xhttp.setRequestHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
	xhttp.setRequestHeader("contentType", "application/json; charset=utf-8");
	xhttp.overrideMimeType("application/json");
	xhttp.send();
}
