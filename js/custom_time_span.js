function Settings()
{
    var $leap = false;
    var $start_year = $('.start_year');
    var $start_month = $('.start_month');
    var $start_day = $('.start_day');
    var $start_hour = $('.start_hour');

    var $end_year = $('.end_year');
    var $end_month = $('.end_month');
    var $end_day = $('.end_day');
    var $end_hour = $('.end_hour');

    var $stats_type = $('.statstype');

    var $submit_button = $('.submit_button');
    document.getElementById("submit_button").onclick = show_stats;

    $stats_type.change(function(e)
    {
        var $type = $('[name="statstype"]:checked').val();
        if ($type == 'hourly')
        {
            $start_month.prop('disabled', false);
            $start_day.prop('disabled', false);
            $start_hour.prop('disabled', false);
            $end_month.prop('disabled', false);
            $end_day.prop('disabled', false);
            $end_hour.prop('disabled', false);
        }
        else if ($type == 'daily')
        {
            $start_month.prop('disabled', false);
            $start_day.prop('disabled', false);
            $start_hour.prop('disabled', true);
            $end_month.prop('disabled', false);
            $end_day.prop('disabled', false);
            $end_hour.prop('disabled', true);
        }
        else if ($type == 'monthly')
        {
            $start_month.prop('disabled', false);
            $start_day.prop('disabled', true);
            $start_hour.prop('disabled', true);
            $end_month.prop('disabled', false);
            $end_day.prop('disabled', true);
            $end_hour.prop('disabled', true);
        }
        else if ($type == 'yearly')
        {
            $start_month.prop('disabled', true);
            $start_day.prop('disabled', true);
            $start_hour.prop('disabled', true);
            $end_month.prop('disabled', true);
            $end_day.prop('disabled', true);
            $end_hour.prop('disabled', true);
        }
    });


    // start date functions

    $start_month.change(function(e)
    {
        var $this = $(this);

        if ($this.val() > 0)
        {
            $start_day.val(-1);
        }

        if ($this.val() == 1)
        {
            set_start_days(31);
        }
        else if ($this.val() == 2)
        {
            if ($leap)
            {
                set_start_days(29);
            }
            else
            {
                set_start_days(28);
            }
        }
        else if ($this.val() == 3)
        {
            set_start_days(31);
        }
        else if ($this.val() == 4)
        {
            set_start_days(30);
        }
        else if ($this.val() == 5)
        {
            set_start_days(31);
        }
        else if ($this.val() == 6)
        {
            set_start_days(30);
        }
        else if ($this.val() == 7)
        {
            set_start_days(31);
        }
        else if ($this.val() == 8)
        {
            set_start_days(31);
        }
        else if ($this.val() == 9)
        {
            set_start_days(30);
        }
        else if ($this.val() == 10)
        {
            set_start_days(31);
        }
        else if ($this.val() == 11)
        {
            set_start_days(30);
        }
        else if ($this.val() == 12)
        {
            set_start_days(31);
        }

    });

    $start_year.change(function(e)
    {
        if ($(this).val() > 0)
        {
            $leap = is_leap($start_year.val());
            $start_month.val(-1);
            $start_day.val(-1);
        }
    });

    // end date functions

    $end_month.change(function(e)
    {
        var $this = $(this);

        if ($this.val() > 0)
        {
            $end_day.val(-1);
        }

        if ($this.val() == 1)
        {
            set_end_days(31);
        }
        else if ($this.val() == 2)
        {
            if ($leap)
            {
                set_end_days(29);
            }
            else
            {
                set_end_days(28);
            }
        }
        else if ($this.val() == 3)
        {
            set_end_days(31);
        }
        else if ($this.val() == 4)
        {
            set_end_days(30);
        }
        else if ($this.val() == 5)
        {
            set_end_days(31);
        }
        else if ($this.val() == 6)
        {
            set_end_days(30);
        }
        else if ($this.val() == 7)
        {
            set_end_days(31);
        }
        else if ($this.val() == 8)
        {
            set_end_days(31);
        }
        else if ($this.val() == 9)
        {
            set_end_days(30);
        }
        else if ($this.val() == 10)
        {
            set_end_days(31);
        }
        else if ($this.val() == 11)
        {
            set_end_days(30);
        }
        else if ($this.val() == 12)
        {
            set_end_days(31);
        }

    });

    $end_year.change(function(e)
    {
        if ($(this).val() > 0)
        {
            $leap = is_leap($end_year.val());
            $end_month.val(-1);
            $end_day.val(-1);
        }
    });


    function set_start_days(num)
    {
        $start_day.empty();
        for (var i = 1; i <= num; i++)
        {
            if (i < 10) // Put 0 in front of the single digits
                $start_day.append("<option value='0" + i + "'>0" + i + "</option>")
            else
                $start_day.append("<option value='" + i + "'>" + i + "</option>")
        }
    }

    function set_end_days(num)
    {
        $end_day.empty();
        for (var i = 1; i <= num; i++)
        {
            if (i < 10)
                $end_day.append("<option value='0" + i + "'>0" + i + "</option>")
            else
                $end_day.append("<option value='" + i + "'>" + i + "</option>")
        }
    }

    function show_stats()
    {
        if ($start_year.val()=="start_year" || $end_year.val()=="end_year")
        {
            alert("Please select the start and end dates");
            return;
        }
        
        statstype = $('[name="statstype"]:checked').val();
        str = "/";
        str += $('[name="statstype"]:checked').val();
        str += "/";
        str += "start=";
        str += $start_year.val();
        if (statstype == 'monthly' || statstype == 'daily' || statstype == 'hourly')
        {
            if ($start_month.val()=="start_month" || $end_month.val()=="end_month")
            {
                alert("Please select the start and end dates");
                return;
            }            
            
            str += "-";
            str += $start_month.val();
            if (statstype == 'daily' || statstype == 'hourly')
            {
                if ($start_day.val()=="start_day" || $end_day.val()=="end_day")
                {
                    alert("Please select the start and end dates");
                    return;
                }
                
                str += "-";
                str += $start_day.val();
                if (statstype == 'hourly')
                {
                    if ($start_hour.val()=="start_hour" || $end_hour.val()=="end_hour")
                    {
                        alert("Please select the start and end dates");
                        return;
                    }
                
                    str += "-";
                    str += $start_hour.val();
                }
            }
        }
        str += "&end=";
        str += $end_year.val();
        if (statstype == 'monthly' || statstype == 'daily' || statstype == 'hourly')
        {
            str += "-";
            str += $end_month.val();
            if (statstype == 'daily' || statstype == 'hourly')
            {
                str += "-";
                str += $end_day.val();
                if (statstype == 'hourly')
                {
                    str += "-";
                    str += $end_hour.val();
                }
            }
        }
        str += "&charttype=";
        str += $('[name="charttype"]:checked').val();
        str += "&upcolor="
        str += $('[name="up_color"]').val();
        str += "&downcolor="
        str += $('[name="down_color"]').val();
        var win = window.open(str, '_blank');
        win.focus();
    };

    function is_leap(yr)
    {
        return (yr % 400) ? ((yr % 100) ? ((yr % 4) ? false : true) : false) : true;
    }
}
 
