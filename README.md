# ESP32 XLock. Generate secure key material based on PUF extractor

This project implements a secure vault system on the ESP32-C3 microcontroller based on the solution proposed in the referenced paper. The project leverages Physically Unclonable Functions (PUFs) to generate and securely store cryptographic keys. The vault initialization and key generation/reconstruction procedures are implemented as described in the paper.

## Overview

The secure vault system provides a method for secure key storage and retrieval using PUFs. The key features of the system include:

- **PUF Response Measurement**: Extract unique, device-specific responses from a PUF.
- **Vault Initialization**: Initialize a vault with locks and indexes derived from the PUF response.
- **Key Generation Procedure (GEN)**: Generate cryptographic keys using the initialized vault and store auxiliary data securely.
- **Key Reconstruction Procedure (REP)**: Reconstruct keys using a new PUF response and verify their validity.
- **NVS Storage**: Store and retrieve critical data securely using the ESP32's Non-Volatile Storage (NVS).

## Key Concepts

- **PUF (Physically Unclonable Function)**: A hardware security feature that generates a unique response based on the physical characteristics of the device.
- **Vault**: A secure storage structure containing locks and indexes to protect cryptographic keys.
- **Locks and Indexes**: Components of the vault used in the GEN and REP procedures to generate and verify keys.

## Prerequisites

- ESP32-C3 microcontroller
- ESP-IDF (Espressif IoT Development Framework) installed and configured
- `mbedtls` library for cryptographic operations


