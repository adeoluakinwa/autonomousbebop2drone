from pyparrot.Bebop import Bebop
from flask import Flask
from flask import request

Istakenoff = False
Islanded = True

#Sets up connection to the drone, then starts the webserver
bebop = Bebop(drone_type="Bebop2")
print("connecting")
success = bebop.connect(10)
print(success)
print("sleeping")
bebop.smart_sleep(2)
bebop.ask_for_state_update()

def pick_free_direction(min_left,min_right,max_left,max_right):
    print("left or right?")
    if IsMaxRight(max_right) and IsMaxLeft(max_left):
        print("Turning left")
        return "left"
    if IsMaxLeft(max_left):
        print("Turning left")
        return "left"
    if IsMaxRight(max_right):
        print("Turning right")
        return "right"

    if Is_leftFree(min_left) and Is_rightFree(min_right):
        if max_right > max_left:
            print("Turning right")
            return "right"
        elif max_left > max_right:
            print("Turning left")
            return "left"
    if Is_leftFree(min_left):
        print("Turning left")
        return "left"

    if Is_rightFree(min_right):
        print("Turning right")
        return "right" 
        
    print("blocked")
    return "blocked"

def IsMaxRight(max_right):
    if max_right > 15000:
        return True
    return False

def IsMaxLeft(max_left):
    if max_left > 15000:
        return True
    return False

def Is_leftFree(min_left):
    if min_left > 500:
        return True
    return False

def Is_rightFree(min_right):
    if min_right > 500:
        return True
    return False

app = Flask(__name__)


@app.route('/', methods=['Get'])
def control():
    return """
    <!DOCTYPE html>
    <head>
        <style>
            body {background-color: powderblue;}
            h1   {color: blue;}
            p    {color: red;}
            .center {
                height: auto;
                margin: 0 auto;
            }
            button {
        background-color: #4CAF50; /* Green */
        border: none;
        color: white;
        padding: 15px 32px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        }
        </style>
        <title>Drone Demo</title>
    </head>
    <body> 
        <div style="height: 5em;width: 30em; background-color: black; margin: 0 auto; text-align: center;border-radius: 0.5em">
            <h1 style="color :white; font-size: 4em">Drone Demo</h1>
        </div>
        <div style="height:40em;width: 50em;background-color: darkgray; margin: 0 auto; margin-top: 2em">
            <div style="margin: 1em; text-align: center; font-size: 3em; padding-top: 0.5em">
                Video Feed
            </div>
            <div style="height:25em; width: 25em; background-color:whitesmoke; border-radius:0.2em; margin: 0 auto">
                <video></video>
            </div>
            <div style="text-align: center;  margin: 2em">  
                <button onclick="takeoff()">Take Off</button>
                <button onclick="land()">Land</button>
            </div> 
        </div>
        <script>
            function takeoff()
            {
                const HTTP = new XMLHttpRequest()
                const url = 'http://192.168.42.5:5000/takeoff'
                HTTP.open("GET", url);
                HTTP.send();
            }
            function land()
            {
                const HTTP = new XMLHttpRequest()
                const url = 'http://192.168.42.5:5000/land'
                HTTP.open("GET", url);
                HTTP.send();
            }
        </script>
    </body>
    """

@app.route('/land', methods=['GET'])
def land():
    global Islanded
    global Istakenoff
    print ("Drone Landing...")
    bebop.safe_land(5)
    Islanded = True
    Istakenoff = False
    return "Landing drone..."

@app.route('/takeoff', methods=['GET'])
def takeoff():
    global Islanded
    global Istakenoff
    print ("Drone Takeoff...")
    bebop.safe_takeoff(10)
    bebop.set_max_tilt(5)
    bebop.set_max_vertical_speed(1)
    Istakenoff = True
    Islanded = False
    return "Drone Takeoff..."

@app.route('/shutdown', methods=['GET'])
def shutdown():
    print ("Initiating Shut down sequence...")
    print ("Drone Landing...")
    bebop.safe_land(5)
    print ("Disconnecting from drone...")
    bebop.disconnect()
    print("Waiting 5 seconds before shuting down server...")
    print ("Server shutting down...")
    shutdown_server()
    return " "

@app.route('/nav', methods=['POST'])
def post():
    if Istakenoff == True:
        content = request.get_json()
        min_ahead = float(content['min_ahead'])
        print("min ahead: ")
        print(min_ahead)
        min_left = float(content['min_left'])
        print("min left: ")
        print(min_left)
        min_right = float(content['min_right'])
        print("Min Right: ")
        print(min_right)
        max_left = float(content['max_left'])
        print("Max Left: ")
        print(max_left)
        max_right = float(content['max_right'])
        print("max right: ")
        print(max_right)
        if min_ahead > 500:
            print ("moving ahead")
            #bebop.fly_direct(roll=0, pitch=20, yaw=0, vertical_movement=0, duration=2)
        elif min_ahead <= 500:
            direction = pick_free_direction(min_left,min_right,max_left,max_right)
            if direction == "left":
                print("Turning left")
                bebop.fly_direct(roll=0, pitch=0, yaw=-100, vertical_movement=0, duration=2)
            if direction == "right":
                print("Turning Right")
                bebop.fly_direct(roll=0, pitch=0, yaw=100, vertical_movement=0, duration=2)
            if direction == "blocked":
                print ("blocked")
                #bebop.fly_direct(roll=0, pitch=0, yaw=0, vertical_movement=0, duration=2)
        return "200"
    else:
        content = request.get_json()
        min_ahead = float(content['min_ahead'])
        print("min ahead: ")
        print(min_ahead)
        min_left = float(content['min_left'])
        print("min left: ")
        print(min_left)
        min_right = float(content['min_right'])
        print("Min Right: ")
        print(min_right)
        max_left = float(content['max_left'])
        print("Max Left: ")
        print(max_left)
        max_right = float(content['max_right'])
        print("max right: ")
        print(max_right)
        if min_ahead > 500:
            print ("moving ahead")
            #bebop.fly_direct(roll=0, pitch=20, yaw=0, vertical_movement=0, duration=2)
        elif min_ahead <= 500:
            direction = pick_free_direction(min_left,min_right,max_left,max_right)
            if direction == "left":
                print("Turning left")
                #bebop.fly_direct(roll=0, pitch=0, yaw=-100, vertical_movement=0, duration=2)
            if direction == "right":
                print("Turning Right")
                #bebop.fly_direct(roll=0, pitch=0, yaw=100, vertical_movement=0, duration=2)
            if direction == "blocked":
                print("Blocked")
        return "200"
                #bebop.fly_direct(roll=0, pitch=0, yaw=0, vertical_movement=0, duration=2)
#app.run(host='192.168.1.135', port=5000,debug=True)
app.run(host='192.168.42.5', port=5000)
#app.run(host='192.168.1.2', port=5000)

def shutdown_server():
    func = request.environ.get('werkzeug.server.shutdown')
    if func is None:
        raise RuntimeError('Not running with the Werkzeug Server')
    func()

