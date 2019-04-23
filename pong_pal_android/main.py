__version__ = "1.0"

import kivy
import socket
import time
#from threading import Thread
#kivy.require('1.10.1') # replace with your current kivy version !

from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.lang import Builder
#from kivy.uix.label import Label

class RootWidget(BoxLayout):
    def __init__(self):
      super(RootWidget, self).__init__()
      self.ip = "192.168.0.87"
      self.port = 7777
      self.sock = socket.socket( socket.AF_INET,     # Internet
                            socket.SOCK_DGRAM)  # UDP
     
    def on_touch_move(self, touch):
      self.sock.sendto(str(round(touch.x/3)).encode(), (self.ip, self.port))

class pong_pal(App):
  def build(self):
    return RootWidget()      
#        return Label(text='PONG-PAL by TSG')


if __name__ == '__main__':
    pong_pal().run()
