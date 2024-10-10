# RSA Decryption Project 🔐

## Overview 🌟

This project implements RSA encryption and decryption using two different approaches: a naive algorithm and the Chinese Remainder Theorem (CRT). It measures and compares the time taken for decryption by each method, demonstrating that the CRT approach is significantly faster.

## Features 🚀

- **Encryption and Decryption**: 🔒 Encrypts a plaintext number and decrypts it using two methods.
- **Performance Measurement**: ⏱️ Records the time taken by each decryption method for comparison.
- **Big Number Support**: 💻 Handles large numbers with custom data structures.

## Sample Output 📊

```
Please input a number (plaintext): 3764 The number after encryption is: 7914 The number after decryption is: 3764 The total time taken by 1st method is: 29.501000 s Using the 2nd method, the decrypted number is: 3764 The total time taken by 2nd method is: 0.184000 s
```

## Code Structure 📂

- **rsa_Impl.c**: The main implementation of RSA encryption and decryption.

### Functions

- **`encrypt()`**: ✉️ Encrypts the plaintext using RSA.
- **`decrypt1()`**: 🔑 Decrypts using the naive algorithm.
- **`decrypt2()`**: ⚡ Decrypts using the Chinese Remainder Theorem.


## Running the Program

### Prerequisites

- C compiler (e.g., GCC)

### Steps to Run

1. Clone the repository:
   ```bash
   git clone <repository_url>

2. Import the project into your preferred IDE or text editor.
3. Build the project.
4. Run the rsa_Impl.c file:
 ```bash
  ./rsa_Impl
```

## Conclusion 🎉

The project effectively demonstrates the RSA encryption and decryption process and highlights the performance benefits of using the Chinese Remainder Theorem for decryption.

## Acknowledgments 🙏

- Inspired by the principles of public-key cryptography and the RSA algorithm.
