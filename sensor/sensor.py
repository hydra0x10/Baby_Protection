import serial
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)

try:
    while 1:
             response = ser.readline()
             print response
except KeyboardInterrupt:
    ser.close()