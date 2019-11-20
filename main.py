#!/usr/bin/python
import smbus
import math
import time
import sys

# Register
power_mgmt_1 = 0x6b
power_mgmt_2 = 0x6c

def read_byte(reg):
    return bus.read_byte_data(address, reg)

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

def dist(a,b):
    return math.sqrt((a*a)+(b*b))

def get_y_rotation(x,y,z):
    radians = math.atan2(x, dist(y,z))
    return -math.degrees(radians)

def get_x_rotation(x,y,z):
    radians = math.atan2(y, dist(x,z))
    return math.degrees(radians)

bus = smbus.SMBus(1)
address = 0x68

bus.write_byte_data(address, power_mgmt_1, 0)

"""
print
print "accelorometer"
print "---------------------"
"""
xout = read_word_2c(0x3b)
yout = read_word_2c(0x3d)
zout = read_word_2c(0x3f)


try:
    while True:
	print "xout: ", ("%6d" % xout)
	print "yout: ", ("%6d" % yout)
	print "zout: ", ("%6d" % zout)
        time.sleep(0.1)

except KeyboardInterrupt:
    sys.exit()
