import smbus
import math
import time
import sys
import bluetooth

def read_word(reg):
    h = bus.read_byte_data(address, reg)
    l = bus.read_byte_data(address, reg+1)
    value = (h << 8) + l
    return value

def read_word_2c(reg):
    val = read_word(reg)
    if (val >= 0x8000):
        return -((65535 - val) + 1)
    else:
        return val

bus = smbus.SMBus(1)
address = 0x68
#location of the acceloremeter
bus.write_byte_data(address, power_mgmt_1, 0)


nearby_devices = bluetooth.discover_devices()
num = 0
print "select devices by entering a number"
for i in nearby_devices:
        num += 1
        print num, ": ", bluetooth.lookup_name(i)

select = input("> ") - 1


##Bluetooth search is over


print "Bluetooth selected: ", bluetooth.lookup_name(nearby_devices[select])

bd_addr = nearby_devices[select]

print (bd_addr)
#connecting to the blue tooth
try:
    sock = bluetooth.BluetoothSocket( bluetooth.RFCOMM )

    sock.connect( (bd_addr, 1) )
    print("Connected")
except BluetoothError as bt:
    print("Did not connect" + str(bt))
    exit(0)
#Testing out the sending
try:
   sock.send("testing")
except BluetoothError as err:
   print("Bluetooth error" + str(err))

#now keep sending, the light on the atmega will turn off everytime you send a something from the socket
while True:
        xout = read_word_2c(0x3b)
        yout = read_word_2c(0x3d)                                                                                                                                                                                          zout = read_word_2c(0x3f) 
        zouy = read_word_2c(0x3f)

	if (xout > 1.100) or (xout < -1.100):
		sock.send('1')
	elif (yout > 1.100) or (yout <-1.100):
		sock.send('1')
	elif (zout > 1.100) or (yout < -1.100):
		sock.send('1')

sock.close()


