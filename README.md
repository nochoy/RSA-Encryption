# RSA Cryptography
NOTE: This program was modified from a Computer Systems and C Programming course assignment. All header files were provided by Professor Darrell Long at UC Santa Cruz. 
<br><br>
This program implements RSA encryption, a public-key cryptography algorithm to generate public and private key pairs to encrypt and decrypt messages. 

## How It Works
RSA is a public-key cryptographic system that uses public and private keys. Public keys are available for others while the private keys are reserved only for the owner. Anyone can encrypt a message using the intended receiver's public key, but the message can be only be decrypted by the receiver's private key. The security of RSA depends on the assumed difficulty of factoring two large prime integers since the fundamental theorem of arithmetic states that every integer has a unique prime factorization. Since RSA uses large integers, this program uses the GNU multiple precision arithmetic library (GMP). 

### Generating Keys
`keygen` generates the key pairs using one way functions. The public key is represented by integers `n` and `e` and the private key is represented by integer `d`. First, two large random prime integers, `p` and `q`, are generated. These two prime integers are used to calculate moudulus `n = pq`. Totient of `n`, `ϕ(n)`, is needed to calculate `e` for the public key. `ϕ(n)` denotes the number of positive integers up to `n` that are relatively prime to `n`:
$$ϕ(n)\ =\ ϕ(pq)\ =\ (p-1)(q-1)$$ 

The public exponent `e` is calculated by generating large random integers and computing the greatest common demonimator (GCD) of the large random integer and `ϕ(n)`, until we find an appropriate number that is coprime with `ϕ(n)`, or when the GCD of the random number and the totient equals to `1`. The random large integer will be `e`. 

The private key `d`, is computed by taking the inverse of 
$$ e\ mod\ ϕ(n) $$

### Encrypting Messages
The encrypter encrypts messages in blocks, rather than the entire file at once. It computes ciphertext by encrypting message `m` using the public key which includes the exponent `e` and modulus `n`:
$$E(m) = c = m^{e} (mod\ n)$$
The ciphertext will then be written to the output as a hexstring. 

### Decrypting Messages
The decrypter all decrypts in blocks. It deciphers the ciphertext by decrypting ciphertext `c` using the private key `d` and modulus `n`:
$$D(c) = m = c^{d}\;(mod\ n)$$
The decrypted message will then be written to the ouput. 



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
### The program supports any combination of the following command-line options:
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
### The program supports any combination of the following command-line options:
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