# Evan Widloski

import machine
import network
import socket
from time import sleep
from utime import ticks_ms, ticks_diff

# ----- Options -----

# patient no.
patient = 25

# raise alarm if ventilator goes this long without inspiration (ms)
time_threshold = 10000
# adc threshold value for inspiration
breath_threshold = 2048
# rate to change alarm frequency (ms)
alarm_rate = 500
# network heartbeat to master (ms)
heartbeat_threshold = 5000

# alarm settings
freq1 = 500
freq2 = 600
intensity = 50

# ----- Wifi config -----

sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.config(dhcp_hostname='micropython')
sta_if.connect('dd-wrt')

# ----- Configure network messaging -----

# set up UDP broadcast
# compute broadcast address
# FIXME - do proper broadcast computation
addr, gateway = sta_if.ifconfig()[:2]
broadcast_port = 8888
broadcast = []
for oct_gw, oct_addr in zip(gateway.split('.'), addr.split('.')):
    if oct_gw != '255':
        broadcast.append('255')
    else:
        broadcast.append(oct_addr)
broadcast = '.'.join(broadcast)
print('computed broadcast:', broadcast)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def send_message(msg):
    if sta_if.isconnected():
        sock.sendto('{}:{}'.format(patient, msg), (broadcast, broadcast_port))
    else:
        print('warning: tried to send message but WiFi is not ready')

# ----- Setup IO/ADC/PWM -----

adc = machine.ADC(machine.Pin(36))

# alarm
pwm = machine.PWM(machine.Pin(12))
led = machine.Pin(2, machine.Pin.OUT)
# verify alarm with sound
pwm.duty(intensity)
led.value(True)
pwm.freq(freq1)
sleep(.25)
led.value(False)
pwm.freq(freq2)
sleep(.25)
pwm.duty(0)

# ----- Main loop -----

time_heartbeat = ticks_ms()
time_last_breath = ticks_ms()
alarm_raised = False

# loop forever
while True:
    sleep(.1)

    adc_val = adc.read()
    print(adc_val)

    # check if ventilator is inspirating
    if adc_val > breath_threshold:
        time_last_breath = ticks_ms()

    # no inspiration recently, raise alarm!
    if ticks_diff(ticks_ms(), time_last_breath) > time_threshold:
        print('warning!')
        send_message('alarm')
        alarm_raised = True

    # send heartbeats to master
    if ticks_diff(ticks_ms(), time_heartbeat) > heartbeat_threshold:
        time_heartbeat = ticks_ms()
        if alarm_raised:
            send_message('alarm')
        else:
            send_message('heartbeat')

    # beep alarm at alternating frequencies
    if alarm_raised:
        pwm.duty(intensity)
        if (ticks_ms() % (2 * alarm_rate)) > alarm_rate:
            pwm.freq(freq1)
            led.value(True)
        else:
            led.value(False)
            pwm.freq(freq2)
