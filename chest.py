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
import getpass
import hashlib
import os
import sys
import argparse

default_extension = ".chest"

def hash_password(password):
    # Hashes the password using SHA-512 and returns the hash.
    sha512 = hashlib.sha512()
    sha512.update(password.encode('utf-8'))
    return sha512.digest()

def process_file(file_path, password, mode, extension):
    # Encrypts or decrypts the file based on the mode.
    hash_bytes = hash_password(password)
    hash_len = len(hash_bytes)
    hash_index = 0

    # Determine the output file name
    if mode == 'encrypt':
        output_file_path = f"{file_path}{extension}"
    elif mode == 'decrypt':
        if file_path.endswith(extension):
            output_file_path = file_path[:-len(extension)]
        else:
            raise ValueError(f"File does not have a {extension} extension for decryption.")

    with open(file_path, 'rb') as input_file, open(output_file_path, 'wb') as output_file:
        while byte := input_file.read(1):
            # Convert the byte to an integer for processing
            byte = byte[0]
            
            if mode == 'encrypt':
                # Encrypt by subtracting hash byte value
                result_byte = (byte - hash_bytes[hash_index]) % 256
            else:
                # Decrypt by adding hash byte value
                result_byte = (byte + hash_bytes[hash_index]) % 256

            # Write the processed byte to the output file
            output_file.write(bytes([result_byte]))

            # Update hash index
            hash_index = (hash_index + 1) % hash_len

    print(f"{'Encrypted' if mode == 'encrypt' else 'Decrypted'} file written to: {output_file_path}")

def main():
    parser = argparse.ArgumentParser(description="Encrypt or decrypt a file using SHA-512 hashed password.")
    parser.add_argument("filename", help="The name of the file to encrypt or decrypt")
    parser.add_argument("-e", "--extension", default=default_extension, help=f"File extension for encrypted file (default: {default_extension})")
    parser.add_argument("-p", "--password-file", help="The file containing the password", required=False)

    args = parser.parse_args()

    # Get the password from the specified file or prompt the user
    if args.password_file:
        try:
            with open(args.password_file, 'r') as pwd_file:
                password = pwd_file.read().strip()
        except FileNotFoundError:
            print(f"Error: Password file '{args.password_file}' not found.")
            sys.exit(1)
    else:
        password = getpass.getpass("Enter password: ")

    # Determine whether to encrypt or decrypt
    mode = 'decrypt' if args.filename.endswith(args.extension) else 'encrypt'
    
    # Process the file
    process_file(args.filename, password, mode, args.extension)

if __name__ == "__main__":
    main()
