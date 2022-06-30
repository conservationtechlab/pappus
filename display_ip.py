# SPDX-FileCopyrightText: 2018 Brent Rubell for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""
Example for using the RFM9x Radio with Raspberry Pi.

Learn Guide: https://learn.adafruit.com/lora-and-lorawan-for-raspberry-pi
Author: Brent Rubell for Adafruit Industries
"""
# Import Python System Libraries
import signal
import subprocess

# Import Blinka Libraries
import busio
from digitalio import DigitalInOut
import board
# Import the SSD1306 module.
import adafruit_ssd1306


def configDisplay():
    # Create the I2C interface.
    i2c = busio.I2C(board.SCL, board.SDA)

    # 128x32 OLED Display
    reset_pin = DigitalInOut(board.D4)
    display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c, reset=reset_pin)
    # Clear the display
    display.fill(0)
    display.show()
    return display


display = configDisplay()


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


def getHost():
    host, i1, i2 = getHostData()
    return host


def displayInternetData():
    spacing = int(display.height/2)

    display.fill(0)
    ip1 = getIP1()
    hostname = getHost()
    host_text = "Host: " + hostname
    display.text(host_text, 0, 0, 1)
    ip1_text = 'IP1: ' + getIP1()
    display.text(ip1_text, 0, spacing, 1)
    display.show()
    return ip1


def refreshInterrupt(signum, _):
    display.fill(0)
    display.show()
    displayInternetData()


if __name__ == '__main__':
    signal.signal(signal.SIGALRM, refreshInterrupt)
    signal.setitimer(signal.ITIMER_REAL, 3, 3)

    display = configDisplay()

    displayInternetData()
    while(True):
        True
