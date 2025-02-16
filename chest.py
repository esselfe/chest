#!/usr/bin/python3
# chest.py has been created by the GPT-4 chat of OpenAI by feeding it the
# C source code files of this project and the following prompt:
# "Write a python script that encrypts or decrypts a file according to
# the presence of the .chest extension in the filename. The user must
# supply a password that is hashed with sha512. That hash is used to
# (encrypt) decrement a byte by the hash byte and (decrypt) increment
# each byte. Everytime a character is read from the input file an output
# byte is calculated by the incremental hash (indexed) value. When the
# hash index points to the end of the hash the index is reset to zero
# and the program hops each byte of the file with each byte of the hash...
# The password or the hash must not be stored in the file and both input
# and output files should be the same size."
# I decided to include it in the project as a working Python variant of
# the original C code written by me, esselfe (Stephane Fontaine).
# Update 2025-01-21: I wrote the shake256 feature :)
# Update 2025-02-16: I wrote the shake256-full-size feature :)
import getpass
import hashlib
import os
import sys
import argparse

default_extension = ".chest"
use_shake256 = False
shake256_is_full_size = False
file_size = 0
default_shake256_base_size = 1024
default_shake256_factor = 1024
shake256_base_size = default_shake256_base_size
shake256_factor = default_shake256_factor

def hash_sha512_password(password):
	sha512 = hashlib.sha512()
	sha512.update(password.encode('utf-8'))
	return sha512.digest()

def hash_shake256_password(password):
	global file_size, shake256_is_full_size
	
	shake = hashlib.shake_256()
	shake.update(password.encode('utf-8'))
	if shake256_is_full_size:
		return shake.digest(file_size)
	else:
		return shake.digest(shake256_base_size * shake256_factor)

def process_file(file_path, password, mode, extension):
	global use_shake256, shake256_base_size, shake256_factor

	hash_bytes = hash_shake256_password(password) if use_shake256 else hash_sha512_password(password)
	hash_len = len(hash_bytes)
	hash_index = 0

	if mode == 'encrypt':
		output_file_path = f"{file_path}{extension}"
	elif mode == 'decrypt':
		if file_path.endswith(extension):
			output_file_path = file_path[:-len(extension)]
		else:
			raise ValueError(f"File does not have a {extension} extension for decryption.")
	else:
		raise ValueError("Invalid mode. Must be 'encrypt' or 'decrypt'.")

	try:
		with open(file_path, 'rb') as input_file, open(output_file_path, 'wb') as output_file:
			while byte := input_file.read(1):
				byte = byte[0]
				result_byte = (
					(byte - hash_bytes[hash_index]) % 256 if mode == 'encrypt' 
					else (byte + hash_bytes[hash_index]) % 256
				)
				output_file.write(bytes([result_byte]))
				hash_index = (hash_index + 1) % hash_len
	except FileNotFoundError:
		print(f"Error: File '{file_path}' not found.")
		sys.exit(1)
	except IOError as e:
		print(f"Error during file processing: {e}")
		sys.exit(1)

	print(f"{'Encrypted' if mode == 'encrypt' else 'Decrypted'} file written to: {output_file_path}")

def main():
	global use_shake256, shake256_base_size, shake256_factor, shake256_is_full_size, file_size

	parser = argparse.ArgumentParser(
		description="Encrypt or decrypt a file using SHA-512 or SHAKE-256 hashed password."
	)
	parser.add_argument("filename", help="The name of the file to encrypt or decrypt")
	parser.add_argument(
		"-e", "--extension", default=default_extension,
		help=f"File extension for encrypted file (default: {default_extension})"
	)
	parser.add_argument(
		"-p", "--password-file", help="The file containing the password"
	)
	parser.add_argument(
		"-S", "--shake256-full-size", action="store_true",
		help="Generate SHAKE-256 digest of same size as the input file"
	)
	parser.add_argument(
		"-s", "--shake256", action="store_true", 
		help="Use SHAKE-256 hash function instead of SHA-512"
	)
	parser.add_argument(
		"-b", "--base", type=int, 
		help=f"SHAKE-256 base size (default: {default_shake256_base_size})"
	)
	parser.add_argument(
		"-f", "--factor", type=int, 
		help=f"SHAKE-256 size multiplication factor (default: {default_shake256_factor})"
	)

	args = parser.parse_args()
	
	use_shake256 = args.shake256
	if use_shake256:
		shake256_base_size = args.base if args.base else default_shake256_base_size
		shake256_factor = args.factor if args.factor else default_shake256_factor

	shake256_is_full_size = args.shake256_full_size
	if shake256_is_full_size:
		use_shake256 = True
		file_size = os.path.getsize(args.filename)

	if args.password_file:
		try:
			with open(args.password_file, 'r') as pwd_file:
				password = pwd_file.read().strip()
		except FileNotFoundError:
			print(f"Error: Password file '{args.password_file}' not found.")
			sys.exit(1)
	else:
		password = getpass.getpass("Enter password: ")

	mode = 'decrypt' if args.filename.endswith(args.extension) else 'encrypt'
	process_file(args.filename, password, mode, args.extension)

if __name__ == "__main__":
	main()

