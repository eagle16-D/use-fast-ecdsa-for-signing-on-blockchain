# import random
# from ecdsa.numbertheory import inverse_mod
# def Invert_lagrange(p:list, id:list, q):
#     lam = []
#     for i in id:
#         lamda_i = 1
#         for j in id:
#             if j != i:
#                 lamda = j / (j - i)
#                 lamda_i *= lamda
#         lam.append(lamda_i)
#     print("lamda_i = ",lam)
#     return sum(lam[i] * p[i] for i in range(len(p))) % q

# def Invert_lagrange2(p:list, id:list, q):
#     lam = []
#     for i in id:
#         lamda_i = 1
#         for j in id:
#             if j != i:
#                 num = j % q
#                 denom = (j - i) % q
#                 lamda_i *= (num * inverse_mod(denom, q)) % q
#         lam.append(lamda_i)
#     print("lamda_i = ",lam)
#     return sum(lam[i] * p[i] for i in range(len(p))) % q

# def shamir_share(x, coeffs, q):
#     return sum([coeffs[i] * (x ** i) for i in range(len(coeffs))]) % q

# def to_binary(value):
#         """Chuyển một số nguyên thành chuỗi nhị phân 256-bit."""
#         return bin(value)[2:].zfill(256)
# def to_hex(value):
#     return hex(value)[2:].zfill(64)

# if __name__ == "__main__":
#     # q = 7919
#     # coefficients = [1234, 16, 2]
#     # id = [1, 3, 5]
#     # p = [shamir_share(i, coefficients, q) for i in id]
#     # sigma_1 = Invert_lagrange(p, id, q)
#     # sigma_2 = Invert_lagrange2(p, id, q)
#     # print(sigma_1)
#     # print(sigma_2)

#     print(to_hex(1234))
#     print(0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)


# from Utils import is_prime
# print(is_prime(2))