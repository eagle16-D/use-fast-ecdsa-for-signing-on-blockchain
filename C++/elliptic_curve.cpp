#include "elliptic_curve.hpp"
#include "point.hpp"
#include "utils.hpp"
#include <stdexcept>

namespace shared_model {
  namespace crypto {

    EllipticCurve::EllipticCurve(BigInt &p, BigInt a, BigInt b) {
      // p phải là số nguyên tố
      if (!isPrime(p)) {
        throw std::invalid_argument("p is not prime");
      }
      p_ = p;
      a_ = a;
      b_ = b;
      O_ = Point();
      G_ = Point();
      order_ = BigInt() ;
    }


    bool EllipticCurve::isOnCurve(const Point &P) const {
      if (P.isInfinity()) {
        return true;
      }
      BigInt x = P.x();
      BigInt y = P.y();
      BigInt left = (y * y) % p_;
      BigInt right = (x * x * x + a_ * x + b_) % p_;
      return left == right;
    }

    BigInt EllipticCurve::inverseMod(BigInt k) const {
      if(k == BigInt(0)) {
        throw std::invalid_argument("k is 0");
      }
      return pow_mod(k, p_ - BigInt(2), p_);
    }

    bool EllipticCurve::coincide(const Point &P, const Point &Q) const {
      if (P.isInfinity() || Q.isInfinity()) {
        return false;
      }
      return P.x() == Q.x() && P.y() == Q.y();
    }

    Point EllipticCurve::add(const Point &P, const Point &Q) const {
      if (P.isInfinity()) {
        return Q;
      }
      if (Q.isInfinity()) {
        return P;
      }

      BigInt x1 = P.x();
      BigInt y1 = P.y();
      BigInt x2 = Q.x();
      BigInt y2 = Q.y();

      if(x1 == x2 && y1 != y2) {
        return O_;
      }
      BigInt s;
      if (coincide(P, Q)) {
        s = (BigInt(3) * x1 * x1 + a_) * inverseMod(BigInt(2) * y1) % p_;
      }
      else {
        s = (y2 - y1) * inverseMod(x2 - x1) % p_;
      }
      BigInt x3 = (s * s - x1 - x2) % p_;
      BigInt y3 = (s * (x1 - x3) - y1) % p_;

      return Point(x3, y3);
    }

    Point EllipticCurve::multiply(const Point &P, BigInt k) const {
      if (P.isInfinity()) {
        return O_;
      }
      if(k < BigInt(0)) {
        throw std::invalid_argument("k < 0");
      }
      if(order_ != BigInt(0)){
        k = k % order_;
      }
      Point R = O_;
      Point Q = P;
      while (k > BigInt(0)) {
        if (k % BigInt(2) == BigInt(1)) {
          R = add(R, Q);
        }
        Q = add(Q, Q);
        k = k / BigInt(2);
      }
      return R;
    }

    Point EllipticCurve::findGenerator() {
      // Tìm điểm sinh G trên đường cong elliptic
      // Sử dụng thuật toán Brute Force để tìm điểm sinh
      for (BigInt x = BigInt(0); x < p_; x = x + BigInt(1)) {
        for (BigInt y = BigInt(0); y < p_; y = y + BigInt(1)) {
          Point P(x, y);
          if (isOnCurve(P)) {
            BigInt order = findOrder(P);
            if(isPrime(order)) {
              // Nếu order là số nguyên tố, thì P là điểm sinh
              G_ = P;
              order_ = order;
              return P;
            }
          }
        }
      }
      throw std::runtime_error("No generator point found");
    }
    void EllipticCurve::setGenerator(const Point &G) {
      if (!isOnCurve(G)) {
        throw std::invalid_argument("G is not on the curve");
      }
      G_ = G;
    }

    bool EllipticCurve::isGenerator(const Point &P) const {
      return findOrder(P) != BigInt(0);
    }

    BigInt EllipticCurve::findOrder(const Point &P) const {
      if (!isOnCurve(P)) {
        throw std::invalid_argument("P is not on the curve");
      }
      BigInt order = BigInt(1);
      Point Q = P;
      while (!Q.isInfinity()) {
        Q = add(Q, P);
        order = order + BigInt(1);
        if (order > p_ + BigInt(1)) {
          throw std::runtime_error("Exceeded iteration limit when computing order");
        }
      }
      return order;
    }

    Point EllipticCurve::sumPoints(const std::vector<Point> &points) const {
      Point result = O_;
      for (const auto &point : points) {
        result = add(result, point);
      }
      return result;
    }


  }  // namespace crypto
}  // namespace shared_model