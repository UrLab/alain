import paho.mqtt.client as mqtt
import paho.mqtt.subscribe as subscribe
import json

from flask import Flask, request, render_template

app = Flask(__name__)


@app.route('/')
def my_form():
    led_porte = receiveColor("porte")
    led_radio = receiveColor("radioactif")
    led_infra = receiveColor("infrabel")
    return render_template('index.html', led_porte=led_porte, led_radio=led_radio, led_infra=led_infra)


@app.route('/plafond')
def my_form_plafond():
    return render_template('multicolor.html', hexa_col=receiveColor("plafond"), titre="Plafond")


@app.route('/plafond', methods=['POST'])
def my_form_post_plafond():
    hexa_coul = request.form['text']
    if hexa_coul != "":
        coul_r = int(hexa_coul[1:3], 16)
        coul_g = int(hexa_coul[3:5], 16)
        coul_b = int(hexa_coul[5:7], 16)

        sendColor(coul_r, coul_g, coul_b, "plafond")
        return render_template('multicolor.html', hexa_col=receiveColor("plafond"), titre="Plafond")
    else:
        sendColor(0, 0, 0, "plafond")
        return render_template('multicolor.html')


@app.route('/porte')
def my_form_porte():
    return render_template('on_off.html', titre="Porte")


@app.route('/radioactif')
def my_form_radioactif():
    return render_template('on_off.html', titre="Radioactif")


@app.route('/infrabel')
def my_form_infrabel():
    return render_template('on_off.html', titre="Infrabel")


@app.route("/background_process", methods=['POST'])
def backend():
    led_state = request.form.get("on")
    topic = json.loads(request.form.get("topic")).lower()
    print(led_state)
    if led_state == "true":
        sendColor(255, 255, 255, topic)
    else:
        sendColor(0, 0, 0, topic)
    return led_state


def sendColor(r, g, b, topic):
    client = mqtt.Client()
    client.connect("hal.lan")
    client.publish("color/{}".format(topic), payload=bytes([r, g, b]), qos=1, retain=True)


def receiveColor(topic):
    messages = subscribe.simple("color/{}".format(topic), qos=1, retained=True, hostname="hal.lan")
    color = "#" + messages.payload.hex()
    return color


if __name__ == '__main__':
    app.run(debug=True)
