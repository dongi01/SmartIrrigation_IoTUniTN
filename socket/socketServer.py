import socket
import time 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('0.0.0.0', 8585 ))
s.listen(0)                 
 
while True:
    client, addr = s.accept()
    client.settimeout(5)
    while True:
        content = client.recv(1024)
        if len(content) == 0:
           break
        if str(content,'utf-8') == '\r\n':
            continue
        else:
            print(str(content,'utf-8'))
            data = 'Hello From Python'
            client.send(data.encode())
    print('Client disconnected')
    client.close()