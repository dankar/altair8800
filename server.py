#!/usr/bin/python
import serial
import time
import io
import sys
import struct
import array
import select

file = io.open('software/4kbas32.bin', 'rb')
data = array.array('B', file.read())

for i in range (1, 65536):
	data.append(0)

dev = serial.Serial('/dev/ttyACM0', baudrate=2000000, timeout=3.0)
time.sleep(5)

def get_raw():
	inb = dev.read()
	val = struct.unpack('B', inb)[0]
	#print("0x%x" % val);

	return val

def get_address():
	return get_raw() * 256 + get_raw()

while True:

	val = get_raw()
	
	address = 0
	if val == 0xee: # Read byte
		address = get_address()
		#print "Reading ", str(address)
		#time.sleep(1)
		#print "Value ", data[address]
		if address < 4096:
			dev.write(struct.pack('B', data[address]))
		else:
			dev.write(chr(0))
	elif val == 0xef: # Write byte
		address = get_address()
		if address < 4096:
			#print "Writing ", str(address) 
			#time.sleep(1)
			outb = get_raw()
			data[address] = outb
	elif val == 0xec: # REad short
		address = get_address()
		#print "Reading short ", str(address)
		#print "Value: ", data[address+1]
		dev.write(struct.pack('B', data[address+1]))
		#print "Value: ", data[address]
		dev.write(struct.pack('B', data[address]))

	elif val == 0xed: # Write short
		address = get_address()
		#print "Writing short ", str(address)
		data[address+1] = get_raw()
		data[address] = get_raw()
	elif val == 0xc0:
		if select.select([sys.stdin,],[],[],0.0)[0]:
			ch = sys.stdin.read(1)
			if ord(ch) == 10:
				ch = chr(13)
			dev.write(ch)
	else:
		sys.stdout.write(chr(val))
		sys.stdout.flush()

