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
        coul_r = int(hexa_coul[1:3],16)
        coul_g = int(hexa_coul[3:5],16)
        coul_b = int(hexa_coul[5:7],16)

        sendColor(coul_r, coul_g, coul_b)
        return render_template('home.html', hexa_col = hexa_coul)
    else:
        sendColor(0, 0, 0)
        return render_template('home.html')


def sendColor(r, g, b):
    client = mqtt.Client()
    client.connect("hal.lan")
    client.publish("color/plafond", payload=bytes([r, g, b]), qos=1, retain=True)

if __name__ == '__main__':
   app.run(debug = True)
