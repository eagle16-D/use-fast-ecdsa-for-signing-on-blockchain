def is_prime(p):
    if p == None:
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


import os
def secure_random_int(q: int) -> int:
    """
    Sinh số ngẫu nhiên an toàn trong khoảng [1, q-1] bằng cách sử dụng os.urandom().
    
    Args:
        q (int): Giá trị giới hạn (số nguyên tố q trong trường Z_q).
    
    Returns:
        int: Số ngẫu nhiên trong khoảng [1, q-1].
    """
    num_bytes = (q.bit_length() + 7) // 8  # Số bytes cần thiết để biểu diễn q
    while True:
        rand_bytes = os.urandom(num_bytes)  # Tạo số ngẫu nhiên dạng bytes
        rand_int = int.from_bytes(rand_bytes, "big") % q  # Chuyển thành số nguyên mod q
        if 1 <= rand_int < q:  # Đảm bảo không lấy giá trị 0
            return rand_int
        

q = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
random_value = secure_random_int(q)

print("🔹 Giá trị ngẫu nhiên:", random_value)