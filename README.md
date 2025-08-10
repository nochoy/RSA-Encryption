# 🔐 RSA Cryptography 

An RSA encryption library implemented in C with optimized key generation, encryption, and decryption capabilities.

## 🎬 Live Demo

![Live Demo](demo.gif)

## 🎯 Features

### 🔒 Security Features
- **Cryptographically Secure**: Uses Miller-Rabin primality testing with 50 iterations
- **Memory Safe**: Zeroes sensitive data after use
- **Key Validation**: Comprehensive key pair validation
- **Side-Channel Resistant**: Constant-time modular exponentiation

### ⚡ Performance Optimizations
- **Fast Modular Exponentiation**: Optimized square-and-multiply algorithm
- **Efficient GCD**: Binary GCD algorithm for key generation
- **Memory Pool**: Efficient memory management for large numbers
- **Multi-threading Ready**: Thread-safe random state management

### 🌐 Cross-Platform Support
- **Linux**: Full support with GCC/Clang
- **macOS**: Native support with Xcode
- **Windows**: MinGW/MSYS2 compatible

## 🔍 Technical Architecture

### RSA Algorithm Implementation
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Key Generation │    │   Encryption    │    │   Decryption    │
├─────────────────┤    ├─────────────────┤    ├─────────────────┤
│ 1. Generate p,q │    │ 1. Read message │    │ 1. Read cipher  │
│ 2. Compute n=pq │    │ 2. Pad message  │    │ 2. Decrypt      │
│ 3. Compute φ(n) │    │ 3. Encrypt      │    │ 3. Remove pad   │
│ 4. Choose e     │    │ 4. Write output │    │ 4. Write output │
│ 5. Compute d    │    │                 │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Mathematical Foundation
- **Key Generation**: Based on the difficulty of factoring large integers
- **Encryption**: `c = m^e mod n`
- **Decryption**: `m = c^d mod n`
- **Security**: Relies on the RSA problem (computing e-th roots modulo n)


## 📋 Installation

### Prerequisites
- **C Compiler**: GCC 7.0+ or Clang 6.0+
- **GMP Library**: GNU Multiple Precision Arithmetic Library
- **Make**: GNU Make or compatible

### Install GMP Library
```bash
# Ubuntu/Debian
sudo apt-get install libgmp3-dev build-essential

# macOS
brew install gmp
```

### Build Instructions
```bash
# Clone the repository
git clone https://github.com/nochoy/RSA-Encryption.git

# Open repository
cd RSA-Encryption

# Build
make clean && make

```

## 📖 Usage Guide

### Key Generation
```bash
./keygen [OPTIONS]

Options:
  -b <bits>    Key size in bits (1024, 2048, 4096)
  [default: 2048]
  -n <file>    Output public key file 
  [default: rsa pub]
  -d <file>    Output private key file 
  [default: rsa.priv]
  -s <seed>    Random seed for reproducible keys
  -v           Verbose output
```

### Encryption
```bash
./encrypt [OPTIONS]

Options:
  -i <file>    Input file to encrypt
  -o <file>    Output encrypted file [default: encrypted.bin]
  -n <file>    Public key file [default: rsa.pub]
  -v           Verbose output
```

### Decryption
```bash
./decrypt [OPTIONS]

Options:
  -i <file>    Input file to decrypt
  -o <file>    Output decrypted file [default: decrypted.txt]
  -n <file>    Private key file [default: rsa.priv]
  -v           Verbose output
```

## Examples

```bash
# Generate RSA key pair
./keygen -b 2048 -n public.pem -d private.pem

# Encrypt a message
echo "Hello, RSA!" > message.txt
./encrypt -i message.txt -o encrypted.bin -n public.pem

# Decrypt the message
./decrypt -i encrypted.bin -o decrypted.txt -d private.pem

# Clean 
make clean
```


## 🧪 Testing & Demo

### Interactive Demo (Linux/macOS)
```bash
./demo.sh
```

This Will:
    
1. Build the project
2. Create sample files
3. Demonstrate key generation & encryption/decription
4. Compare key sizes
5. Show security features

### Manual Testing
```bash
# Create demo file
echo "This is a secret message!" > secret.txt

# Generate keys
./keygen -b 2048 -n public.pem -d private.pem

# Encrypt
./encrypt -i secret.txt -o secret.enc -n public.pem

# Decrypt
./decrypt -i secret.enc -o secret_decrypted.txt -d private.pem

# Verify
diff secret.txt secret_decrypted.txt && echo "Success!"
```


