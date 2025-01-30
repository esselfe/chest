![chest logo](chest-128.png)  
Updated: 250130  
  
The chest program can be used to encrypt and decrypt files using a password  
hash hopping algorithm (SHA512 with fixed length or SHAKE256 with variable  
hash length). Note that the encrypted output may be vulnerable to hash  
database cracking due to its simplicity.  
  
Just give a filename as argument and it should detect, according to the  
presence of the .chest extension, whether to encrypt or decrypt.  
  
Neither the password or the hash are stored in the output file, the hash is  
generated at runtime after the user has been asked for a password.  
  
It is now possible to use a file as password. The hash will be computed with    
the content all at once, so use a file that can be entirely loaded into    
memory.  
  
Also, it's now possible to specify the extension used to detect the  
correct mode as a program option.  

COMPILING  
To compile the source code into an executable program, run 'make' in a  
terminal in the root of the source tree. To install it to /usr/local,  
run 'sudo make install'. Note that you will need the OpenSSL development  
headers installed on your system.  

If you want to specify the installation location, add PREFIX=path-here to  
the 'make install' command.  

If you want to build and install the Go version, add "USE_GO=1" to the  
'make' commands. The Python and Rust versions can also be built and  
installed this way, with the "USE_PYTHON=1" and "USE_RUST=1" options.  
  
DISCLAIMER  
There is no warranty that the encrypted output file is not  
crackable! Normally you shouldn't store critical data with this encryption.  
See the LICENSE file for full disclosure.  
  
BUGS  
The C version and the Go versions should give the same results, but differ  
from the encrypted output from the Python and Rust versions when used with  
a password file ending with a newline character.  
  
NOTES  
As a strategy you can rename the file to remove the .chest or specified  
extension and reencrypt again to reduce chances of bruteforce attacks,  
since there's not really a way to tell if the first pass gives correct results.  
  
Enjoy!  
  
