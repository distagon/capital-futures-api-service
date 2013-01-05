import socket
import sys
import time


ctl = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
ctl.connect(('127.0.0.1',3396));

#login
ctl.send(b'\x01\x02');
rt = ctl.recv(3);
print(rt);
time.sleep(1);

#set watch
ctl.send(b'\x0b\x05MTX01\0\0\0\0\0');
ra = ctl.recv(3);
print(ra);
time.sleep(1);

#pull
ctl.send(b'\x05\x06\0\0\0\0');
ra = ctl.recv(20);

#logout
#A=struct.pack('2b',2,3);
ctl.send(b'\x01\x03');
time.sleep(10);

#exit
#A=struct.pack('2b',2,4);
ctl.send(b'\x01\x04');
time.sleep(2);


ctl.close();

