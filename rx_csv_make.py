"""
Purpose: Log LoRa messages addressed to this to a CSV file
Organization: San Diego Wildlife Association: Conservation Technology Lab (CTL)
Author: Trent Moca
Date: 11/10/22

This code configures a Raspberry Pi to be a LoRa receiver, with a node address of 
0x72 ('r' in ascii). All LoRa logs are appended to a CSV file with a timestamp 
reflecting seconds since the epoch. Additionally, the IP address and hostname for
the LoRa device are displayed on an LCD screen that comes built in on the Adafruit 
RFM9X LoRa shield.

SOURCE
Learn Guide: https://learn.adafruit.com/lora-and-lorawan-for-raspberry-pi
Author: Brent Rubell for Adafruit Industries
"""
# Import Python System Libraries
import time
import threading
import datetime

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
import adafruit_rfm9x  # https://github.com/adafruit/Adafruit_CircuitPython_RFM9x/blob/main/adafruit_rfm9x.py

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


# Display hostnam, ip address, and LoRa status
def currentDateTime():
    return datetime.datetime.fromtimestamp(time.time())


def convertPacketEpochToDateTime(packet):
    packet_array = packet.split(':')
    epoch_as_datetime = datetime.datetime.fromtimestamp(packet_array[0])
    return epoch_as_datetime + ': ' + packet_array[1]


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
        time.sleep(1)


if __name__ == '__main__':
    rfm9x = configRadio()

    display = configDisplay()

    displayUpdate = threading.Thread(target=updateDisplay, daemon=True)
    displayUpdate.start()

    try:
        # First row indicates a new log start
        header = ["Log Time Start", "Time Received",
                  "Packet Converted", "Packet Raw"]

        with open("TempTimeData.csv", 'x') as new_csv:
            writer = csv.writer(new_csv)
            writer.writerow(header)
            writer.writerow([currentDateTime(), '', '', ''])
        new_csv.close()

    except FileExistsError:
        True

    with open("TempTimeData.csv", 'a') as log_file:
        log_writer = csv.writer(log_file)
        writer.writerow([currentDateTime(), '', '', '']) # New log start

        while True:
            time.sleep(1)
            if not btnA.value:
                display.fill(0)
                display.show()
                break

            # Only receives from nodes addressed to 0x72 -- should be able to receive non ack messages as well...
            packet = rfm9x.receive(with_ack=True, with_header=True)
            if packet is None:
                log_writer.writerow(['', currentDateTime(), ' ', ' '])
            else:
                rec_packet_flag = True  # For LoRa status on display
                packet_text = str(packet[4:], "utf-8")
                packet_conv = convertPacketEpochToDateTime(packet_text)
                data_row = ['', currentDateTime(), packet_conv, packet_text]
                log_writer.writerow(data_row)
