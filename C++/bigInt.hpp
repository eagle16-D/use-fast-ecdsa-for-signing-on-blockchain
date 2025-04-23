#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace shared_model {
  namespace crypto {

    class BigInt {
     public:
      /*
       * Constructor mặc định
       * @return Đối tượng BigInt mới
       */
      BigInt();  // Default

      /*
       * Constructor từ chuỗi hex
       * @param hex Chuỗi hex
       * @return Đối tượng BigInt mới
       * @throws std::invalid_argument nếu chuỗi hex không hợp lệ
       * @note Chuỗi hex phải bắt đầu bằng "0x" hoặc "0X"
       * @note Chuỗi hex phải có độ dài chẵn
       * @note Các ký tự trong chuỗi hex phải là 0-9, a-f, A-F
       * @note Các byte trong chuỗi hex được lưu trữ theo thứ tự big-endian
       */
      explicit BigInt(const std::string &hex_str);

      /*
       * Constructor từ vector bytes
       * @param data Vector chứa các byte
       * @return Đối tượng BigInt mới
       */
      BigInt(const std::vector<uint8_t> &data);

      /*
       * Constructor sao chép
       * @param other Đối tượng BigInt khác
       * @return Đối tượng BigInt mới
       */
      BigInt(const BigInt &other) = default;

      /*
       * Constructor từ uint64_t
       * @param value Giá trị uint64_t
       * @return Đối tượng BigInt mới
       * @note Giá trị được lưu trữ theo thứ tự big-endian
       */
      explicit BigInt(uint64_t value);

      /*
       * getter cho dữ liệu
       * @return vector chứa các byte
       * @note Dữ liệu được lưu trữ theo thứ tự big-endian
       */
      const std::vector<uint8_t> &data() const {
        return data_;
      }

      std::vector<uint8_t> &data() {
        return data_;
      }

      /*
       * hàm from_hex() để tạo một đối tượng BigInt từ chuỗi hex
       * @param hex_str Chuỗi hex
       * @return Đối tượng BigInt tương ứng
       * @throws std::invalid_argument nếu chuỗi hex không hợp lệ
       * @note Chuỗi hex phải bắt đầu bằng "0x" hoặc "0X"
       * @note Chuỗi hex phải có độ dài chẵn
       * @note Các ký tự trong chuỗi hex phải là 0-9, a-f, A-F
       */
      static BigInt from_hex(const std::string &hex_str);

      /*
       * hàm from_bytes() để tạo một đối tượng BigInt từ vector bytes
       * @param bytes Vector chứa các byte
       * @return Đối tượng BigInt tương ứng
       * @throws std::invalid_argument nếu vector bytes rỗng
       */
      static BigInt from_bytes(const std::vector<uint8_t> &bytes);

      /*
       * hàm to_hex() để chuyển đổi đối tượng BigInt thành chuỗi hex
       * @return Chuỗi hex tương ứng
       * @note Chuỗi hex được định dạng theo chuẩn "0x" + các byte
       * @note Các byte được lưu trữ theo thứ tự big-endian
       */
      std::string to_hex() const;

      size_t size() const {
        return data_.size();
      }

      /*
       * hàm bit_length() để tính số bit của BigInt
       * @return Số bit của BigInt
       */
      size_t bit_length() const;

      /*
       * Kiểm tra xem bit thứ `pos` (0-based từ LSB) có bằng 1 không.
       * @param pos Vị trí bit cần kiểm tra
       * @return true nếu bit thứ `pos` là 1, false nếu không
       */
      bool test_bit(size_t pos) const;

      // Arithmetic operators
      BigInt operator+(const BigInt &rhs) const;
      BigInt operator-(const BigInt &rhs) const;
      BigInt operator*(const BigInt &rhs) const;
      BigInt operator/(const BigInt &rhs) const;
      BigInt operator%(const BigInt &rhs) const;

      BigInt &operator+=(const BigInt &rhs);
      BigInt &operator-=(const BigInt &rhs);
      BigInt &operator*=(const BigInt &rhs);
      BigInt &operator/=(const BigInt &rhs);
      BigInt &operator%=(const BigInt &rhs);

      // Comparison operators
      bool operator==(const BigInt &rhs) const;
      bool operator!=(const BigInt &rhs) const;
      bool operator<(const BigInt &rhs) const;
      bool operator<=(const BigInt &rhs) const;
      bool operator>(const BigInt &rhs) const;
      bool operator>=(const BigInt &rhs) const;

      // Debug
      friend std::ostream &operator<<(std::ostream &os, const BigInt &b) {
        return os << b.to_hex();
      }

     private:
      void trim();
      std::vector<uint8_t> data_;  // Big-endian
    };

  }  // namespace crypto
}  // namespace shared_model

#endif  // BIGINT_HPP
