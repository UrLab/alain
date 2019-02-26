import paho.mqtt.client as mqtt
from flask import Flask, request, render_template

app = Flask(__name__)


@app.route('/')
def my_form():
    return render_template('home.html')


@app.route('/plafond')
def my_form_plafond():
    return render_template('multicolor.html')


@app.route('/plafond', methods=['POST'])
def my_form_post_plafond():
    hexa_coul = request.form['text']
    if hexa_coul != "":
        coul_r = int(hexa_coul[1:3], 16)
        coul_g = int(hexa_coul[3:5], 16)
        coul_b = int(hexa_coul[5:7], 16)

        sendColor(coul_r, coul_g, coul_b, "plafond")
        return render_template('multicolor.html', hexa_col=hexa_coul)
    else:
        sendColor(0, 0, 0, "plafond")
        return render_template('multicolor.html')


@app.route('/porte')
def my_form_porte():
    return render_template('on_off.html')


@app.route('/porte', methods=['POST'])
def my_form_post_porte():
    led_on = request.form['switch']
    if led_on == "on":
        sendColor(255, 255, 255, "porte")
    elif led_on == "off":
        sendColor(0, 0, 0, "porte")
    return render_template("on_off.html", led=True if led_on == "on" else False)


@app.route('/radioactif')
def my_form_radioactif():
    return render_template('on_off.html')


@app.route('/radioactif', methods=['POST'])
def my_form_post_radioactif():
    led_on = request.form['switch']
    if led_on == "on":
        sendColor(255, 255, 255, "radioactif")
    elif led_on == "off":
        sendColor(0, 0, 0, "radioactif")
    return render_template("on_off.html", led=True if led_on == "on" else False)


@app.route('/infrabel')
def my_form_infrabel():
    return render_template('on_off.html')


@app.route('/infrabel', methods=['POST'])
def my_form_post_infrabel():
    led_on = request.form['switch']
    if led_on == "on":
        sendColor(255, 255, 255, "infrabel")
    elif led_on == "off":
        sendColor(0, 0, 0, "infrabel")
    return render_template("on_off.html", led=True if led_on == "on" else False)


def sendColor(r, g, b, topic):
    client = mqtt.Client()
    client.connect("hal.lan")
    client.publish("color/{}".format(topic), payload=bytes([r, g, b]), qos=1, retain=True)



if __name__ == '__main__':
    app.run(debug=True)
