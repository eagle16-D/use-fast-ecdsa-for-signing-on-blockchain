from Utils import is_prime
from Point import Point
class EllipticCurve:
    """
    Lớp đại diện cho một đường cong elliptic trên trường hữu hạn Z_p.
    Đường cong được biểu diễn bởi phương trình: y^2 = x^3 + ax + b mod p
    
    Thuộc tính:
    - p: số nguyên tố
    - a, b: hệ số trong phương trình đường cong
    - O: điểm vô cực (Identity element)
    - G: điểm sinh (Generator Point)
    - order: bậc của điểm sinh G

    Phương thức:
    - is_on_curve(P): Kiểm tra điểm P có nằm trên đường cong hay không
    - inverse_mod(k): Tính nghịch đảo modulo của k trong trường Z_p
    - add(P, Q): Cộng hai điểm P và Q trên đường cong
    - multiply(k, P): Nhân điểm P với số nguyên k
    - find_generator(): Tìm một điểm sinh G trên đường cong
    - find_order(P): Tìm bậc của một điểm P
    - is_generator(P): Kiểm tra xem điểm P có phải là điểm sinh không
    - set_generator(P): Đặt điểm sinh G thủ công

    """
    def __init__(self, p, a, b):
        """Khởi tạo đường cong Elliptic: y^2 = x^3 + ax + b mod p"""
        self.p = p  # Trường hữu hạn Z_p
        self.a = a  # Hệ số a trong phương trình đường cong
        self.b = b  # Hệ số b trong phương trình đường cong
        self.O = Point(None, None)  # Điểm vô cực (Identity element)
        self.G = None  # Điểm sinh (Generator Point)
        self.order = None  # Bậc của điểm sinh G

    def is_on_curve(self, P:Point):
        """Kiểm tra điểm P có nằm trên đường cong hay không."""
        if P == self.O:
            return True
        x, y = P.x, P.y
        return (y**2 - (x**3 + self.a * x + self.b)) % self.p == 0

    def inverse_mod(self, k):
        """Tính nghịch đảo modulo của k trong trường Z_p."""
        if k == 0:
            raise ValueError("it has no inverse modulo for 0")
        # Sử dụng Fermat's Little Theorem k^(-1) = k^(q-2)  mod p
        return pow(k, self.p - 2, self.p)
    
    def coincide(self, P, Q):
        """Kiểm tra hai điểm P và Q có trùng nhau không."""
        return P.x == Q.x and P.y == Q.y

    def add(self, P, Q):
        """Cộng hai điểm P và Q trên đường cong."""
        if P == self.O:
            return Q
        if Q == self.O:
            return P

        x1, y1 = P.x, P.y
        x2, y2 = Q.x, Q.y

        if x1 == x2 and y1 != y2:
            return self.O  # P + (-P) = O
        if P == Q:
            s = (3 * x1**2 + self.a) * self.inverse_mod(2 * y1) % self.p
        else:
            s = (y2 - y1) * self.inverse_mod(x2 - x1) % self.p

        x3 = (s**2 - x1 - x2) % self.p
        y3 = (s * (x1 - x3) - y1) % self.p

        return Point(x3, y3)

    def multiply(self, P, k):
        """Nhân điểm P với số nguyên k >= 0 trên đường cong."""
        if P == None: return None
        if self.coincide(P, self.O):
            return self.O
        if k < 0:
            raise ValueError("k must be >= 0")# Không hỗ trợ nhân với số âm
        if self.order:
            k = k % self.order
        if P == self.O or k == 0:
            return self.O
        R = self.O
        Q = P
        while k > 0:
            if k & 1:  # lẻ thì cộng rồi nhân đôi Q
                R = self.add(R, Q)
            Q = self.add(Q, Q)
            k >>= 1
        return R

    def find_generator(self):
        """Tìm một điểm sinh G trên đường cong."""
        for x in range(self.p):
            rhs = (x**3 + self.a * x + self.b) % self.p
            for y in range(self.p):
                if (y**2 - rhs) % self.p == 0:
                    P = Point(x, y)
                    order = self.find_order(P)
                    if is_prime(order):
                        self.G = P
                        self.order = order
                        return P
        return None

    def find_order(self, P):
        """Tìm bậc của một điểm P (tức là số nguyên nhỏ nhất n sao cho nP = O)."""
        if not self.is_on_curve(P):
            raise ValueError("Điểm không thuộc đường cong!")
        for n in range(1, self.p + 1):
            tmp = self.multiply(P,n)
            if self.coincide(tmp, self.O):
                return n
        return None

    def is_generator(self, P):
        """Kiểm tra xem điểm P có phải là điểm sinh không."""
        return self.find_order(P) is not None

    def set_generator(self, P:Point):
        """Đặt điểm sinh G thủ công."""
        if P == None or self.coincide(P, self.O):
            print(99)
            raise ValueError("Điểm sinh không hợp lệ!")
        if self.is_on_curve(P):
            self.G = P
        else:
            raise ValueError("Điểm không thuộc đường cong!")
    def get_generator(self):
        return self.G
        
    # compute the sum of all point in the list of points
    def sum_points(self, points):
        sum = self.O
        for point in points:
            sum = self.add(sum, point)
        return sum
        

# Khởi tạo một đường cong elliptic ví dụ
if __name__ == "__main__":
    # curve = EllipticCurve(p=17, a=2, b=2)
    # # G = curve.find_generator()
    # G = Point(None, None)
    # print(G == curve.O)
    # try:
    #     curve.set_generator(G)
    # except ValueError as e:
    #     print(e)
    # print("Generator G:", G)
    # print("Order of G:", curve.order)

    # # Kiểm tra phép cộng
    # P = Point(5, 1)
    # Q = Point(6, 3)
    # R = curve.add(P, Q)
    # print(f"{P} + {Q} = {R}")
    # print(R.x, R.y, curve.is_on_curve(R))

    # # Kiểm tra phép nhân
    # nP = curve.multiply(G, 3)
    # print(f"3 * G = {nP}")

    # print("inverse Of 5: ", curve.inverse_mod(5))
    Gx = 0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
    Gy = 0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8
    p = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
    a = 0
    b = 7
    curve = EllipticCurve(p, a, b)
    q = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
    G = Point(Gx, Gy)
    T = curve.multiply(G, q)
    print(curve.coincide(T, curve.O))