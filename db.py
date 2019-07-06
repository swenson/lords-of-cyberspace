import os
import os.path

#from flask_login import UserMixin
from werkzeug.security import generate_password_hash, check_password_hash
from peewee import *

if not os.path.exists('data'):
  os.mkdir('data')

db = SqliteDatabase('data/bbs.db')

class BaseModel(Model):
    class Meta:
        database = db

class User(BaseModel):
    username = CharField(unique=True)
    pwhash = CharField()
    ansiflag = IntegerField(default=0)

db.connect()
db.create_tables([User])

HASH_PEPPER = '5cf99d571dd968d4c2732e784ee8f2ad'

def pwhash(pw, salt=None):
  return generate_password_hash(HASH_PEPPER + pw + HASH_PEPPER)

def hashcheck(pw, hash):
  return check_password_hash(hash, HASH_PEPPER + pw + HASH_PEPPER)

def find_user(username):
  users = User.select().where(User.username == username)
  if len(users) != 1:
    return None
  return users[0]

def check_password(username, password):
  u = find_user(username)
  if not u:
    return None
  return hashcheck(password, u.pwhash)

def create_user(username, password):
  User.create(username=username, pwhash=pwhash(password))