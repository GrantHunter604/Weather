# Name: Grant Hunter
# Class: CPS 210 Tues-Thurs 11-12:15
# Date: 2/28/17
# Homework: Raspberry Pi Project
# Description: Read from files and write to display
 
import os
import sys
import logging
from WeatherAlert import wemain
import time
from luma.core.render import canvas
from luma.core import cmdline, error
from PIL import ImageFont

def get_device(actual_args=None):
    if actual_args is None:
        actual_args = sys.argv[1:]
    parser = cmdline.create_parser(description='Arguments')
    args = parser.parse_args(actual_args)

    if args.config:
        # load config from file
        config = cmdline.load_config(args.config)
        args = parser.parse_args(config + actual_args)

    # create device
    try:
        device = cmdline.create_device(args)
    except error.Error as e:
        parser.error(e)

    return device

def make_font(name, size):
    font_path = os.path.abspath(os.path.join(
        os.path.dirname(__file__), 'fonts', name))
    return ImageFont.truetype(font_path, size)

#From here on is what I wrote
read = ""
readl2 = ""
it=0
while(1):
    try:
        #Read from file and get contents
        contents = open("/home/pi/workspace/Weather/WeatherDisplay.txt")
        readl = contents.readlines()

        #If the filelines is not the same as the previous
        if readl2!=readl[0]:
            #Clear the screen
            device = get_device()

            #Create a new canvas to write on
            with canvas(device) as draw:
                #Create new font
                font = make_font("FreePixel.ttf", device.height - 20)

                #Create string to write to device
                tempStr = readl[0]+""

                #Get size of text
                w, h = draw.textsize(text=tempStr, font=font)

                #Figure out where to place text
                left = (device.width - w)/2
                top = (device.height - h)/2

                draw.text((left, top), tempStr, font=font, fill="yellow")

        #Open the weatheralert file and read the lines
        con = open("/home/pi/workspace/Weather/WeatherAlert.txt")
        read2 = con.readlines()

        #If the file is not the same as previous and is not first time through
        if read2!=read:
            if it!=0:
                #call wemain from WeatherAlert.py
                wemain();

        #Make sure iterator does not get too large
        if it == 1000:
            it = 1

        #iterate and change previous file lines
        it=it+1
        readl2 = readl[0]
        read = read2

        #rest for one second
        time.sleep(1)
    except Exception as e:
        print "Error! File did not read"


