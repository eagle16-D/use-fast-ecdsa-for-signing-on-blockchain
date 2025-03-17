# **Fast Multiparty Threshold ECDSA with Fast Trustless Setup**

## **Project Description**

This project implements a **fast multiparty threshold ECDSA** protocol based on the paper *Fast Multiparty Threshold ECDSA with Fast Trustless Setup*. The protocol enables **n** participants to collectively share an ECDSA signing key and generate valid signatures without requiring a trusted dealer. A subset of at least **t** participants can collaboratively produce a valid signature, whereas a group smaller than **t** cannot.

The protocol offers improvements in:

- **Dealerless Key Generation:** No need for a trusted third party to set up the shared signing key.
- **High Efficiency:** Significantly reduces computational overhead and communication complexity.
- **Security Against Malicious Majority:** Ensures security even if multiple participants are controlled by an adversary.

This project is designed to facilitate **digital signing on blockchain**, specifically for cryptocurrency transactions such as Bitcoin and Ethereum.

---

## **Project Structure**

### **EllipticCurve.py**

This file defines the **EllipticCurve** class, which represents an elliptic curve over a finite field. It includes:

- **Curve Initialization:** Defines curve parameters such as `p, a, b, G, order`.
- **Validation Methods:** Ensures points lie on the curve and calculates modular inverses.
- **Point Addition and Multiplication:** Implements basic elliptic curve operations.
- **Other Utilities:** Computes inverses, finds generator points, determines point order, sums multiple points, and checks or sets the generator.

### **Point.py**

Defines a **Point** class to represent coordinates on the elliptic curve. It provides:

- **Point Representation:** Stores `x` and `y` coordinates.

### **Utils.py**

Provides utility functions such as:

- **Secure Random Number Generation:** Generates cryptographically secure random numbers in `Z_q`.
- **Modular Arithmetic Operations:** Includes modular inverse computation.
- **Lagrange Interpolation:** Implements secret reconstruction in threshold cryptography.
- **Shamir's sharing:** Sharing the need to restruct the private key for every player.

### **Sign.py**

The main program for generating key and signing the message

---

## **Main Steps in the Protocol**

### **1. Distributed Key Generation (DKG)**

- Each participant **Pᵢ** generates a private key **uᵢ** and computes the public key **yᵢ = G ∗ uᵢ**.
- A **commitment scheme** is used to ensure consistency.
- Secret shares are distributed using **Feldman’s Verifiable Secret Sharing (VSS)**.

### **2. Secret Key Sharing and Management**

- The secret signing key **x** is distributed using **Shamir’s Secret Sharing**, ensuring that it can only be reconstructed with at least **t** shares.

### **3. Signature Generation**

- Signing participants randomly generate **k** and **γ**, using the **Multiplicative-to-Additive (MtA) Share Conversion** protocol to convert multiplicative shares into additive shares.
- The signature **(r, s)** is computed following the **ECDSA** standard.

---

## **Block Diagram**