## 📁 Project Structure
```
RSA-Encryption/
├── keygen.c            # Key generation program
├── encrypt.c           # Encryption program
├── decrypt.c           # Decryption program
├── rsa.c/.h            # Core RSA implementation
├── numtheory.c/.h      # Number theory utilities
├── randstate.c/.h      # Random state management
└── examples/           # Example files
├── Makefile            # Build configuration
├── demo.sh             # Interactive Demo script
├── README.md           # This file
```

## 🛡️ Security Considerations

### Best Practices
- **Key Size**: Use at least 2048-bit keys for production
- **Key Storage**: Store private keys securely with appropriate permissions
- **Random Generation**: Use high-quality random sources
- **Padding**: Always use proper padding schemes

### Limitations
- **Message Size**: Limited by key size (2048-bit = 256 bytes max)
- **Performance**: Slower than symmetric encryption
- **Use Case**: Best for key exchange, not bulk encryption


## 🔬 Algorithm Details

### RSA Key Generation Process

The RSA key generation follows these mathematical steps:

#### 1. Prime Number Generation
```mathematical
1. Generate two distinct large prime numbers p and q
2. Verify primality using Miller-Rabin test with 50 iterations
3. Ensure |p - q| is sufficiently large to prevent Fermat factorization
```

#### 2. Modulus Calculation
```mathematical
n = p × q
```
Where `n` becomes the RSA modulus (public and private)

#### 3. Euler's Totient Function
```mathematical
φ(n) = (p-1) × (q-1)
```
This represents the count of integers up to `n` that are coprime to `n`

#### 4. Public Exponent Selection
```mathematical
Choose e such that:
- 1 < e < φ(n)
- gcd(e, φ(n)) = 1
- Common choices: e = 3, 17, or 65537 (2^16 + 1)
```

#### 5. Private Exponent Calculation
```mathematical
d ≡ e^(-1) mod φ(n)
```
Where `d` is the modular multiplicative inverse of `e modulo φ(n)`

### RSA Encryption Process

#### Encryption Algorithm
```mathematical
ciphertext = plaintext^e mod n
```

The program encrypts files in blocks, rather than the entire file at once. The ciphertext is written to the output as hexstring.

### RSA Decryption Process

#### Decryption Algorithm
```mathematical
plaintext = ciphertext^d mod n
```

The program decrypts files in blocks, using the private key `d` and modulus `n`. The decoded text is written to the output in plaintext.

### Modular Exponentiation (Square-and-Multiply)

The core operation uses the efficient square-and-multiply algorithm to efficiently compute large exponents:

```algorithm
function modular_exponentiation(base, exponent, modulus):
    result = 1
    base = base mod modulus
    
    while exponent > 0:
        if exponent mod 2 = 1:
            result = (result × base) mod modulus
        exponent = exponent >> 1
        base = (base × base) mod modulus
    
    return result
```

## 📊 Complexity Analysis

Let **k** be the number of bits in the RSA key.

### Time Complexity

| Operation | Time Complexity | Description |
| :--- | :--- | :--- |
| **Key Generation** | O(k⁵) | Dominated by prime number generation. |
| **Prime Testing** | O(k⁴) | Miller-Rabin test with a fixed number of iterations. |
| **Encryption** | O(k²) | Modular exponentiation with a small public exponent. |
| **Decryption** | O(k³) | Modular exponentiation with a large private exponent. |

### Space Complexity

| Component | Space Complexity | Description |
| :--- | :--- | :--- |
| **Key Storage** | O(k) | Storage for public and private keys. |
| **Intermediate** | O(k) | Temporary variables used during calculations. |
| **Total Memory** | O(k) | The memory usage is linear with the key size. |


## 🔧 Troubleshooting

### Common Issues

#### Build Errors

**Problem**: `fatal error: gmp.h: No such file or directory`
```bash
# Ubuntu/Debian
sudo apt-get install libgmp3-dev

# macOS
brew install gmp
```

**Problem**: `undefined reference to '__gmpz_init'`
```bash
# Ensure GMP library is linked
make clean && make
# Or manually:
gcc rsa.c keygen.c -lgmp -o keygen
```

### Permission Errors

**Problem**: Private key file permissions too open
```bash
# Secure private key permissions
chmod 600 private.pem
```


---
NOTE: This program was modified from a Computer Systems and C Programming course assignment. All header files were provided by Professor Darrell Long at UC Santa Cruz.
