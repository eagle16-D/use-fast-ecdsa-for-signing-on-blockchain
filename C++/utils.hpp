#ifndef IROHA_CRYPTO_UTILS_HPP
#define IROHA_CRYPTO_UTILS_HPP

#include <map>
#include <vector>

#include "bigInt.hpp"
#include "elliptic_curve.hpp"

namespace shared_model {
  namespace crypto {

    /*
     * Hàm tính lũy thừa của base với exp theo modulo mod
     * @param base Cơ sở
     * @param exp Số mũ
     * @param mod Modulo
     * @return Kết quả của (base^exp) mod mod
     */
    BigInt pow_mod(BigInt base, BigInt exp, const BigInt &mod);

    /// @brief Sinh số ngẫu nhiên an toàn trong khoảng [low, high]
    /// @param low Giới hạn dưới
    /// @param high Giới hạn trên
    /// @return Số ngẫu nhiên trong khoảng [low, high]
    /// @throws std::invalid_argument nếu low > high
    /// @note giá trị trả về nằm trong khoảng [low, high], bao gôm cả low và
    /// high
    BigInt secureRandom(const BigInt &low, const BigInt &high);

    /*
     * Hàm tính nghịch đảo modulo của k với q
     * @param k Số nguyên cần tính nghịch đảo
     * @param q Modulo
     * @return Nghịch đảo của k mod q
     */
    BigInt inverseMod(const BigInt &k, const BigInt &q);

    /*
     * Hàm kiểm tra xem p có phải là số nguyên tố hay không
     * @param p Số nguyên cần kiểm tra
     * @return true nếu p là số nguyên tố, false nếu không
     * @note Sử dụng thuật toán Miller-Rabin
     * @param k Số lần kiểm tra (số lần lặp lại)
     * @note k càng lớn thì xác suất sai càng thấp
     */
    bool isPrime(BigInt &n, int k = 25);

    /*
     * Nhóm kết quả của Lagrange interpolation
     * @param x Secret tái tạo
     * @param lambda Các lambda_i
     * @param omega Các omega_i = lambda_i * p(i)
     */
    struct LagrangeResult {
      BigInt x;                    // Secret tái tạo
      std::vector<BigInt> lambda;  // Các lambda_i
      std::vector<BigInt> omega;   // Các omega_i = lambda_i * p(i)
    };

    /*
     * Hàm thực hiện Lagrange interpolation
     * @param shares Bảng chia sẻ
     * @param indices Chỉ số của các phần tử trong shares
     * @param q Modulo
     * @return Kết quả Lagrange interpolation
     */
    LagrangeResult lagrangeInterpolation(const std::map<int, BigInt> &shares,
                                         const std::vector<int> &indices,
                                         BigInt q);

    /*
     * Hàm tính giá trị của đa thức p(xi) trong Shamir's Secret Sharing Scheme.
     * 
     * Đa thức được định nghĩa là:
     *     p(x) = sigma + coeffs[1] * x + coeffs[2] * x^2 + ... + coeffs[T-1] * x^(T-1) mod q
     *     trong đó sigma = coeffs[0]
     * @param x Tọa độ x để tính p(x)
     * @param coeffs Danh sách các hệ số của đa thức, trong đó phần tử đầu tiên là khóa bí mật 
     * @param q Số nguyên tố dùng cho phép toán modulo
     * @return Phần chia p(xi) mod q
     */
    BigInt shamirShare(int x,
                  const std::vector<BigInt> &coeffs,
                  BigInt q);

    /*
     * Hàm tính giá trị xác minh v_i = g^a_i
     * @param coeffs Các hệ số của đa thức
     * @param curve Đường cong elliptic
     * @return Giá trị xác minh
     */
    Point verifyShamirShare(const std::vector<BigInt> &coeffs,
                            const EllipticCurve &curve);

    /*
      * Hàm xác minh chia sẻ trong Shamir Secret Sharing:
      g^sigma_i = sum(v_i^z_i) in G
      * @param v Bảng chia sẻ
      * @param sigma_i Giá trị sigma_i
      * @param id Chỉ số của phần tử cần xác minh
      * @param curve Đường cong elliptic
      * @return true nếu chia sẻ hợp lệ, false nếu không
    */
    bool verify(const std::map<int, Point> &v,
                BigInt sigma_i,
                int id,
                const EllipticCurve &curve);

  }  // namespace crypto
}  // namespace shared_model

#endif  // IROHA_CRYPTO_UTILS_HPP