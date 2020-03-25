from __future__ import unicode_literals

from kivy.support import install_twisted_reactor
from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.textinput import TextInput
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.floatlayout import FloatLayout
from kivy.uix.gridlayout import GridLayout
from kivy.properties import ObjectProperty, ListProperty, DictProperty
# from kivy.uix.listview import ListItemButton, ListView
from kivy.uix.button import Button

# install_twisted_rector must be called before importing the reactor
install_twisted_reactor()

# A Simple Client that send messages to the Echo Server
from twisted.internet import reactor, protocol
from twisted.internet.protocol import DatagramProtocol
from twisted.internet.error import AlreadyCalled
import time

red = (255, 0, 0, 255)
green = (0, 255, 0, 255)
yellow = (255, 255, 0, 255)
fontsize = 70
patient_timeout = 10

def callback(button):
    button.background_color = yellow
    print('called back')
    print(button)

class HeartbeatReceiver(DatagramProtocol):
    def __init__(self, name, app):
        self.name = name
        self.app = app

        # {'patient': 0, 'status':'heartbeat', 'callback':func}
        self.patients = {}

    # def startProtocol(self):
    #     "Called when transport is connected"
    #     self.transport.joinGroup('224.0.0.1')
    #     pass

    def stopProtocol(self):
        "Called after all transport is teared down"
        pass

    def datagramReceived(self, data, hostport):
        host, port = hostport
        now = time.localtime(time.time())
        timeStr = str(time.strftime("%y/%m/%d %H:%M:%S",now))

        patient, status = data.decode('utf8').rstrip('\n').split(':')
        print('patient', patient, 'status', status)

        if patient not in self.patients:
            button = Button(text='Patient #{}'.format(patient), font_size=fontsize)


            self.patients[patient] = {
                'button': button,
                'status': status,
                'callback': reactor.callLater(patient_timeout, callback, button=button)
            }
            self.app.layout.add_widget(button)
        else:
            try:
                self.patients[patient]['callback'].cancel()
            except AlreadyCalled:
                pass

        self.patients[patient]['callback'] = reactor.callLater(
            patient_timeout,
            callback,
            button=self.patients[patient]['button']
        )
        color = green if status == 'heartbeat' else red
        self.patients[patient]['button'].background_color = color

        self.app.print_message(data.decode('utf-8'))



class MyApp(App):
    main = ObjectProperty()

    def build(self):
        return app


# A simple kivy App, with a textbox to enter messages, and
# a large label to display all the messages received from
# the server
class TwistedClientApp(App):
    connection = None
    textbox = None
    label = None

    def build(self):
        root = self.setup_gui()
        self.connect_to_server()
        return root

    def setup_gui(self):
        self.textbox = TextInput(size_hint_y=.1, multiline=False)
        self.textbox.bind(on_text_validate=self.send_message)
        self.label = Label(text='connecting...\n')
        self.layout = GridLayout(cols=3)
        self.layout.add_widget(self.label)

        for p in range(18, 25):
            button = Button(
                text='Patient {}'.format(p),
                background_color=green,
                font_size=fontsize
            )
            self.layout.add_widget(button)

        # self.layout.add_widget(self.textbox)
        return self.layout

    def connect_to_server(self):
        # reactor.connectTCP('localhost', 8000, EchoClientFactory(self))
        reactor.listenMulticast(8888, HeartbeatReceiver("listener", self), listenMultiple=True)

    def on_connection(self, connection):
        self.print_message("Connected successfully!")
        self.connection = connection

    def send_message(self, *args):
        msg = self.textbox.text
        if msg and self.connection:
            self.connection.write(msg.encode('utf-8'))
            self.textbox.text = ""

    def print_message(self, msg):
        self.label.text += "{}\n".format(msg)


if __name__ == '__main__':
    TwistedClientApp().run()
