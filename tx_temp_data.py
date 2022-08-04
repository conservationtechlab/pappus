# SPDX-FileCopyrightText: 2018 Brent Rubell for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""
Example for using the RFM9x Radio with Raspberry Pi.

Learn Guide: https://learn.adafruit.com/lora-and-lorawan-for-raspberry-pi
Author: Brent Rubell for Adafruit Industries
"""
# RFM9X GitHub Repo : https://github.com/adafruit/Adafruit_CircuitPython_RFM9x

# Import Python System Libraries
import time
import subprocess

# Import Blinka Libraries
import busio
from digitalio import DigitalInOut, Direction, Pull
import board
# Import the SSD1306 module.
import adafruit_ssd1306
# Import RFM9x
import adafruit_rfm9x
# For accessing CPU temperature data
from gpiozero import CPUTemperature


btnA = DigitalInOut(board.D5)
btnA.direction = Direction.INPUT
btnA.pull = Pull.UP


def configDisplay():
    # Create the I2C interface.
    i2c = busio.I2C(board.SCL, board.SDA)

    # 128x32 OLED Display
    reset_pin = DigitalInOut(board.D4)
    display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c, reset=reset_pin)
    # Clear the display.
    display.fill(0)
    display.show()
    return display


def getTemperature():
    cpu = CPUTemperature()
    return cpu.temperature  # float, in degrees celsius


def configRadio():
    CS = DigitalInOut(board.CE1)
    RESET = DigitalInOut(board.D25)
    spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
    rfm9x = adafruit_rfm9x.RFM9x(spi, CS, RESET, 915.0)
    rfm9x.tx_power = 23
    rfm9x.ack_delay = 0.1
    rfm9x.node = ord('t')  # t as in tx
    rfm9x.destination = ord('r')  # r as in rx
    return rfm9x


def sendTemp(rfm9x):
    curr_time = int(time.time())
    temp_str = str(curr_time) + ": " + str(getTemperature())
    temp_data = bytes(temp_str, "utf-8")
    return rfm9x.send_with_ack(temp_data)


# returns hostname, first ip, and second ip
def getHostData():  # derived from scrubcam "little_readout.py"
    cmd = "hostname"
    hostname = subprocess.check_output(cmd, shell=True).decode("utf-8")
    cmd = "hostname -I | cut -d' ' -f1"
    ip1 = subprocess.check_output(cmd, shell=True).decode("utf-8")
    cmd = "hostname -I | cut -d' ' -f2"
    ip2 = subprocess.check_output(cmd, shell=True).decode("utf-8")
    return hostname, ip1, ip2


def getIP1():
    host, i1, i2 = getHostData()
    return i1


if __name__ == '__main__':
    rfm9x = configRadio()

    display = configDisplay()
    spacing = int(display.height/4)

    hostname, ip1, ip2 = getHostData()
    host_text = "Host: " + hostname
    ip1_text = "IP1: " + ip1
    ip2_text = "IP2: " + ip2

    while True:
        if not btnA.value:
            display.fill(0)
            display.text(ip1_text, 0, spacing, 1)
            display.show()
            break
        time.sleep(3)
        got_ack = sendTemp(rfm9x)
        display.fill(0)
        display.show()
        display.text(host_text, 0, 0, 1)
        if len(ip1) < 2:
            ip1_text = 'IP1: ' + getIP1()  # update ip if none at start
        display.text(ip1_text, 0, spacing, 1)
        if (got_ack):
            display.text('Sent Successfully', 0, 3*spacing, 1)
        else:
            display.text('Unable to reach destination', 0, 3*spacing, 1)
        display.show()
