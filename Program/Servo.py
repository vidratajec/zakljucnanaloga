"""
port -> Serijski port do RP2040
absolutno -> absolutne stopinje [0,360]
inkrementalno -> inkrementalen premik
hitrost -> RPM
smer vrtenja -> ura, proti uri, najkrajsa pot ...
izhodisce -> nastavi tretutno pozicijo kot izhodiscno
"""
import serial

class ServoMotor():

    def __init__(self, port):
        self.port = port
        

    def abs(stopinje):
        
        """
        Absoluten premik, arguemnt podan v stopinjah (float)
        """
        pass

    def ink(stopinje):
        """inkrementalen pomik, argument podan v stopinjah (float), smer v "cc" ali "cw" """
        pass

    def hitrost():
        """Hitrost, argument v RPM oziroma obratih na minuto"""
    
    def smer():
        """Smer vrtneja, podana kot SU, NSU, MIN
        U -> smer ure
        P  -> proti smeri ure
        M -> najmanjsa razdalja, smer doloci arduino sam.
        """

    def izhodisce():
        """nastavi podane ali trenutne stopinje za novo izhodicse"""
