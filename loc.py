import hashlib
import os
import os.path
import socket
import struct
import subprocess
import sys
import time
import threading

from queue import Queue, Empty

LOGIN_CODE       = 1
LOGOUT_CODE      = 2
UPDATE_USER_CODE = 3
TTY_CODE         = 4

shutdown = False

def start_loc_process():
  return subprocess.Popen('./cyber', cwd='cyber')

def handle_to_loc_socket(loc_socket, to_loc):
  while not shutdown:
    try:
      packet = to_loc.get(timeout=5)
    except Empty:
      continue
    (code, user, data) = packet
    
    # print('Sending %d %d %s' % (code, user, data))
    loc_socket.send(struct.pack('<BHH', code, user, len(data)))
    if isinstance(data, str):
      loc_socket.send((data).encode('utf8'))
    else:
      loc_socket.send(data)

def handle_from_loc_socket(loc_socket, from_loc):
  while not shutdown:
    cul = loc_socket.recv(5)
    if len(cul) != 5:
      continue
    code, user, length = struct.unpack('<BHH', cul)
    data = b''
    while len(data) != length:
      data += loc_socket.recv(length - len(data))
    from_loc.put((code, user, data))


# def read_input(queue):
#   while True:
#     r = input()
#     queue.put((TTY_CODE, 0, r + '\n'))

# def write_output(from_loc_queue):
#     while True:
#         code, data = from_loc_queue.get()
#         sys.stderr.write(data.decode('ascii'))
#         sys.stderr.flush()

to_loc_queue = Queue()
from_loc_queue = Queue()


def login(username, port):
    to_loc_queue.put((LOGIN_CODE, port, username.encode('ascii')))
    to_loc_queue.put((TTY_CODE, port, '\n'))
    to_loc_queue.put((TTY_CODE, port, 'y\n'))

def logout(port):
  to_loc_queue.put((LOGOUT_CODE, port, b''));

def start(sock_file):
    if os.path.exists(sock_file):
      os.unlink(sock_file)
    p = start_loc_process()
    
    while not os.path.exists(sock_file):
      time.sleep(0.1)
  
    loc_socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    loc_socket.connect(sock_file)

    to_loc_thread = threading.Thread(target=handle_to_loc_socket,
        args=[loc_socket, to_loc_queue])
    to_loc_thread.daemon = True
    to_loc_thread.start()
    from_loc_thread = threading.Thread(target=handle_from_loc_socket,
        args=[loc_socket, from_loc_queue])
    from_loc_thread.daemon = True
    from_loc_thread.start()

    print("Connected to socket " + sock_file)
    return to_loc_queue, from_loc_queue