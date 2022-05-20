# pappus
Code and tools related to LoRa communication, particularly as pertains to data links to and between devices in the Biodiversity Reserve.



## INITIALIZING NEW RASPBERRY PI

Scripts in this repository use libraries provided by Adafruit, to be used with the "Adafruit LoRa 900 MHz Bonnet," product 4074.
Libraries must be downloaded using the following terminal code lines:

    sudo pip3 install adafruit-circuitpython-ssd1306
    sudo pip3 install adafruit-circuitpython-framebuf
    sudo pip3 install adafruit-circuitpython-rfm9x
    
The font file must also be installed using the following code line, and moved into the same directory as used to run the scripts:
    
    wget https://github.com/adafruit/Adafruit_CircuitPython_framebuf/raw/main/examples/font5x8.bin
    
I2C and SPI will need to be configured in order for the scripts to appropriately send and receive data
    
