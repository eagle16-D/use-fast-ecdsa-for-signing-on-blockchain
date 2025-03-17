# g = 5
# q = 17

# def check_generator(g, q):
#     # Check if g is a generator of the multiplicative group of integers modulo q
#     t = []
#     for i in range(1, q):
#         t.append(pow(g, i, q))
#     if len(t) != q - 1:
#         return False
#     t.sort()
#     for i in range(1, q):
#         if t[i - 1] != i:
#             return False
#     return True

# def shamirShare(x, coeffs, q):
#     """Compute the Shamir's Secret Sharing share for a given index."
#     """
#     p_i = sum(coeffs[i] * x**i % q for i in range(len(coeffs))) % q
#     v_i = pow(g, coeffs[x-1], q)
#     return (v_i, p_i)

# t = 3
# def verify_shamir_share(p_i, v, q, sigma_i, t):
#     prod = 1
#     for i in range(len(t)):
#         prod *= pow(v, pow(p_i, i, q), q)

#     return prod % q == pow(g, sigma_i, q)

# def find_generator(q):
#     for g in range(2, q):
#         if check_generator(g, q):
#             return g

# if __name__ == '__main__':
#     print(check_generator(5, 17))
#     print(find_generator(17))
#     coeffs = [1234, 16, 2]
#     x = 1234
#     p_i = [1, 2, 3]
#     v, p = shamirShare(1, coeffs, q)
#     print(verify_shamir_share(p_i, v, q, p, coeffs))


v = {1: 16, 2:2}
print(v[1])
for x, y in v.items():
    print(x, y)