const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 Heart Rate Monitoring</title>
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      h1 { font-size: 1.5rem; }
      .textareaStyle {
        width: 295px;
        height: 70px;
        border:solid  1px #DCDCDC;
        box-shadow: 0px 0px 3px 0px #DCDCDC;
        margin: 5px auto;
      }
      .divStyle {
        border: 1px solid #DCDCDC;
        border-radius: 10px;
        width: 300px;
        height: 50px;
        margin: 0px auto;
        box-shadow: 0px 2px 3px 0px #DCDCDC;
        position: relative;
      }
      .child {
        width: 300px;
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
      }
      #gambarNormal, #gambarTinggi {
        width: 200px;
        display: none;
        margin: 8px auto 16px;
      }
    </style>
  </head>
  
  <body> 
    <h1 style="color:#00878F;font-family:consolas;">ESP32 HEART RATE MONITORING</h1>

    <img id="gambarNormal" src="https://media1.tenor.com/m/7Wr359XtEtEAAAAd/uma-musume-meep.gif">
    <img id="gambarTinggi" src="https://media1.tenor.com/m/HubSLmav3QsAAAAC/oguri-cap-high-cortisol.gif">

    <canvas id="myCanvas" width="300" height="150" style="box-shadow: 0px 2px 3px 0px #DCDCDC;"></canvas>
    <br><br>
    <button type="button" id="BTN_Start_Stop" onclick="my_BTN_Start_Stop()">Start Calculating HeartBeat</button>
    <p id="show_Info" hidden></p>
    <br><br>
    <div class="divStyle">
      <div class="child">
        <img width="27" height="27" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAB4AAAAeCAYAAAA7MK6iAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAC4hAAAuIQEHW/z/AAAACXZwQWcAAAAeAAAAHgD4T+E9AAAEI0lEQVRIx52WXWxTZRjHf+9Zt9l1dGMojGlxITLARGRBogkGhpEYjfLhhTo2EtfyockCcoEao4yheGG8MHGJiWOaoKBwY0bQxZg4xobMDJSghsUtsm46JmGjp+1pQdb38YLuC9bT0/2v2vP8n//vfZ/2nPcoEWFM9YbhWuD2voiiClgBFAFh4IKGbyKxcONrWseZRo2GkWfkebcasAlYBniBEeAcwuH+ePhondajY341Bj7oLiwzsjiqYDmp1TOakPXb4mb3FKi7YIkrSx0HFqVqFDg/muCF7fFQzzj4oLuwLCuLDuAe0uvf63Jz5auWNQDwicfju0tldwHzHPReuZng8e3xUI/ap5TLl+ftSrPT29XRHwuvAViQN+sUqFVOGwXOD8TCK1VTXsEWpTiUAfSWtGzVoAxDNWbeyhZX8o+UsUSpejWTRsBQVLkUsgIyj1CKe2fIBeQRA9TsmQfMVKrQACJOrGsPfc769lbue2rdHbUnj33FS73d3P/cs07JUQO4lM41q7SU0uc3Mae8nNING6bUPCUl+J55GndxMSv27UUZhhPwJQM4m861qHrz+OeStWum1OZP+l6wuIwHqiqdgLsMkBY7h1JqPEwSCTw+H96FCycWUlEBwJUznQCUv/UmWTk5tlRBWgwrFvkOGE5lmr9mNR6fj2gwSLD5eHLXFeOLGpvAT7t2c+33P/D4fCzZ6rfjDsdikRajVusbQMqHwKIt1QD0Hvmaf35snTLegsVluIuLiQ8NEbrYzbn6/QAse30P2fn5KXbLp7Va33ABbO77y5V7d5HteHq/PDIxhdWrb+02OebB1jZEhIGW77nS+TNzH3uU6qG/p82JX73qAjAAcubMtuygQ6faiQSDRIJBon195BbNZs7yhyl5Igk+eXLce3bvPtsN5BYVWTDpWGwwjFxPnvdXYKld46qPP6Ks5mV+qX+Xh3bvItvr5VjZg1iDg6TRRSsWLk/+tIzfdLVa3xAhAIzadQ+2tgGw9JUdZHu9mH/2OIGOavCPQaeAAfxW6Awi++0SLre1gQjueXOTC2lNB0VE6gPRUOfka3c8ZvrjkQMgJ1KFXB8eYeTCbxMLSU7ABntiIB55//ard4DrtNajsUi1IN2pogZP3oKJ1lxu70iNRLqtWKSqTmudFgywTWtTJdRG4Np09f4T3zLU3kHvF4f5zzRTcUdUQm2s1To8XVFNfsu8XU2eglWGUj8AbjJTXIusC1jm6VQG26MkYJmnNVIJJDKAJjRSaQdNCwYIRM1mtOwAJJ0XELTsCETN5nRGR4dnTcxsEvROB9SdNTGzyUmmIzCAPxpu0CJvpIQKe/zRcIPTPMdggIBlfiAi70xDfdtvhT7MJCsjMIDfMt8TkboJptTVWOaBTHNsbyc7fZZfWAfgj4bqZ9L/P5FZrj+hbSxaAAAAJXRFWHRjcmVhdGUtZGF0ZQAyMDIzLTA3LTIxVDEzOjM4OjQ3KzA3OjAwEH/56gAAACV0RVh0bW9kaWZ5LWRhdGUAMjAyMy0wNy0yMVQxMzozODo0NyswNzowME/Oj94AAAAASUVORK5CYII="/>
        <label id="" style="font-size:18px;">Heart Rate</label>
        <label id="bpm_Show" style="font-size:36px;"></label>
      </div>
    </div>
    
    <br>
    <textarea id="log_BPM" name="log_BPM" class="textareaStyle" readonly></textarea>
    <br>
    <button type="button" onclick="save_To_CSV()">Save to CSV</button>
    <button type="button" onclick="clear_BPM_Log()">Clear Log</button>

    <script>
      var axis_x = 0;
      var axis_y = 0;
      var last_BPM_TimeStamp = "";
      var count_Down = 3;
      var myInterval_Count_Down;

      // Batas BPM yang dianggap "tinggi". Ubah sesuai kebutuhan.
      var batasBPM = 100;

      document.getElementById("bpm_Show").innerHTML = "&nbsp;0 BPM";

      var P_show_Info = document.getElementById("show_Info");

      setInterval(myTimer, 35);

      //------------------------------------------------------------ Processes the data received from the ESP32.
      if (!!window.EventSource) {
        var source = new EventSource('/events');

        source.addEventListener('open', function(e) {
          console.log("Events Connected");
        }, false);

        source.addEventListener('error', function(e) {
          if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
          }
        }, false);

        source.addEventListener('message', function(e) {
          console.log("message", e.data);
        }, false);

        source.addEventListener('allDataJSON', function(e) {
          console.log("allDataJSON", e.data);

          var obj = JSON.parse(e.data);
          var get_Heartbeat_Signal = obj.heartbeat_Signal;
          var get_BPM_TimeStamp = obj.BPM_TimeStamp;
          var get_BPM_Val = obj.BPM_Val;
          var get_BPM_State = obj.BPM_State;

          if (get_Heartbeat_Signal > 850) get_Heartbeat_Signal = 800;
          if (get_Heartbeat_Signal < 350) get_Heartbeat_Signal = 200;

          axis_y = map(get_Heartbeat_Signal, 350, 850, 150, 0); // 150 is the height of the canvas. Look at the canvas height in the "<body>" section.

          if (get_BPM_State == true) {
            document.getElementById("bpm_Show").innerHTML = "&nbsp;" + get_BPM_Val + " BPM";

            // Tampilkan gambar sesuai kondisi BPM, hanya saat sedang menghitung
            if (get_BPM_Val > 0) {
              if (get_BPM_Val > batasBPM) {
                document.getElementById('gambarTinggi').style.display = 'block';
                document.getElementById('gambarNormal').style.display = 'none';
              } else {
                document.getElementById('gambarTinggi').style.display = 'none';
                document.getElementById('gambarNormal').style.display = 'block';
              }
            } else {
              document.getElementById('gambarTinggi').style.display = 'none';
              document.getElementById('gambarNormal').style.display = 'none';
            }

            if (last_BPM_TimeStamp != get_BPM_TimeStamp) {
              var my_textarea = document.getElementById('log_BPM');
              var lastLog = my_textarea.textContent;
              if (lastLog == "") {
                document.getElementById('log_BPM').textContent =  "Time,BPM\r\n" + get_BPM_TimeStamp + "," + get_BPM_Val;
              }
              else {
                document.getElementById('log_BPM').textContent =  lastLog + "\r\n" + get_BPM_TimeStamp + "," + get_BPM_Val;
              }
              my_textarea.scrollTop = my_textarea.scrollHeight;
            }

            last_BPM_TimeStamp = get_BPM_TimeStamp;
          } else {
            document.getElementById("bpm_Show").innerHTML = "&nbsp;0 BPM";
            document.getElementById('gambarTinggi').style.display = 'none';
            document.getElementById('gambarNormal').style.display = 'none';
          }
        }, false);
      }
      //------------------------------------------------------------

      //------------------------------------------------------------Line graph.
      var canvas = document.getElementById('myCanvas');
      var context = canvas.getContext('2d');

      drawGraph();

      context.beginPath();
      context.lineWidth = 1;
      context.strokeStyle = '#0583F2';

      function drawGraph() {
        context.beginPath();
        context.moveTo(1,150);
        context.lineTo(1,0);
        context.moveTo(50,150);
        context.lineTo(50,0);
        context.moveTo(100,150);
        context.lineTo(100,0);
        context.moveTo(150,150);
        context.lineTo(150,0);
        context.moveTo(200,150);
        context.lineTo(200,0);
        context.moveTo(250,150);
        context.lineTo(250,0);
        context.moveTo(299,150);
        context.lineTo(299,0);

        context.moveTo(0,1);
        context.lineTo(300,1);
        context.moveTo(0,25);
        context.lineTo(300,25);
        context.moveTo(0,50);
        context.lineTo(300,50);
        context.moveTo(0,75);
        context.lineTo(300,75);
        context.moveTo(0,100);
        context.lineTo(300,100);
        context.moveTo(0,125);
        context.lineTo(300,125);
        context.moveTo(0,149);
        context.lineTo(300,149);

        context.lineWidth = 0.5;
        context.strokeStyle = '#DCDCDC';
        context.closePath();
        context.stroke();
      }
      //------------------------------------------------------------

      //------------------------------------------------------------Displays the signal from the pulse sensor on a line graph.
      function set_Axis_XY(x,y) {
        context.lineTo(x, y);
        context.stroke();
        if (axis_x > 300) {
           reset_Graph();
        }
      }
      //------------------------------------------------------------

      //------------------------------------------------------------
      function reset_Graph() {
        axis_x = 0; 
        context.clearRect(0,0,300,150);
        context.closePath();
        drawGraph();
        context.beginPath();
        context.lineWidth = 1;
        context.strokeStyle = '#0583F2';
        context.moveTo(0,axis_y);
      }
      //------------------------------------------------------------

      //------------------------------------------------------------
      function myTimer() {
        axis_x += 3;
        set_Axis_XY(axis_x, axis_y);
      }
      //------------------------------------------------------------

      //------------------------------------------------------------Scale from the signal value to the line graph.
      // Source: https://www.arduino.cc/reference/en/language/functions/math/map/ , https://forum.jquery.com/topic/jquery-map-like-arduino-map
      function map( x,  in_min,  in_max,  out_min,  out_max){
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
      }
      //------------------------------------------------------------

      //------------------------------------------------------------
      function my_BTN_Start_Stop() {
        reset_Graph();

        var my_BTN_Start_Stop_Text = document.getElementById("BTN_Start_Stop").innerHTML;
        if (my_BTN_Start_Stop_Text == "Start Calculating HeartBeat") {
          document.getElementById("BTN_Start_Stop").innerHTML = "Stop Calculating HeartBeat";
          send_BTN_Cmd("START");

          count_Down = 3;
          P_show_Info.style.display = "block";
          document.getElementById("show_Info").innerHTML = "Please Wait...";
          myInterval_Count_Down = setInterval(myTimer_Count_Down, 1000);
        } else if (my_BTN_Start_Stop_Text == "Stop Calculating HeartBeat") {
          document.getElementById("BTN_Start_Stop").innerHTML = "Start Calculating HeartBeat";
          send_BTN_Cmd("STOP");
          document.getElementById('gambarTinggi').style.display = 'none';
          document.getElementById('gambarNormal').style.display = 'none';
        }
      }
      //------------------------------------------------------------

      //------------------------------------------------------------
      function myTimer_Count_Down() {
        count_Down -= 1;
        document.getElementById("show_Info").innerHTML = "Please Wait " + (count_Down + 1);
        if (count_Down < 0) {
          P_show_Info.style.display = "none";
          myStop_Count_Down();
        }
      }

      function myStop_Count_Down() {
        clearInterval(myInterval_Count_Down);
      }
      //------------------------------------------------------------

      //------------------------------------------------------------
      function save_To_CSV() {
        var a = document.createElement('a');
        with (a) {
          href='data:text/csv;base64,' + btoa(document.getElementById('log_BPM').value);
          download='BPMLog.csv';
        }
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
      }
      //------------------------------------------------------------

      //------------------------------------------------------------
      function clear_BPM_Log() {
        let dialog_text = "Are you sure to clear the Log ?";
        if (confirm(dialog_text) == true) {
          document.getElementById('log_BPM').textContent = "";
        } 
      }
      //------------------------------------------------------------

      //------------------------------------------------------------ XMLHttpRequest to submit data.
      function send_BTN_Cmd(state) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "BTN_Comd?BTN_Start_Get_BPM=" + state, true);
        xhr.send();
      }
      //------------------------------------------------------------
    </script>
  </body>
</html>
)=====";