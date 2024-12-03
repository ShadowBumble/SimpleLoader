# SimpleLoader
A Simple shellcode loader used in the Ninja Hacker Academy Challenge Lab Blogpost

It features the following

- RC4 Encryption
- Simple sandbox evasion
- Some (very) basic code Obfuscation
- EarlyBird APC Technique

Just create a file in the same directory as the project called `shellcode.c` with your RC4 encrypted shellcode, change the decryption key and compile the project. 
