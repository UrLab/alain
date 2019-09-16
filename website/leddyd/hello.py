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
    led_exterior = receiveColor("exterior")
    return render_template('index.html', led_porte=led_porte, led_radio=led_radio, led_infra=led_infra, led_exterior=led_exterior)


@app.route('/plafond')
def my_form_plafond():
    return render_template('multicolor.html', hexa_col=receiveColor("plafond"), titre="Plafond")


@app.route('/plafond', methods=['POST'])
def my_form_post_plafond():
    hexa_coul = request.form['text']
    anim = request.form.get('anim')
    if anim == "Rainbow":
        sendColor(255, 255, 255, "animation/plafond")
    else:
        coul_r = int(hexa_coul[1:3], 16)
        coul_g = int(hexa_coul[3:5], 16)
        coul_b = int(hexa_coul[5:7], 16)

        sendColor(0, 0, 0, "animation/plafond")
        sendColor(coul_r, coul_g, coul_b, "color/plafond")
    return render_template('multicolor.html', hexa_col=receiveColor("plafond"), titre="Plafond")


@app.route('/alcool')
def my_form_alcool():
    return render_template('multicolor.html', hexa_col=receiveColor("alcool"), titre="Alcool")


@app.route('/alcool', methods=['POST'])
def my_form_post_alcool():
    hexa_coul = request.form['text']
    anim = request.form.get('anim')
    if anim == "Rainbow":
        sendColor(255, 255, 255, "animation/alcool")
    else:
        coul_r = int(hexa_coul[1:3], 16)
        coul_g = int(hexa_coul[3:5], 16)
        coul_b = int(hexa_coul[5:7], 16)

        sendColor(0, 0, 0, "animation/alcool")
        sendColor(coul_r, coul_g, coul_b, "color/alcool")
    return render_template('multicolor.html', hexa_col=receiveColor("alcool"), titre="Alcool")


@app.route('/porte')
def my_form_porte():
    led_porte = receiveColor("porte")
    return render_template('on_off.html', led=not led_porte[:3] == '#00', titre="Porte")


@app.route('/radioactif')
def my_form_radioactif():
    led_radio = receiveColor("radioactif")
    return render_template('on_off.html', led=not led_radio[:3] == '#00', titre="Radioactif")


@app.route('/infrabel')
def my_form_infrabel():
    led_infra = receiveColor("infrabel")
    return render_template('on_off.html', led=not led_infra[:3] == '#00', titre="Infrabel")


@app.route('/exterior')
def my_form_exterior():
    return render_template('on_off.html', titre="Exterior")


@app.route("/background_process", methods=['POST'])
def backend():
    led_state = request.form.get("on")
    topic = json.loads(request.form.get("topic")).lower()
    if led_state == "true":
        sendColor(255, 255, 255, "color/{}".format(topic))
    else:
        sendColor(0, 0, 0, "color/{}".format(topic))
    return led_state


def sendColor(r, g, b, topic):
    client = mqtt.Client()
    client.connect("hal.lan")
    client.publish(topic, payload=bytes([r, g, b]), qos=1, retain=True)


def receiveColor(topic):
    messages = subscribe.simple("color/{}".format(topic), qos=1, retained=True, hostname="hal.lan")
    color = "#" + messages.payload.hex()
    return color


if __name__ == '__main__':
    app.run(debug=True)
