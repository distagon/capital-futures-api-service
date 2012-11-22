import socket
import sys
import struct

A=struct.pack('2b',2,1);
ssoo = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
ssoo.connect(('172.17.4.129',1677));

for a in xrange(1,1000) :
    ssoo.send(A);

ssoo.close();

