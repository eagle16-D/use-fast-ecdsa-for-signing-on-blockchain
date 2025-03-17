def is_prime(p):
    if p is None:
        return False
    if p < 2:
        return False
    for i in range(2, int(p**0.5) + 1):
        if p % i == 0:
            return False
    return True

def inverse_mod(k, q):
    if k == 0:
        raise ValueError("it has no inverse modulo for 0")
    return pow(k, q - 2, q)


def lagrange_interpolation(shares:dict, indices, q):
    """
    Recovers the secret value using Lagrange interpolation.
    Retruns the list of the products of the lamda with the corresponding shares: lamda_i * p(i)
    p(i) = shares[pi_id]

    """
    x = 0
    lamda = []
    omega = []
    for pi_id in indices:
        num, denom = 1, 1
        for pj_id in indices:
            if pi_id != pj_id:
                num = (num * pj_id) % q
                denom = (denom * (pj_id - pi_id)) % q
        lambda_i = (num * inverse_mod(denom, q)) % q
        lamda.append(lambda_i)  #use this to compute omega_i = lambda_i * p(i)
        omega_i = (shares[pi_id] * lambda_i) % q
        omega.append(omega_i)
    x = sum(omega) % q
    return x, lamda, omega


import os

def secure_random_int(q: int) -> int:
    """
    Generate a secure random integer in the range [1, q-1] using os.urandom().
    
    Args:
        q (int): The upper limit (prime number q in field Z_q).
    
    Returns:
        int: A random integer in the range [1, q-1].
    """
    num_bytes = (q.bit_length() + 7) // 8  # Number of bytes needed to represent q
    while True:
        rand_bytes = os.urandom(num_bytes)  # Generate random bytes
        rand_int = int.from_bytes(rand_bytes, "big") % q  # Convert to integer mod q
        if 1 <= rand_int < q:  # Ensure the value is not 0
            return rand_int


def shamir_share(x, coeffs, q):
    """
    Computes the value of the polynomial p(xi) in Shamir's Secret Sharing Scheme.
    
    The polynomial is defined as:
        p(x) = sigma + coeffs[1] * x + coeffs[2] * x^2 + ... + coeffs[T-1] * x^(T-1) mod q
        in which sigma = coeffs[0]
    Args:
        x (int): The x-coordinate where p(x) is evaluated.
        sigma (int): The secret to be shared (constant term of the polynomial).
        coeffs (list): A list of (T-1) randomly chosen coefficients with the first element is the private key.
        q (int): A prime number used for modular arithmetic.
    
    Returns:
        int: The computed share p(xi) mod q.
    """
    p_i = sum(coeffs[i] * pow(x, i, q) for i in range(len(coeffs))) % q
    return p_i

def verify_shamir_share(coeffs, curve):
    return curve.multiply(curve.G, coeffs)
def verify(v:dict, sigma_i, id, curve): 
    # sigma_i is the secret to be shared p(i)
    left = curve.multiply(curve.G, sigma_i)
    right = curve.O
    for i, v_i in v.items():
        right = curve.add(right, curve.multiply(v_i, pow(id, i, curve.order)))
    return curve.coincide(left, right)