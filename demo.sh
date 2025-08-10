#!/bin/bash

# RSA Encryption/Decryption Interactive Demo

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[1;35m'
NC='\033[0m' # No Color

# Function to print colored output
print_header() {
    echo -e "\n${BLUE}=== $1 ===${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}ℹ $1${NC}"
}

print_comments() {
    echo -e "${PURPLE}$1${NC}"
    echo -e "\n"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

# Check if we're in the right directory
if [[ ! -f "keygen" || ! -f "encrypt" || ! -f "decrypt" ]]; then
    print_error "RSA programs not found. Building..."
    make all
fi

print_header "RSA Encryption/Decryption Demo"
echo "Welcome to the interactive RSA cryptography demonstration!"
echo "This demo will show you how public-key encryption works."

# Create sample files for demonstration
print_info "Creating sample files..."

# Sample 1: Simple secret message
cat > sample1.txt << EOF
This is a confidential message for RSA encryption demo.
It contains sensitive information that needs to be protected.
The quick brown fox jumps over the lazy dog.
1234567890 - Numbers and special characters: !@#$%^&*()
EOF

# Sample 2: Alice to Bob message
cat > sample2.txt << EOF
Dear Bob,

This is Alice. I hope this message finds you well.
I've encrypted this using your public key so only you can read it.
Let's meet at 3 PM tomorrow at the coffee shop.

Best regards,
Alice
EOF

# Sample 3: Bob's reply
cat > sample3.txt << EOF
Dear Alice,

Thank you for your message. I received it securely.
3 PM at the coffee shop sounds perfect.
I'll bring the documents we discussed.

See you tomorrow,
Bob
EOF

# Create a larger file for performance testing
for i in {1..100}; do
    echo "Line $i: This is test data for RSA encryption performance testing." >> sample4.txt
done

print_success "Sample files created!"

# Function to run RSA demo
run_rsa_demo() {
    local input_file=$1
    local description=$2
    
    print_header "RSA Demo: $description"
    echo "File: $input_file"
    
    # Show original file
    echo -e "\nOriginal content (first 100 chars):"
    head -c 100 "$input_file" | sed 's/$/.../' 2>/dev/null || cat "$input_file"
    
    # Get original size
    original_size=$(wc -c < "$input_file")
    echo -e "\nOriginal size: ${original_size} bytes"
    
    # Generate RSA key pair
    print_info "Generating 2048-bit RSA key pair..."
    print_info "This uses Miller-Rabin primality testing for secure prime generation"
    
    if ./keygen -b 2048 -n "test_${input_file}.pub" -d "test_${input_file}.priv"; then
        print_success "Key pair generated successfully!"
        
        # Show key sizes
        pub_size=$(wc -c < "test_${input_file}.pub")
        priv_size=$(wc -c < "test_${input_file}.priv")
        print_info "Key sizes: Public=${pub_size}B, Private=${priv_size}B"
    else
        print_error "Key generation failed!"
        return 1
    fi
    
    # Encrypt
    print_info "Encrypting with RSA public key..."
    if ./encrypt -i "$input_file" -o "${input_file}.enc" -n "test_${input_file}.pub"; then
        encrypted_size=$(wc -c < "${input_file}.enc")
        expansion_ratio=$(echo "scale=2; $encrypted_size/$original_size" | bc -l)
        
        print_success "Encryption successful!"
        echo "Encrypted size: ${encrypted_size} bytes"
        echo "Size expansion: ${expansion_ratio}x (RSA adds overhead)"
    else
        print_error "Encryption failed!"
        return 1
    fi
    
    # Show encrypted data
    echo -e "\nEncrypted data (first 20 bytes):"
    head -c 20 "${input_file}.enc" | hexdump -C
    
    # Decrypt
    print_info "Decrypting with RSA private key..."
    if ./decrypt -i "${input_file}.enc" -o "${input_file}.decrypted" -n "test_${input_file}.priv"; then
        # Verify integrity
        if diff -q "$input_file" "${input_file}.decrypted" > /dev/null; then
            print_success "Integrity check passed! Original and decrypted files are identical."
        else
            print_error "Integrity check failed! Files differ."
            return 1
        fi
    else
        print_error "Decryption failed!"
        return 1
    fi
    
    # Show decrypted content
    echo -e "\nDecrypted content (first 100 chars):"
    head -c 100 "${input_file}.decrypted" | sed 's/$/.../' 2>/dev/null || cat "${input_file}.decrypted"
    echo -e "\n"
}

# Run demos for all sample files
print_header "Starting RSA Encryption Demonstrations"

run_rsa_demo "sample1.txt" "Secret Message"
print_comments "Notice the encrypted file was larger than the original file.
This is due to RSA's padding overhead. RSA encryption adds significant overhead
to the original data, especially for small files."
sleep 5

run_rsa_demo "sample2.txt" "Alice to Bob Message"
print_comments "Notice how RSA ensures only the intended recipient can read the message.
Alice encrypted this with Bob's public key, ensuring only Bob's private key can decrypt it.
This demonstrates the power of public-key cryptography."
sleep 5

run_rsa_demo "sample3.txt" "Bob's Reply"
print_comments "Notice the bidirectional nature of RSA encryption.
Bob can encrypt his reply with Alice's public key, ensuring only Alice can read it.
This enables secure two-way communication."
sleep 5

run_rsa_demo "sample4.txt" "Large File Performance Test"
print_comments "Notice the performance impact with larger files.
RSA encryption becomes slower with larger data due to mathematical complexity.
In practice, RSA is often used to encrypt symmetric keys, not large data directly."
sleep 5

# Key size comparison demo
print_header "RSA Key Size Comparison"
print_info "Testing different key sizes for performance and security..."

key_sizes=(1024 2048 4096)
for size in "${key_sizes[@]}"; do
    print_info "Testing $size-bit key..."
    
    # Time key generation
    start_time=$(date +%s.%N)
    if ./keygen -b $size -n "test_${size}.pub" -d "test_${size}.priv"; then
        end_time=$(date +%s.%N)
        duration=$(echo "$end_time - $start_time" | bc -l)
        
        # Get key sizes
        pub_size=$(wc -c < "test_${size}.pub")
        priv_size=$(wc -c < "test_${size}.priv")
        
        print_success "✓ $size-bit: Generated in ${duration}s, Public=${pub_size}B, Private=${priv_size}B"
    else
        print_error "✗ $size-bit: Failed"
    fi
done

# Interactive section
print_header "Interactive Mode"
print_info "Let's try with your own text!"

read -p "Enter some text to encrypt (or press Enter to skip): " user_text
if [[ -n "$user_text" ]]; then
    echo "$user_text" > user_input.txt
    run_rsa_demo "user_input.txt" "Your Custom Text"
fi

# Performance demonstration
print_header "Performance Analysis"
print_info "Running performance benchmarks..."

# Create larger test file
base64 /dev/urandom | head -c 10000 > large_test.txt
run_rsa_demo "large_test.txt" "10KB Random Data"
print_comments "Notice the significant performance impact with larger files.
RSA encryption has O(n³) complexity, making it impractical for large files.
This is why hybrid encryption (RSA + symmetric) is used in practice."

# Summary
print_header "Demo Summary"
echo "✅ RSA encryption successfully demonstrated!"
echo "✅ Public/private key cryptography working correctly"
echo "✅ Data integrity verified for all test cases"
echo "✅ Performance characteristics analyzed"

# Cleanup
print_info "Cleaning up temporary files..."
rm -f sample*.txt sample*.enc sample*.decrypted sample*.pub sample*.priv
rm -f user_input.txt large_test.txt wrong_decrypted.txt test_*.pub test_*.priv

print_success "Demo complete! Thank you for trying the RSA Encryption/Decryption demo."
sleep 3