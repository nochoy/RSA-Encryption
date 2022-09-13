# RSA Encryption
NOTE: This program was modified from a Computer Systems and C Programming course assignment. All header files were provided by Professor Darrel Long at UC Santa Cruz. 
<br><br>
This program implements RSA encryption, a public-key cryptography algorithm to generate public and private key pairs to encrypt and decrypt messages. 

## How To Build
Build the program with:
```
$ make all
```
Clean binary and object files with:
```
$ make clean
```

## How To Generate keys
To print the usage and help page, run with a `-h` flag:
```
$ ./keygen -h
```

To specify the file to save the Public key (default: rsa.pub), use the `-n` flag:
```
$ ./keygen -n [file name]
```

To specify the file to save the Private key (default: rsa.priv), use the `-d` flag:
```
$ ./keygen -d [file name]
```

Generate public and private key pairs:
```
$ ./keygen
```


## How To Run
To print the usage and help page, run the encrypter or decrypter with a `-h` flag:
```
$ ./encrypt -h
$ ./decrypt -h
```

To read from an input file, run the encrypter or decrypter with a `-i` flag:
```
$ ./encrypt -i [file name]
$ ./decrypt -i [file name]
```

To print to an output file, run the encrypter or decrypter with a `-o` flag:
```
$ ./encrypt -i [file name] -o [file name]
$ ./decrypt -i [file name] -o [file name]
```

To print verbose statistics, run the encrypter or decrypter with a `-v` flag:
```
$ ./encrypt -v
$ ./decrypt -v
```

To specify the public key or private key file (default: rsa.pub/rsa.priv), run the encrypter or decrypter with a `-n` flag:
```
$ ./encrypt -n [file name]
$ ./decrypt -n [file name]
```