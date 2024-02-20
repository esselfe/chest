# chest.py has been created by the GPT-4 chat of OpenAI by feeding it the 
# chest.c file of this project. I decided to include it in the project as
# working Python variant of the original C code written by me,
# esselfe (Stephane Fontaine).
import hashlib
import os

def hash_password(password):
    """Hashes the password using SHA-512 and returns the hash."""
    sha512 = hashlib.sha512()
    sha512.update(password.encode('utf-8'))
    return sha512.digest()

def process_file(file_path, password, mode):
    """Encrypts or decrypts the file based on the mode."""
    hash_bytes = hash_password(password)
    hash_len = len(hash_bytes)
    hash_index = 0

    # Determine the output file name
    if mode == 'encrypt':
        output_file_path = f"{file_path}.chest"
    elif mode == 'decrypt':
        if file_path.endswith('.chest'):
            output_file_path = file_path[:-6]
        else:
            raise ValueError("File does not have a .chest extension for decryption.")

    with open(file_path, 'rb') as input_file, open(output_file_path, 'wb') as output_file:
        while byte := input_file.read(1):
            # Convert the byte to an integer for processing
            byte_value = int.from_bytes(byte, 'big')
            hash_byte_value = hash_bytes[hash_index]

            # Encrypt or decrypt based on the mode
            if mode == 'encrypt':
                output_byte_value = (byte_value - hash_byte_value) % 256
            elif mode == 'decrypt':
                output_byte_value = (byte_value + hash_byte_value) % 256

            # Write the processed byte to the output file
            output_file.write(output_byte_value.to_bytes(1, 'big'))

            # Increment the hash index and wrap it if necessary
            hash_index = (hash_index + 1) % hash_len

if __name__ == "__main__":
    file_path = input("Enter the file path: ")
    password = input("Enter the password: ")

    # Determine mode based on the file extension
    if file_path.endswith('.chest'):
        mode = 'decrypt'
    else:
        mode = 'encrypt'

    try:
        process_file(file_path, password, mode)
        print(f"File successfully processed in {mode} mode.")
    except ValueError as e:
        print(e)

