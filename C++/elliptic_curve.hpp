#ifndef IROHA_ELLIPTIC_CURVE_HPP
#define IROHA_ELLIPTIC_CURVE_HPP

#include <cstdint>
#include <stdexcept>
#include <vector>
#include "point.hpp"
#include "bigInt.hpp"
#include "utils.hpp"

namespace shared_model {
  namespace crypto {

    // Lớp đại diện cho đường cong elliptic
    class EllipticCurve {
     public:
     /*
      * Constructor cho đường cong elliptic
      * @param p Trường hữu hạn Z_p
      * @param a Hệ số a
      * @param b Hệ số b
      * @note Đường cong elliptic được định nghĩa bởi phương trình y^2 = x^3 + ax + b mod p
      * @note p phải là số nguyên tố
      * @throws std::invalid_argument nếu p không phải là số nguyên tố
     */
      EllipticCurve(BigInt &p, BigInt a, BigInt b);
      ~EllipticCurve() = default;

      // lấy giá trị a và b
      BigInt a() const {
        return a_;
      }
      BigInt b() const {
        return b_;
      }

      /*
      * Hàm kiểm tra xem một điểm có nằm trên đường cong elliptic hay không
      * @param P Điểm cần kiểm tra
      * @return true nếu điểm nằm trên đường cong, false nếu không
      * @note Nếu điểm là điểm vô cực, hàm sẽ trả về true
      */
      bool isOnCurve(const Point &P) const;

      /*
      * Hàm tính nghịch đảo modulo của k với p
      * @param k Số nguyên cần tính nghịch đảo
      * @return Nghịch đảo của k mod p
      * @note Nếu k = 0, hàm sẽ ném ra ngoại lệ std::invalid_argument
      * @note Nếu k không có nghịch đảo modulo p, hàm sẽ ném ra ngoại lệ std::invalid_argument
      */
      BigInt inverseMod(BigInt k) const;

      /*
      * Hàm kiểm tra xem hai điểm có trùng nhau hay không
      * @param P Điểm thứ nhất
      * @param Q Điểm thứ hai
      * @return true nếu hai điểm trùng nhau, false nếu không
      * @note Nếu một trong hai điểm là điểm vô cực, hàm sẽ trả về false
      */
      bool coincide(const Point &P, const Point &Q) const;

      /*
      * Hàm cộng hai điểm trên đường cong elliptic
      * @param P Điểm thứ nhất
      * @param Q Điểm thứ hai
      * @return Điểm tổng của hai điểm
      * @note Nếu một trong hai điểm là điểm vô cực, hàm sẽ trả về điểm còn lại
      */
      Point add(const Point &P, const Point &Q) const;

      /*
      * Hàm nhân một điểm với một số nguyên k
      * @param P Điểm cần nhân
      * @param k Số nguyên cần nhân
      * @return Điểm kết quả của phép nhân
      * @note Nếu k = 0, hàm sẽ trả về điểm vô cực
      * @note Nếu k < 0, hàm sẽ ném ra ngoại lệ std::invalid_argument
      * @note Nếu k > 0, hàm sẽ trả về điểm P nhân với k
      */
      Point multiply(const Point &P, BigInt k) const;

      /*
      * Hàm tìm điểm sinh của đường cong elliptic
      * @return Điểm sinh của đường cong
      */
      Point findGenerator();
      BigInt findOrder(const Point &P) const;
      bool isGenerator(const Point &P) const;
      void setGenerator(const Point &P);
      Point getGenerator() const;
      Point sumPoints(const std::vector<Point> &points) const;

     private:
      BigInt p_;      // Trường hữu hạn Z_p
      BigInt a_;      // Hệ số a
      BigInt b_;      // Hệ số b
      Point O_;         // Điểm vô cực
      Point G_;         // Điểm sinh
      BigInt order_;  // Bậc của điểm sinh
    };

  }  // namespace crypto
}  // namespace shared_model

#endif  // IROHA_ELLIPTIC_CURVE_HPP