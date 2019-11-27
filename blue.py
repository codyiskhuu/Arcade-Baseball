import sys
import bluetooth

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
##connecting to the blue tooth
try:
    sock = bluetooth.BluetoothSocket( bluetooth.RFCOMM )

    sock.connect( (bd_addr, 1) )
    print("Initial Connection")
except BluetoothError as bt:
    print("Cannot connect to host" + str(bt))
    exit(0)
##Testing out the sending
try:
   sock.send("test")
except BluetoothError as err:
   print("Bluetooth error on send" + str(err))

#now keep sending, the light on the atmega will turn off everytime you send a something from the socket
while True:
        data = raw_input("Please enter 1 \n")
        print ("Entered: " + data)

        if not data:
                break
        sock.send(data)

sock.close()








##send stuff to the HC 05 by taking the address of the HC 05 and sending it

