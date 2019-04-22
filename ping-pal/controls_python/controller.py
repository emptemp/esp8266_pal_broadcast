import socket 
import click

UDP_IP = "192.168.0.87"
UDP_PORT = 7777

sock = socket.socket( socket.AF_INET,     # Internet
                      socket.SOCK_DGRAM)  # UDP


while 1:
  c = click.getchar()
  click.echo()
  if c == 'w':  
    sock.sendto('w', (UDP_IP, UDP_PORT))
  elif c == 's':
    sock.sendto('s', (UDP_IP, UDP_PORT))
  elif c == 'o':  
    sock.sendto('o', (UDP_IP, UDP_PORT))
  elif c == 'l':
    sock.sendto('l', (UDP_IP, UDP_PORT))
  elif c == 'r':
    sock.sendto('r', (UDP_IP, UDP_PORT))
#  elif c == 'o':
#    sock.sendto('p2:up', (UDP_IP, UDP_PORT))
#  elif c == 'l':  
#    sock.sendto('p2:down', (UDP_IP, UDP_PORT))


