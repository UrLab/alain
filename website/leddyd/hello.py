import paho.mqtt.client as mqtt
from flask import Flask, request, render_template

app = Flask(__name__)


@app.route('/')
def my_form():
    return render_template('home.html')


@app.route('/', methods=['POST'])
def my_form_post():
    hexa_coul = request.form['text']
    if hexa_coul != "":
        coul_r = int(hexa_coul[0:2],16)
        coul_g = int(hexa_coul[2:4],16)
        coul_b = int(hexa_coul[4:],16)

        sendColor(coul_r, coul_g, coul_b)
        return render_template('home.html', hexa_col = "#" + hexa_coul)
    else:
        sendColor(0, 0, 0)
        return render_template('home.html')


def sendColor(r, g, b):
    client = mqtt.Client()
    client.connect("hal.lan")
    client.publish("color/plafond", payload=chr(r)+chr(g)+chr(b), qos=1, retain=True)
    print("SEND " + chr(r)+chr(g)+chr(b))


if __name__ == '__main__':
   app.run(debug = True)
