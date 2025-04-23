// point.hpp
#ifndef POINT_HPP
#define POINT_HPP

#include <cstdint>
#include <iostream>
#include "bigInt.hpp"

namespace shared_model {
namespace crypto {

class Point {
 public:
  // Constructors
  /*
    * Constructor cho điểm vô cực, điểm mặc định
    * @note Điểm vô cực được biểu diễn bằng (0, 0)
    * @note is_infinity_ được đặt thành true
    */  

  Point() : x_(BigInt(0)), y_(BigInt(0)), is_infinity_(true) {}

  /*
    * Constructor cho điểm trên đường cong elliptic
    * @param x Hoành độ
    * @param y Tung độ
    * @note is_infinity_ được đặt thành false
    */
  Point(BigInt x, BigInt y) : x_(x), y_(y), is_infinity_(false) {}
  Point(const Point &other) = default;

  BigInt x() const {
    if (is_infinity_) {
      throw std::logic_error("Cannot access x() of point at infinity");
    }
    return x_;
  }

  BigInt y() const {
    if (is_infinity_) {
      throw std::logic_error("Cannot access y() of point at infinity");
    }
    return y_;
  }
  bool isInfinity() const {
    return is_infinity_;
  }

  /*
    * Hàm thiết lập điểm thuộc tính vô cực của điểm
    * @param is_infinity true nếu là điểm vô cực, false nếu không
    * @note Nếu is_infinity là true, x_ và y_ sẽ được đặt thành 0
    * @note Nếu is_infinity là false, x_ và y_ sẽ được giữ nguyên
  */
  void setInfinity(bool is_infinity) {
    is_infinity_ = is_infinity;
    if (is_infinity) {
      x_ = BigInt(0);
      y_ = BigInt(0);
    }
  }

 private:
  BigInt x_;
  BigInt y_;
  bool is_infinity_;
};

}  // namespace crypto
}  // namespace shared_model

#endif
