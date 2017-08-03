# Name: Grant Hunter
# Class: CPS 210 Tues-Thurs 11-12:15
# Date: 2/28/17
# Homework: Raspberry Pi Project
# Description: Read from files and send email

import time
from datetime import datetime
import smtplib

from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

def wemain():
   #Grab from file, readlines, and get current time to send email
   content = open("/home/pi/workspace/Weather/WeatherAlert.txt")
   readl = content.readlines()
   time = datetime.now().strftime('%H:%M on %a %d %b')

   #Get email to send and other info
   toEmailAddr = 'granthunter604@gmail.com'
   fromEmailAddr = 'granthunter604@gmail.com'
   p = '******'

   #Format email
   if float(readl[0])>72.0000:
      subject = 'Apartment is TOO WARM: ' + time
   else:
      subject = 'Apartment is TOO COLD: ' + time

   #Create formating for email
   message = MIMEMultipart()
   message['Subject'] = subject
   message['From'] = fromEmailAddr
   message['To'] = toEmailAddr
   message.preamble = subject
   readl = round(float(readl[0]), 1)
   msg = "The temperature is currently: " + str(readl) + "F"
   string = MIMEText(msg, 'plain')
   message.attach(string)

   #Send email to gmail server
   try:
      s = smtplib.SMTP('smtp.gmail.com',587)
      s.ehlo()
      s.starttls()
      s.login(fromEmailAddr,p)
      print(message)
      s.sendmail(fromEmailAddr, toEmailAddr, message.as_string())
      s.quit()
   except:
      print ("Error: unable to send email")
