# SPDX-FileCopyrightText: 2018 Brent Rubell for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""
Example for using the RFM9x Radio with Raspberry Pi.

Learn Guide: https://learn.adafruit.com/lora-and-lorawan-for-raspberry-pi
Author: Brent Rubell for Adafruit Industries
"""
# Import Python System Libraries
import time
import threading

# Permit cmdline for getting host info
import subprocess

# Log input LoRa data
import csv

# Import Blinka Libraries
import busio
from digitalio import DigitalInOut, Direction, Pull
import board
# Import the SSD1306 module.
import adafruit_ssd1306
# Import RFM9x
import adafruit_rfm9x

btnA = DigitalInOut(board.D5)
btnA.direction = Direction.INPUT
btnA.pull = Pull.UP

rec_packet_flag = False


def configDisplay():  # 128x32 OLED Display
    # Create the I2C interface.
    i2c = busio.I2C(board.SCL, board.SDA)

    reset_pin = DigitalInOut(board.D4)
    display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c, reset=reset_pin)
    display.fill(0)  # clear the display
    display.show()
    return display


def configRadio():
    CS = DigitalInOut(board.CE1)
    RESET = DigitalInOut(board.D25)
    spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
    rfm9x = adafruit_rfm9x.RFM9x(spi, CS, RESET, 915.0)
    rfm9x.tx_power = 23
    rfm9x.ack_delay = 0.1
    rfm9x.node = ord('r')  # r as in rx
    return rfm9x


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


def updateDisplay():
    global rec_packet_flag
    while(True):
        display.fill(0)
        display.show()
        getHostData()

        spacing = int(display.height/4)

        hostname, ip1, ip2 = getHostData()
        host_text = "Host: " + hostname
        ip1_text = "IP1: " + ip1
        display.text(host_text, 0, 0, 1)
        display.text(ip1_text, 0, spacing, 1)
        if rec_packet_flag is False:
            display.text('- Waiting -', 15, 2*spacing, 1)
        else:
            display.text('Received', 15, 2*spacing, 1)
            rec_packet_flag = False

        display.show()
        time.sleep(1.5)


if __name__ == '__main__':
    rfm9x = configRadio()

    display = configDisplay()

    displayUpdate = threading.Thread(target=updateDisplay, daemon=True)
    displayUpdate.start()

    try:
        header = ["Log Time Start", "Time Received", "Packet Data"]

        with open("TempTimeData.csv", "x") as new_csv:
            writer = csv.writer(new_csv)
            writer.writerow(header)
            writer.writerow([time.time(), '', ''])
        new_csv.close()

    except FileExistsError:
        True

    with open("TempTimeData.csv", 'a') as log_file:
        log_writer = csv.writer(log_file)

        while True:
            if not btnA.value:
                display.fill(0)
                display.show()
                break

            packet = rfm9x.receive(with_ack=True, with_header=True)
            if packet is None:
                continue
            else:
                rec_packet_flag = True
                packet_text = str(packet[4:], "utf-8")
                log_writer.writerow(['', time.time(), packet_text])
