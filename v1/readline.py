import serial
import time
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=0) 

with serial.Serial('/dev/ttyACM0', 9600, timeout=1) as ser:
    while True:
        line=ser.readline()
        print(line)
        with open('output.txt', 'a') as file:  # Use file to refer to the file object
            file.write(line)
            file.flush()
