import asyncio
import binascii
import os
import os.path
import string
import sys
import threading
from queue import Queue, Empty

from flask import Flask, request, jsonify, abort
from flask_sockets import Sockets

import db
import loc

if not os.path.exists('.data/systems'):
  # restore the data
  os.system('unzip -LL data-demo.zip -d .data/')

# TODO: run cyber_clean

app = Flask(__name__, static_folder='public')
sockets = Sockets(app)

to_ws_queue = Queue()
from_ws_queue = Queue()
tokens = {}

ws_ports = [None] * 32
usernames = {}

def write_to_ws():
  while True:
    x = to_ws_queue.get()
    if x is None:
      break
    p, d = x
    if ws_ports[p]:
      try:
        ws_ports[p].send(d)
      except Exception as e:
        sys.stderr.write("Error writing to websocket for port %d: %s\n" % (p, e))
        sys.stderr.flush()
        logout(p)
    else:
      sys.stderr.write("No port found for %d\n" % p)
      sys.stderr.flush()


def logout(p):
  sys.stderr.write("Logging out port %d\n" % p)
  sys.stderr.flush()
  try:
    ws_ports[p].close()
  except Exception as e:
    sys.stderr.write("Error closing websocket for port %d: %s\n" % (p, e))
    sys.stderr.flush()
  ws_ports[p] = None
  loc.logout(p)
  

@sockets.route('/tty')
def ws_tty(ws):
  token = request.args.get('token')
  if token not in tokens:
    return abort(403)
  port = tokens[token]
  # TODO: validate token better
  # sys.stderr.write("%s\n" % [str(list(dir().keys()))])
  sys.stderr.write("Got websocket connection assigned to port %d\n" % port)
  sys.stderr.flush()
  
  ws_ports[port] = ws
  loc.login(usernames[token], port)

  buffer = b''
  while not ws.closed:
    try:
      x = ws.receive()
      if x is None:
        logout(port)
        break
      if x == b'\x0a' or x == b'\x0d':
        buffer += x
        from_ws_queue.put((port, buffer))
        buffer = b''
      elif x == b'\x7f' and buffer:
        buffer = buffer[:-1]
      else:
        buffer += x
    except Exception as e:
      sys.stderr.write("Error reading from websocket for port %d: %s\n" % (port, e))
      sys.stderr.flush()
      logout(port)
    
  #to_ws_queue.put(None)
  sys.stderr.write("Websocket connection closed\n")
  sys.stderr.flush()


valid_chars = string.ascii_letters + string.digits

def meets_requirements(username):
  return all([x for x in username if x in valid_chars])

def find_open_port():
  for i in range(len(ws_ports)):
    if ws_ports[i] is None:
      return i
  return None

@app.route('/login', methods=['POST'])
def login():
  username = request.form.get('username', '')
  password = request.form.get('password', '')
  user = db.check_password(username, password)
  exists = db.find_user(username)
  port = find_open_port()
  if port is None:
    return abort(503)
  if exists and user:
    t = binascii.hexlify(os.urandom(16)).decode('ascii')
    ws_ports[port] = 1 # placeholder
    tokens[t] = port
    usernames[t] = username
    return jsonify({'token': t})
  elif exists:
    return abort(403)
  elif username and password and meets_requirements(username):
    db.create_user(username, password)
    t = binascii.hexlify(os.urandom(16)).decode('ascii')
    ws_ports[port] = 1 # placeholder
    tokens[t] = port
    usernames[t] = username
    return jsonify({'token': t})
  else:
    return abort(400)

@app.route("/")
def index():
  with open('views/index.html') as fin:
    return fin.read()

def users_on():
  return sum(1 for p in ws_ports if p is not None)
  
@app.route("/stats")
def stats():
  return jsonify({'users_on': users_on(),
                  'max_users': 32})

  
def read_input(queue):
  while True:
    p, r = from_ws_queue.get()
    queue.put((loc.TTY_CODE, p, r))

def write_output(from_loc_queue):
    while True:
        code, port, data = from_loc_queue.get()
        to_ws_queue.put((port, data))
  
if __name__ == '__main__':

  # if len(sys.argv) < 2:
  #   exit("Error: unix socket file not specified")
  # sock_file = sys.argv[1]
  sock_file = 'cyber/muisock'
  to_loc_queue, from_loc_queue = loc.start(sock_file)
  
  t = threading.Thread(target=read_input, args=[to_loc_queue])
  t.daemon = True
  t.start()

  t2 = threading.Thread(target=write_output, args=[from_loc_queue])
  t2.daemon = True
  t2.start()
  
  t3 = threading.Thread(target=write_to_ws, args=[])
  t3.daemon = True
  t3.start()

  from gevent import pywsgi
  from geventwebsocket.handler import WebSocketHandler
  port = int(os.getenv('PORT', 8000))
  server = pywsgi.WSGIServer(('', port), app, handler_class=WebSocketHandler)
  sys.stderr.write("Starting on port %d\n" % port)
  sys.stderr.flush()
  server.serve_forever()