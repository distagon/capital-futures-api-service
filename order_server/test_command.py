import socket
import sys

ssoo = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
ssoo.connect(('127.0.0.1',1677));

#login
#A=struct.pack('2b',2,1);
ssoo.send(b'\x02\x01');

#select account
#A=struct.pack('3b',3,2,1);
ssoo.send(b'\x03\x02\x01');

#order market
#a=b'\x05MTX12\0\x01\x01\x02';
#b=len(a);
#b+a;
#ssoo.send(b);

#logout
#A=struct.pack('2b',2,3);
ssoo.send(b'\x02\x03');

#exit
#A=struct.pack('2b',2,4);
ssoo.send(b'\x02\x04');

ssoo.close();

