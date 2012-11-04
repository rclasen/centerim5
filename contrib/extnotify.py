#!/usr/bin/env python

'''This script processes events generated by the extaction plugin and displays
notifications of these events on the screen.'''

import os, sys
import base64
import pynotify

def main():
  # make the parameters saved in enviromental variables easier accessible
  try:
    event_type = os.environ['EVENT_TYPE']
    # this script can handle only the msg type
    if event_type != 'msg':
      sys.exit(1)

    #event_network = os.environ['EVENT_NETWORK']
    #event_local_user = os.environ['EVENT_LOCAL_USER']
    event_remote_user = os.environ['EVENT_REMOTE_USER']
    event_message = os.environ['EVENT_MESSAGE']
    #event_message_html = os.environ['EVENT_MESSAGE_HTML']
  except KeyError:
    # some necessary parameters are missing
    sys.exit(1)

  if not pynotify.init('Extaction-plugin handler'):
    sys.exit(1)

  title = 'Message from %s:' % event_remote_user
  body = event_message[0:256]
  n = pynotify.Notification(title, body)
  if (os.environ.has_key('EVENT_REMOTE_USER_ICON')):
    # the icon is encoded in base64, decode it first
    icon_encoded = os.environ['EVENT_REMOTE_USER_ICON']
    icon_decoded = base64.b64decode(icon_encoded)

    # create a pixbuf loader
    loader = pynotify.gtk.gdk.PixbufLoader();
    loader.set_size(48, 48);
    loader.write(icon_decoded);
    loader.close()

    # set icon from the pixbuf
    pixbuf = loader.get_pixbuf()
    n.set_icon_from_pixbuf(pixbuf)

  # get the notification on the screen
  n.show()

  pynotify.uninit()

if __name__ == '__main__':
  main()
