#!/usr/bin/env python3

import hashlib
import os
import sys
import csv
import json
import urllib.parse
import sqlite3
import uuid

# TestUser: hans@gmail.com,Test123,Hansbert42

# def deleteUser(cursor, email):
# 	try:
# 		cursor.execute("DELETE FROM users WHERE email = ?", (email))
# 	except sqlite3.IntegrityError:
# 		print("User deletion failed")

def addUser(cursor, email, password_hash, username):
	try:
		cursor.execute("INSERT INTO users (email, password, username) VALUES (?, ?, ?)", (email, password_hash, username))
		print("Content-Type: application/json\n")
		print(json.dumps({
			"status": "ok",
			"username": username
		}))
	except sqlite3.IntegrityError:
		print("Content-Type: application/json\n")
		print(json.dumps({
			"status": "fail",
			"message": "User already exists"
		}))

def checkCookie(email, cookie):
	conn = sqlite3.connect('users.db')
	cursor = conn.cursor()
	cursor.execute("SELECT username FROM users WHERE email = ? AND cookie = ?", (email, cookie))
	result = cursor.fetchone()
	conn.close()
	return True if result else False

def logout(cursor, email, cookie):
	cursor.execute("SELECT username FROM users WHERE email = ? and cookie = ?", (email, cookie))
	row = cursor.fetchone()

	if row:
		cursor.execut("UPDATE users SET cookie = ? WHERE email = ?", ("", email))

		print("Status: 302 Found")
		print("Location: /index.html")
		print(f"Set-Cookie: session_id=""; Path=/; HttpOnly")
		print()

		print(json.dumps({
			"status": "Logout succesfull",
			"username": username,
			"cookie": "removed"
		}))

def login(cursor, email, hashed_password):
	cursor.execute("SELECT username FROM users WHERE email = ? AND password = ?", (email, hashed_password))
	row = cursor.fetchone()

	if row:
		username = row[0]

		# Cookie generieren (z. B. UUID)
		cookie = str(uuid.uuid4())

		# Cookie setzen in der DB
		cursor.execute("UPDATE users SET cookie = ? WHERE email = ?", (cookie, email))
	
		# print("Content-Type: application/json")
		print("Status: 302 Found")
		print("Location: /index.html")
		print(f"Set-Cookie: session_id={cookie}; Path=/; HttpOnly")
		print()

		print(json.dumps({
			"status": "ok",
			"username": username,
			"cookie": cookie
		}))
	else:
		print("Content-Type: application/json\n")
		print(json.dumps({
			"status": "fail",
			"message": "Username or password is wrong"
		}))

def main():
	# contentLength = int(os.environ.get('CONTENT_LENGTH', 0))
	bodyString = os.environ.get('BODY_STRING', '')
	dbPath = os.environ.get('ROOT_PATH', '') + "/user.db"
	
	params = urllib.parse.parse_qs(bodyString)

	# Username & Passwort extrahieren
	email = params.get("email", [""])[0]
	password = params.get("password", [""])[0]
	username = params.get("username", [""])[0]
	authType = params.get("source", [""])[0]

	#hash password
	hashed_password = hashlib.sha256(password.encode()).hexdigest()
	
	#create database if not existing
	conn = sqlite3.connect(dbPath)
	cursor = conn.cursor()

	cursor.execute('''
	CREATE TABLE IF NOT EXISTS users (
		email TEXT PRIMARY KEY,
		password TEXT,
		username TEXT,
		cookie TEXT
		)
	''')

	try:
		if authType == "login":
			login(cursor, email, hashed_password)
		elif authType == "create":
			addUser(cursor, email, hashed_password, username)
		elif authType == "delete":
			deleteUser(cursor, email)

		conn.commit()
	finally:
		conn.close()

	# for debugging, REMOVE
	# print(f"<p>Email: {email}</p>")
	# print(f"<p>Passwort (klartext): {password}</p>")
	# print(f"<p>Passwort (SHA-256): {hashed_password}</p>")
	# print(f"<p>Username: {username}</p>")
	# print(f"<p>DB Path: {dbPath}</p>")

if __name__ == "__main__":
	main()
