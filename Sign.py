from hashlib import sha256
from EllipticCurve import EllipticCurve
from Utils import inverse_mod, secure_random_int, lagrange_interpolation, shamir_share, verify_shamir_share, verify
from Point import Point

# System parameters
# Q_BITS = 256  # Bit length of q in ECC
# N_BITS = 2048 # Bit length of modulus RSA in Paillier

T = 2         # Threshold (minimum number of participants required to cooperate). it is t+1
N = 3         # Total number of participants

p = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
a = 0x0000000000000000000000000000000000000000000000000000000000000000
b = 0x0000000000000000000000000000000000000000000000000000000000000007
_Gx = 0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
_Gy = 0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8

# 1. Key generation step
class Player:
    def __init__(self, id):
        """
        Initializes a Player in the secret sharing scheme.
        
        Args:
            id (int): The unique identifier for the player.
        
        Attributes:
            ui (int): The player's private key, randomly generated in the range [1, q-1].
            yi (y-coordinate): The player's public key, computed as yi = ui * G.
            secret_share (int or None): The player's assigned secret share (initially None).
        """
        self.id = id      # The player's unique identifier from 1 to N
        self.ui = secure_random_int(q)  # Private key for the player
        self.yi = None # Public key computed as yi = ui * G
        self.secret_share = None  # Will store the assigned share of the secret

    def commit(self):
        """
        Computes the cryptographic commitment of the player's public key.
        
        Returns:
            str: The SHA-256 hash of the public key yi.
        """
        return sha256(str(self.yi).encode()).hexdigest()



if __name__ == "__main__":

    # Choose the elliptic curve

    curve = EllipticCurve(p, a, b)

    P = Point(_Gx, _Gy)
    curve.set_generator(P)
    q = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
    curve.order = q
    G = curve.get_generator()
    # Initialize players
    players = [Player(i + 1) for i in range(N)]  # players have index from 1 to N
    for p in players:
        p.yi = curve.multiply(G, p.ui)
    # 2. Public key commitment
    commitments = {p.id: p.commit() for p in players}
    print("Commitments:", commitments)

    # 3. Secret sharing using Feldman VSS
    pub_key_y = curve.sum_points([p.yi for p in players])
    print("Public Key:", pub_key_y)

    # compute secret key
    secret_key = sum(p.ui for p in players) % q  # need to define the addition operation on the curve
    print("Secret Key:", secret_key)

    # create coefficients for the polynomial
    coefficients = [secure_random_int(q) for _ in range(T - 1)]
    coefficients.insert(0, secret_key)  # insert the secret key as the first coefficient
    print("len_Coefficients:", len(coefficients))
    print("Coefficients:", coefficients)

    for p in players:
        p.secret_share = shamir_share(p.id, coefficients, q)

    verify_shares = {v: verify_shamir_share(coefficients[v], curve) for v in range(T)}
    print(verify_shares)
    for p in players:
        p.verify_share = verify(verify_shares, p.secret_share, p.id, curve)
        print(p.verify_share)

    # 4. Digital signature process
    message = "Hello, Threshold ECDSA!"
    m = int(sha256(message.encode()).hexdigest(), 16) % q

    # restruct the secret key from the secret share using Lagrangian algorithm

    # list of player that accepted to sign the message
    accepted_players = []
    while accepted_players.__len__() < T:
        random_player_id = secure_random_int(N)
        if players[random_player_id] not in accepted_players:
            accepted_players.append(players[random_player_id])
    print("Accepted players:", accepted_players)

    indices = [p.id for p in accepted_players]
    print("indices: ", indices)
    accepted_shared = {p.id: p.secret_share for p in accepted_players}
    print("Accepted shares:", accepted_shared)
    lagrange_result = lagrange_interpolation(accepted_shared, indices, q)
    secret_key_restruct = lagrange_result[0]
    lambda_result = lagrange_result[1]
    omega_result = lagrange_result[2]
    print("Secret key restruct: ", secret_key_restruct)

    for i in range(len(indices)):
        accepted_players[i].lambda_i = lambda_result[i]
        accepted_players[i].omega_i = omega_result[i]


    # 4.1 Each player generates random k_i and gamma_i
    for p in accepted_players:
        p.ki = secure_random_int(q)
        p.gamma_i = secure_random_int(q)
        p.Ci = curve.multiply(G, p.gamma_i)  # Commit giá trị gamma_i = tau_i


    # 4.2 Tính tổng k và gamma (ngầm hiểu)
    # k = sum(p.ki for p in accepted_players) % q
    # Gamma = sum(p.gamma_i for p in accepted_players) % q

    # 4.2.1 Chuyển đổi nhân sang cộng với MtA
    alpha = [[0] * T for _ in range(T)]
    beta = [[0] * T for _ in range(T)]
    mu = [[0] * T for _ in range(T)]
    nu = [[0] * T for _ in range(T)]

    # ki * gamma_j = (alpha_ij + beta_ji)  mod q
    for i in range(T):
        for j in range(T):
            if i != j:
                alpha[i][j] = secure_random_int(q)  # Pi creates random alpha_ij then send to Pj
                beta[i][j] = (accepted_players[i].ki * accepted_players[j].gamma_i - alpha[i][j]) % q  # Pj computes beta_ij then send back to Pi
                mu[i][j] = secure_random_int(q)
                nu[i][j] = (accepted_players[i].ki * accepted_players[i].omega_i - mu[i][j]) % q

        accepted_players[i].delta_i = (accepted_players[i].ki * accepted_players[i].gamma_i + sum(alpha[i]) + sum(beta[j][i] for j in range(T) if j != i)) % q
        accepted_players[i].sigma_i = (accepted_players[i].ki * accepted_players[i].omega_i + sum(mu[i]) + sum(nu[j][i] for j in range(T) if j != i)) % q

    # 4.3 Mỗi người chơi phát sóng delta_i và tính delta

    delta = sum(p.delta_i for p in accepted_players) % q  # delta = sum(delta_i) mod q = k * gamma

    if delta == 0:
        raise ValueError("delta không thể bằng 0!")
    delta_inv = inverse_mod(delta, q)

    # 4.4 Tính R và r
    Tau = curve.sum_points([p.Ci for p in accepted_players])
    R = curve.multiply(Tau, delta_inv)  # R = delta^-1 * Tau
    r = int(sha256(str(R).encode()).hexdigest(), 16) % q

    # 4.5 Tính s_i và s
    for p in accepted_players:
        p.si = (m * p.ki + r * p.sigma_i) % q
    s = sum(p.si for p in accepted_players) % q

    print("Signature (r, s):", (r, s))

