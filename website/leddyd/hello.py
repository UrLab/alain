import paho.mqtt.client as mqtt
import paho.mqtt.subscribe as subscribe
import json

from flask import Flask, request, render_template

app = Flask(__name__)


def getColorValue(hexa):
    hexa = hexa[1:]
    r = int(hexa[:2], 16)
    v = int(hexa[2:4], 16)
    b = int(hexa[4:], 16)
    colorValue = (0.299*r + 0.587*v + 0.144*b)/255

    return colorValue


@app.route('/')
def my_form():

    buttons = [
        {
            'name': 'plafond',
            'btn-tag': 'btn-plafond',
            'btn-icon': 'fa-door-open',
            'link': '/plafond',
            'color': receiveColor("plafond"),
            'intColor': getColorValue(receiveColor("plafond")),
            'on': receiveColor("plafond") != '#000000',
            'anim': receiveAnim("plafond") != '#000000',
            'multicolor': True
        },
        {
            'name': 'Alcool',
            'btn-tag': 'btn-alcool',
            'btn-icon': 'fa-glass-cheers',
            'link': '/alcool',
            'color': receiveColor("alcool"),
            'intColor': getColorValue(receiveColor("alcool")),
            'on': receiveColor("alcool") != '#000000',
            'anim': receiveAnim("alcool") != '#000000',
            'multicolor': True
        },
        {
            'name': 'Infrabel',
            'btn-tag': 'btn-infrabel',
            'btn-icon': 'fa-hard-hat',
            'link': '/infrabel',
            'color': '#EF930A',
            'on': receiveColor("infrabel")[:3] != '#00',
            'multicolor': False
        },
        {
            'name': 'radioactif',
            'btn-tag': 'btn-radioactif',
            'btn-icon': 'fa-radiation-alt',
            'link': '/radioactif',
            'color': '#8b00ff',
            'on': receiveColor("radioactif")[:3] != '#00',
            'multicolor': False
        },
        {
            'name': 'Exterieur',
            'btn-tag': 'btn-exterieur',
            'btn-icon': 'fa-campground',
            'link': '/exterior',
            'color': '#04A200',
            'on': receiveColor("exterior")[:3] != '#00',
            'multicolor': False
        },
        {
            'name': 'Porte',
            'btn-tag': 'btn-porte',
            'btn-icon': 'fa-door-open',
            'link': '/porte',
            'color': '#04A200',
            'on': receiveColor("porte")[:3] != '#00',
            'multicolor': False
        }
    ]

    return render_template(
        'index.html',
        all_buttons=buttons
        )


@app.route('/plafond')
def my_form_plafond():
    return render_template(
        'multicolor.html', hexa_col=receiveColor("plafond"),
        anim_running=receiveAnim("plafond"), titre="Plafond")


@app.route('/plafond', methods=['POST'])
def my_form_post_plafond():
    anim = request.form.get('anim')
    if anim == "Rainbow":
        sendColor(255, 255, 255, "animation/plafond")
    else:
        hexa_coul = request.form['text']
        coul_r = int(hexa_coul[1:3], 16)
        coul_g = int(hexa_coul[3:5], 16)
        coul_b = int(hexa_coul[5:7], 16)

        sendColor(0, 0, 0, "animation/plafond")
        sendColor(coul_r, coul_g, coul_b, "color/plafond")
    return render_template(
        'multicolor.html', hexa_col=receiveColor("plafond"),
        anim_running=receiveAnim("plafond"), titre="Plafond")


@app.route('/alcool')
def my_form_alcool():
    return render_template(
        'multicolor.html', hexa_col=receiveColor("alcool"),
        anim_running=receiveAnim("alcool"), titre="Alcool")


@app.route('/alcool', methods=['POST'])
def my_form_post_alcool():
    anim = request.form.get('anim')
    if anim == "Rainbow":
        sendColor(255, 255, 255, "animation/alcool")
    else:
        hexa_coul = request.form['text']
        coul_r = int(hexa_coul[1:3], 16)
        coul_g = int(hexa_coul[3:5], 16)
        coul_b = int(hexa_coul[5:7], 16)

        sendColor(0, 0, 0, "animation/alcool")
        sendColor(coul_r, coul_g, coul_b, "color/alcool")
    return render_template(
        'multicolor.html', hexa_col=receiveColor("alcool"),
        anim_running=receiveAnim("alcool"), titre="Alcool")


@app.route('/porte')
def my_form_porte():
    led_porte = receiveColor("porte")
    return render_template(
        'on_off.html', led=led_porte[:3] != '#00', titre="Porte")


@app.route('/radioactif')
def my_form_radioactif():
    led_radio = receiveColor("radioactif")
    return render_template(
        'on_off.html', led=led_radio[:3] != '#00', titre="Radioactif")


@app.route('/infrabel')
def my_form_infrabel():
    led_infra = receiveColor("infrabel")
    return render_template(
        'on_off.html', led=led_infra[:3] != '#00', titre="Infrabel")


@app.route('/exterior')
def my_form_exterior():
    led_exte = receiveColor("exterior")
    return render_template(
        'on_off.html', led=led_exte[:3] != '#00', titre="Exterior")


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
    try:
        messages = subscribe.simple(
            "color/{}".format(topic), qos=1, retained=True, hostname="hal.lan")
        color = "#" + messages.payload.hex()
        if len(color) != 7:
            raise ValueError('Hexadecimal value too short :' + color)
        return color
    except Exception as e:
        print("Error :", e)
        return "#000000"


def receiveAnim(topic):
    try:
        messages = subscribe.simple(
            "animation/{}".format(topic), qos=1,
            retained=True, hostname="hal.lan")
        color = "#" + messages.payload.hex()
        if len(color) != 7:
            raise ValueError('Hexadecimal value too short')
        return color
    except Exception as e:
        print("Error :", e)
        return "#000000"


if __name__ == '__main__':
    app.run(debug=True)
