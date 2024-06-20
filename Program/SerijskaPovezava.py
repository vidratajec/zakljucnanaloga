#placeholder
import serial
import sys

def serijsk_porti():
    dostopni_porti = []
    
    if sys.platform.startswith('win32'):
        mozni_porti = []
        for i in range(1,20):
            mozni_porti.append("COM"+ str(i))
    else:
        mozni_porti = "Vnesi rocno!"

    #return mozni_porti;
    for port in mozni_porti:
        try:
            s =serial.Serial(port)
            s.close()
            dostopni_porti.append(port)

        except (OSError, serial.SerialException):
            pass
    return dostopni_porti

