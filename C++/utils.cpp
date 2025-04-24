#include "utils.hpp"

#include <sodium.h>

#include <cmath>
#include <stdexcept>

#include "bigInt.hpp"
#include "elliptic_curve.hpp"
#include "point.hpp"

namespace shared_model {
  namespace crypto {

    BigInt pow_mod(BigInt base, BigInt exp, const BigInt &mod) {
      BigInt result(1);
      base = base % mod;

      while (exp > BigInt(0)) {
        if ((exp % BigInt(2)) == BigInt(1)) {
          result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp = exp / BigInt(2);
      }

      return result;
    }

    BigInt secureRandom(const BigInt &low, const BigInt &high) {
      if (low >= high) {
        throw std::invalid_argument("low >= high");
      }
      // Tính khoảng [0, range-1] với range = high - low + 1
      BigInt range = high - low + BigInt(1);
      size_t bits = range.bit_length();
      size_t bytes = (bits + 7) / 8;

      std::vector<uint8_t> buf(bytes);
      while (true) {
        // Sinh ngẫu nhiên bytes an toàn
        randombytes_buf(buf.data(), buf.size());

        // Mask bớt các bit vượt quá độ dài cần thiết
        size_t extra_bits = bytes * 8 - bits;
        if (extra_bits) {
          uint8_t mask = uint8_t(0xFF >> extra_bits);
          buf[0] &= mask;
        }

        // Chuyển thành BigInt
        BigInt rnd(buf);
        // Nếu rnd < range thì chấp nhận, else lặp lại
        if (rnd < range) {
          return low + rnd;
        }
        // còn >= range thì reject và sinh lại
      }
    }
    // Hàm tính ước số chung lớn nhất của hai số
    BigInt gcd(const BigInt &a, const BigInt &b) {
      if (b == BigInt(0)) {
        return a;
      }
      return gcd(b, a % b);
    }

    BigInt inverseMod(const BigInt &k, const BigInt &p) {
      // Nếu k = 0, ném ngoại lệ
      if (k == BigInt(0)) {
        throw std::invalid_argument("k is 0");
      }
      // Nếu k không có nghịch đảo modulo q, ném ngoại lệ
      if (gcd(k, p) != BigInt(1)) {
        throw std::invalid_argument("k has no inverse modulo q");
      }
      // Sử dụng Extended Euclidean Algorithm để tìm nghịch đảo
      BigInt r0 = p, r1 = k;
      BigInt t0 = BigInt(0), t1 = BigInt(1);
      while (r1 > BigInt(0)) {
        BigInt q = r0 / r1;
        BigInt r2 = r0 - q * r1;
        BigInt temp = q * t1;
        BigInt t2;
        if (temp > t0) {
          while (temp > p)
          {
            temp = temp - p;
          }
          
          temp = p - temp;
        }
        t2 = (t0 + temp) % p;

        r0 = r1;
        r1 = r2;
        t0 = t1;
        t1 = t2;
      }
      // Nếu t0 < 0, thêm q vào để đảm bảo t0 là dương
      if (t0 < BigInt(0)) {
        t0 = t0 + p;
      }
      // Trả về t0 là nghịch đảo của k mod q
      return t0;
    }

    bool is_even(const BigInt &n) {
      return (n % BigInt(2)) == BigInt(0);
    }

    bool isPrime(BigInt &n, int k) {
      if (n <= BigInt(1))
        return false;
      if (n == BigInt(2) || n == BigInt(3))
        return true;
      if (is_even(n))
        return false;

      // Viết n - 1 = 2^r * d
      BigInt d = n - BigInt(1);
      int r = 0;
      while (is_even(d)) {
        d = d / BigInt(2);
        ++r;
      }

      for (int i = 0; i < k; ++i) {
        BigInt a = secureRandom(BigInt(2), n - BigInt(2));
        BigInt x = pow_mod(a, d, n);
        if (x == BigInt(1) || x == n - BigInt(1))
          continue;

        bool continue_outer = false;
        for (int j = 0; j < r - 1; ++j) {
          x = pow_mod(x, BigInt(2), n);
          if (x == n - BigInt(1)) {
            continue_outer = true;
            break;
          }
        }

        if (!continue_outer)
          return false;
      }

      return true;
    }

    LagrangeResult lagrangeInterpolation(const std::map<int, BigInt> &shares,
                                         const std::vector<int> &indices,
                                         BigInt q) {
      LagrangeResult result;
      result.x = BigInt(0);

      for (int pi_id : indices) {
        BigInt num = BigInt(1);
        BigInt denom = BigInt(1);

        for (int pj_id : indices) {
          if (pi_id != pj_id) {
            num = (num * BigInt(pj_id)) % q;
            if (pj_id < pi_id) {
              BigInt temp = q - BigInt(pi_id);
              denom = (denom * (temp + BigInt(pj_id))) % q;
            }
            else {
              denom = (denom * (BigInt(pj_id) - BigInt(pi_id))) % q;
            }
          }
        }

        BigInt lambda_i = (num * inverseMod(denom, q)) % q;
        result.lambda.push_back(lambda_i);

        BigInt omega_i = (shares.at(pi_id) * lambda_i) % q;
        result.omega.push_back(omega_i);

        result.x = (result.x + omega_i) % q;
      }

      return result;
    }

    BigInt shamirShare(int x, const std::vector<BigInt> &coeffs, BigInt q) {
      BigInt result = BigInt(0);
      BigInt x_big = BigInt(x);
      BigInt x_pow = BigInt(1);  // x^0 initially

      for (size_t i = 0; i < coeffs.size(); ++i) {
        result = (result + (coeffs[i] * x_pow) % q) % q;
        x_pow = (x_pow * x_big) % q;  // x^i
      }

      return result;
    }

    Point verifyShamirShareValue(const BigInt &coeff,
                            const EllipticCurve &curve) {
      Point result ;

      result = curve.multiply(curve.G(), coeff);

      return result;
    }

    bool verify(const std::map<int, Point> &v,
                BigInt sigma_i,
                int id,
                const EllipticCurve &curve) {
      Point left = curve.multiply(curve.G(), sigma_i); // g^sigma_i
      Point right = curve.O();
      BigInt id_ = BigInt(id);
      BigInt order = curve.order();

      for (const auto &[i, v_i] : v) {
        BigInt exp = pow_mod(id_, BigInt(i), order);
        Point term = curve.multiply(v_i, exp);
        right = curve.add(right, term);
      }

      return curve.coincide(left, right);
    }

  }  // namespace crypto
}  // namespace shared_model

int main(int argc, char const *argv[]) {
  using namespace shared_model::crypto;

  // Test the functions here

  // Example usage of the functions
  BigInt p(43);
  BigInt q(47);
  BigInt k(5);
  BigInt mod(7);
  BigInt result = pow_mod(p, q, mod);
  std::cout << "Result of pow_mod: " << result.to_hex() << std::endl;
  BigInt random_num = secureRandom(p, q);
  std::cout << "Random number between p and q: " << random_num.to_hex()
            << std::endl;
  bool is_even_result = is_even(k);
  std::cout << "Is k even? " << (is_even_result ? "Yes" : "No") << std::endl;
  bool is_prime_result = isPrime(p);
  std::cout << "Is p prime? " << (is_prime_result ? "Yes" : "No") << std::endl;

  BigInt inverse_result = inverseMod(k, mod);
  std::cout << "Inverse of k mod q: " << inverse_result.to_hex() << std::endl;
  if(!isPrime(p)){
    std::cout << "p is not prime" << std::endl;
  }
  else{
    std::cout << "p is prime" << std::endl;
  }

  std::map<int, BigInt> shares = {{1, BigInt(18)}, {2, BigInt(20)}};
  std::vector<int> indices = {1, 2};
  LagrangeResult lagrange_result = lagrangeInterpolation(shares, indices, q);
  std::cout << "Lagrange interpolation result: " << lagrange_result.x.to_hex()
            << std::endl;
  std::cout << "Lagrange lambda values: ";
  for (const auto &lambda : lagrange_result.lambda) {
    std::cout << lambda.to_hex() << " ";
  }
  std::cout << std::endl;
  std::cout << "Lagrange omega values: ";
  for (const auto &omega : lagrange_result.omega) {
    std::cout << omega.to_hex() << " ";
  }
  std::cout << std::endl;
  return 0;
}
