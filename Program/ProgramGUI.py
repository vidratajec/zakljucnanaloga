import typing
from PyQt6.QtCore import QSize
from PyQt6.QtWidgets import QMainWindow, QPushButton, QLabel, QApplication, QWidget, QLabel, QLineEdit, QGridLayout, QComboBox
from PyQt6.QtGui import QIcon
import sys
import serial
from time import sleep


app = QApplication(sys.argv)



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

class Servo:
    def __init__(self):
        pass;
    
    def komunikacija(self, port, baud):
        self.port = port
        self.baud = baud
        self.povezava = serial.Serial(self.port, self.baud, timeout=0.5)

    def povezan(self):
        if self.povezava.isOpen() == True:
            return "Povezan"
        else:
            return "Ni povezan"

    def poslji(self, komanda):
        self.povezava.write(komanda.encode()); 

    def beri(self):
        a = str(self.povezava.readline()).replace('b','').replace('r','').replace('n','').replace('\\','')
        b = str(self.povezava.readline()).replace('b','').replace('r','').replace('n','').replace('\\','')
        if len(a) > len(b):
            return a
        else:
            return b
    
    def kot(self):
        kot_ = ""
        a = str(self.povezava.readline()).replace('b','').replace('r','').replace('n','').replace('\\','')
        b = str(self.povezava.readline()).replace('b','').replace('r','').replace('n','').replace('\\','')
        if len(a) > len(b):
            kot_ = a
        else:
            kot_ = b
        if('.' in kot_):
            return kot_
        else:
            return "-"
        

servo = Servo()


class Okno(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Servomotor")
        self.setWindowIcon(QIcon('ikona.png'))
        #self.setFixedSize(QSize(700, 1000))

        ##Serijska povezava
        self.SerijskiPort = QLabel("Serijski port:  ")
        self.porti = QComboBox()
        self.porti.addItems(serijsk_porti())
        self.BaudLabel = QLabel("Baud :  ")
        self.baud = QComboBox()
        self.baud.addItems(["115200"])
        self.gumbPoveziSe = QPushButton("Poveži!")
        self.gumbPoveziSe.clicked.connect(self.serijska_povezava)
        
        ##Premik
        self.besediloPremik = QLabel("Premik:  ")
        self.TipPremika = QComboBox()
        self.TipPremika.addItems(["i", "a"])
        self.SmerPremika = QComboBox()
        self.SmerPremika. addItems(["+","-","0"])
        self.Kot = QLineEdit()
        self.gumbPosljiPremik = QPushButton("Pošlji!")
        self.gumbPosljiPremik.clicked.connect(self.premik)

        ##Nastavitev hitrosti
        self.Hitrost = QLabel("Hitrost : ")
        self.VnosHitrosti = QLineEdit()
        self.gumbPosljiHitrost = QPushButton("Pošlji!")
        self.gumbPosljiHitrost.clicked.connect(self.hitrost)

        ##Nastavitev zamika
        self.NapisZamik = QLabel("Zamik: ")
        self.VnosZamika = QLineEdit()
        self.gumbPosljiZamik = QPushButton("Pošlji!")
        self.gumbPosljiZamik.clicked.connect(self.zamik)

        ## Povratna sporocila:
        self.StanjeServo = QLabel("Stanje Servomotorja :")
        self.Stanje = QLabel(" Povratna sporocila servomotorja")


        layout = QGridLayout()
        #Port
        layout.addWidget(self.SerijskiPort, 0,0)
        layout.addWidget(self.porti, 0,1,)
        layout.addWidget(self.BaudLabel, 0,2)
        layout.addWidget(self.baud, 0,3)
        layout.addWidget(self.gumbPoveziSe, 0,4)
        #Premik
        layout.addWidget(self.besediloPremik,1,0)
        layout.addWidget(self.TipPremika, 1,1)
        layout.addWidget(self.SmerPremika, 1, 2)
        layout.addWidget(self.Kot, 1,3)
        layout.addWidget(self.gumbPosljiPremik,1,4)
        #Hitrost
        layout.addWidget(self.Hitrost, 2,0)
        layout.addWidget(self.VnosHitrosti, 2,1,1,3)
        layout.addWidget(self.gumbPosljiHitrost, 2,4)
        #Zamik
        layout.addWidget(self.NapisZamik, 3,0)
        layout.addWidget(self.VnosZamika, 3,1,1,3)
        layout.addWidget(self.gumbPosljiZamik, 3,4)
        #Stanje
        layout.addWidget(self.StanjeServo, 4,0)
        layout.addWidget(self.Stanje, 4,1,1,3)

        
        container = QWidget();
        container.setLayout(layout)
    
        self.setCentralWidget(container);
    
    def serijska_povezava(self):
        port = self.porti.currentText()
        baudrate = self.baud.currentText()
        #print(port, baudrate)
        servo.komunikacija(port, baudrate) 
        self.Stanje.setText(str(servo.povezan()))


    
    def premik(self):
        tip = self.TipPremika.currentText()
        smer = self.SmerPremika.currentText()
        kot_ = self.Kot.text() #TODO dolzina in predelava stringa v prav format
        komanda = tip + smer + kot_
        servo.poslji(komanda)
        sleep(1)
        b = servo.beri()
        self.Stanje.setText(b)


    def hitrost(self):
        hitrost = self.VnosHitrosti.text()
        komanda = "r" + hitrost
        servo.poslji(komanda)
        print(komanda)
        b = servo.beri()
        self.Stanje.setText(b)


    
    def zamik(self):
        kotzamika = self.VnosZamika.text() #TODO format zamika
        komanda = "n" + kotzamika
        servo.poslji(komanda)
        print(komanda)
        b = servo.beri()
        self.Stanje.setText(b)

        

okno = Okno();
okno.show()


app.exec()