#include "bigInt.hpp"

#include <iostream>

#include "algorithm"

namespace shared_model {
  namespace crypto {
    /*
     * Constructor mặc định
     * @return Đối tượng BigInt mới
     */
    BigInt::BigInt() {
      data_.push_back(0);  // Khởi tạo với một byte 0
    }

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
    BigInt::BigInt(const std::string &hex) {
      // 1. Kiểm tra bắt đầu bằng "0x" hoặc "0X"
      if (hex.size() < 3 || hex[0] != '0' || (hex[1] != 'x' && hex[1] != 'X')) {
        throw std::invalid_argument("Hex string must start with '0x' or '0X'");
      }

      std::string hex_str = hex.substr(2);  // Bỏ "0x" ra

      // 2. Kiểm tra độ dài phải chẵn
      if (hex_str.empty() || hex_str.size() % 2 != 0) {
        throw std::invalid_argument(
            "Hex string after '0x' must be of even length");
      }

      // 3. Kiểm tra các ký tự trong chuỗi hex có hợp lệ không (0-9, a-f, A-F)
      for (char c : hex_str) {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
              || (c >= 'A' && c <= 'F'))) {
          throw std::invalid_argument("Invalid hex digit: "
                                      + std::string(1, c));
        }
      }

      // 4. Chuyển thành các byte
      data_.reserve(hex_str.size() / 2);
      for (size_t i = 0; i < hex_str.size(); i += 2) {
        std::string byte_str = hex_str.substr(i, 2);
        try {
          data_.push_back(
              static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16)));
        } catch (const std::exception &) {
          throw std::invalid_argument("Invalid hex digit: " + byte_str);
        }
      }

      trim();
    }

    /*
     * Constructor từ vector bytes
     * @param data Vector chứa các byte
     * @return Đối tượng BigInt mới
     */
    BigInt::BigInt(const std::vector<uint8_t> &data) : data_(data) {
      trim();  // Ensure no leading zeros
    }

    /*
     * Constructor từ uint64_t
     * @param value Giá trị uint64_t
     * @return Đối tượng BigInt mới
     * @note Giá trị được lưu trữ theo thứ tự big-endian
     */
    BigInt::BigInt(uint64_t value) {
      // Chuyển đổi uint64_t thành vector byte
      data_.resize(sizeof(value));
      for (size_t i = 0; i < sizeof(value); ++i) {
        data_[sizeof(value) - 1 - i] = static_cast<uint8_t>(value >> (i * 8));
      }
      trim();  // Ensure no leading zeros
    }

    /*
     * Hàm trim() để loại bỏ các byte 0 ở đầu dữ liệu
     * (nếu có) để đảm bảo rằng số nguyên không có
     * tiền tố không cần thiết.
     * @param data_ Dữ liệu BigInt
     */
    void BigInt::trim() {
      // Loại bỏ các byte 0 ở đầu
      while (data_.size() > 1 && data_[0] == 0) {
        data_.erase(data_.begin());
      }

      // Đảm bảo vector không rỗng
      if (data_.empty()) {
        data_.push_back(0);
      }
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
    BigInt BigInt::from_hex(const std::string &hex_str) {
      // 1. Kiểm tra bắt đầu bằng "0x" hoặc "0X"
      if (hex_str.size() < 3 || hex_str[0] != '0'
          || (hex_str[1] != 'x' && hex_str[1] != 'X')) {
        throw std::invalid_argument("Hex string must start with '0x' or '0X'");
      }

      // 2. Lấy phần chuỗi hex bỏ "0x" ra
      std::string hex = hex_str.substr(2);

      // 3. Kiểm tra độ dài của chuỗi hex phải chẵn
      if (hex.empty() || hex.size() % 2 != 0) {
        throw std::invalid_argument(
            "Hex string after '0x' must be of even length");
      }

      // 4. Kiểm tra các ký tự trong chuỗi có hợp lệ không
      for (char c : hex) {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
              || (c >= 'A' && c <= 'F'))) {
          throw std::invalid_argument("Invalid hex digit: "
                                      + std::string(1, c));
        }
      }

      // 5. Tạo đối tượng BigInt và chuyển chuỗi hex thành các byte
      BigInt result;
      result.data_.reserve(hex.size() / 2);
      for (size_t i = 0; i < hex.size(); i += 2) {
        std::string byte_str = hex.substr(i, 2);
        result.data_.push_back(
            static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16)));
      }

      // 6. Trim dữ liệu (nếu cần)
      result.trim();

      return result;
    }

    /*
     * hàm from_bytes() để tạo một đối tượng BigInt từ vector bytes
     * @param bytes Vector chứa các byte
     * @return Đối tượng BigInt tương ứng
     * @throws std::invalid_argument nếu vector bytes rỗng
     */
    BigInt BigInt::from_bytes(const std::vector<uint8_t> &bytes) {
      // 1. Kiểm tra vector bytes không rỗng
      if (bytes.empty()) {
        throw std::invalid_argument("Byte vector cannot be empty");
      }

      // 2. Tạo đối tượng BigInt và sao chép các byte vào data_
      BigInt result;
      result.data_ = bytes;  // Sao chép trực tiếp các byte vào data_

      // 3. Trim dữ liệu (nếu cần)
      result.trim();

      return result;
    }

    /*
     * hàm to_hex() để chuyển đổi đối tượng BigInt thành chuỗi hex
     * @return Chuỗi hex tương ứng
     * @note Chuỗi hex được định dạng theo chuẩn "0x" + các byte
     * @note Các byte được lưu trữ theo thứ tự big-endian
     */
    std::string BigInt::to_hex() const {
      std::ostringstream oss;
      oss << "0x";  // Thêm tiền tố
      for (auto byte : data_) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(byte);
      }
      return oss.str();
    }

    /*
     * Hàm cộng hai đối tượng BigInt
     * @param rhs Đối tượng BigInt bên phải
     * @return Đối tượng BigInt kết quả của phép cộng
     * @note Nếu một trong hai đối tượng là 0, trả về đối tượng còn lại
     */
    BigInt BigInt::operator+(const BigInt &rhs) const {
      // Đảm bảo dữ liệu của cả hai đối tượng BigInt là hợp lệ
      // nếu rhs = 0
      if (rhs.data_.empty()) {
        return *this;  // Nếu rhs là 0, trả về chính nó
      }
      if (data_.empty()) {
        return rhs;  // Nếu data_ là 0, trả về rhs
      }
      size_t max_size = std::max(data_.size(), rhs.data_.size());
      std::vector<uint8_t> temp_result;
      temp_result.reserve(
          max_size
          + 1);  // Dự trữ bộ nhớ cho phép cộng có thể tạo ra một byte dư

      uint8_t carry = 0;  // Biến carry để lưu trữ giá trị mang theo

      // Thực hiện phép cộng từng byte từ phải sang trái (big-endian)
      size_t i = data_.size();
      size_t j = rhs.data_.size();

      while (i > 0 || j > 0 || carry != 0) {
        uint8_t byte1 = (i > 0)
            ? data_[--i]
            : 0;  // Lấy byte từ data_ nếu còn, nếu không lấy 0
        uint8_t byte2 = (j > 0) ? rhs.data_[--j] : 0;  // Tương tự cho rhs

        // Tính tổng của byte1, byte2 và carry
        uint16_t sum = (uint16_t)byte1 + (uint16_t)byte2 + carry;

        // Cập nhật carry
        carry = sum >= 256 ? 1 : 0;

        // Lưu kết quả vào data_ của đối tượng result
        temp_result.push_back(sum & 0xFF);  // Lấy phần dư khi chia cho 256
      }

      std::reverse(temp_result.begin(), temp_result.end());

      BigInt result(temp_result);
      result.trim();
      // Trả về kết quả cộng
      return result;
    }

    /*
     * Hàm trừ hai đối tượng BigInt
     * @param rhs Đối tượng BigInt bên phải
     * @return Đối tượng BigInt kết quả của phép trừ
     * @note Nếu rhs = 0, trả về chính nó
     * @note Nếu *this < rhs, ném ngoại lệ std::invalid_argument
     */
    BigInt BigInt::operator-(const BigInt &rhs) const {
      // Đảm bảo dữ liệu của cả hai đối tượng BigInt là hợp lệ
      if (*this < rhs) {
        throw std::invalid_argument(
            "Cannot subtract a larger number from a smaller one");
      }

      // Nếu rhs = 0, trả về chính nó
      if (rhs.data_.empty()) {
        return *this;  // Nếu rhs là 0, trả về chính nó
      }

      std::vector<uint8_t> temp_result;
      temp_result.reserve(
          std::max(data_.size(), rhs.data_.size()));  // Dự trữ bộ nhớ

      uint8_t borrow = 0;  // Biến borrow để lưu trữ giá trị mượn

      // Thực hiện phép trừ từng byte từ phải sang trái (big-endian)
      size_t i = data_.size();
      size_t j = rhs.data_.size();

      while (i > 0 || j > 0 || borrow != 0) {
        uint8_t byte1 = (i > 0)
            ? data_[--i]
            : 0;  // Lấy byte từ data_ nếu còn, nếu không lấy 0
        uint8_t byte2 = (j > 0) ? rhs.data_[--j] : 0;  // Tương tự cho rhs

        // Tính hiệu của byte1 và byte2, cộng với borrow
        int16_t diff = byte1 - byte2 - borrow;

        // Nếu kết quả nhỏ hơn 0, cần mượn
        if (diff < 0) {
          diff += 256;
          borrow = 1;  // Cập nhật borrow
        } else {
          borrow = 0;  // Không cần mượn
        }

        // Lưu kết quả vào data_ của đối tượng result
        temp_result.push_back(diff & 0xFF);  // Lấy phần dư khi chia cho 256
      }
      std::reverse(temp_result.begin(), temp_result.end());
      BigInt result(temp_result);
      // Cắt bỏ các byte thừa không cần thiết (0 phía trước)
      result.trim();

      // Trả về kết quả trừ
      return result;
    }

    /*
     * Hàm nhân hai đối tượng BigInt
     * @param rhs Đối tượng BigInt bên phải
     * @return Đối tượng BigInt kết quả của phép nhân
     * @note Nếu một trong hai đối tượng là 0, trả về 0
     * @note Nếu một trong hai đối tượng là 1, trả về đối tượng còn lại
     */
    BigInt BigInt::operator*(const BigInt &rhs) const {
      // Đảm bảo dữ liệu của cả hai đối tượng BigInt là hợp lệ
      if (data_.empty() || rhs.data_.empty()) {
        return BigInt(0);  // Nếu một trong hai đối tượng là 0, trả về 0
      }
      if (rhs == BigInt(1)) {
        return *this;  // Nếu rhs là 1, trả về chính nó
      }
      if (*this == BigInt(1)) {
        return rhs;  // Nếu *this là 1, trả về rhs
      }
      // Khởi tạo đối tượng BigInt để lưu kết quả
      BigInt result;

      // Cấp phát bộ nhớ cho result, tối đa cần 2 lần kích thước của hai số
      result.data_.resize(data_.size() + rhs.data_.size(), 0);

      // Nhân từng byte của data_ với từng byte của rhs.data_
      for (size_t i = 0; i < data_.size(); ++i) {
        uint8_t byte1 = data_[data_.size() - 1 - i];  // Lấy byte từ data_
        uint8_t carry = 0;

        for (size_t j = 0; j < rhs.data_.size(); ++j) {
          uint8_t byte2 =
              rhs.data_[rhs.data_.size() - 1 - j];  // Lấy byte từ rhs

          // Tính tích của byte1 và byte2, cộng với carry (nếu có)
          uint16_t product = byte1 * byte2
              + result.data_[result.data_.size() - 1 - (i + j)] + carry;

          // Cập nhật carry và lưu kết quả vào data_
          result.data_[result.data_.size() - 1 - (i + j)] =
              static_cast<uint8_t>(product % 256);
          carry = product / 256;  // Cập nhật carry
        }

        // Nếu còn carry sau khi nhân với tất cả các byte của rhs, lưu carry vào
        // kết quả
        size_t carry_position =
            result.data_.size() - 1 - (i + rhs.data_.size());
        while (carry != 0) {
          uint16_t product = result.data_[carry_position] + carry;
          result.data_[carry_position] = static_cast<uint8_t>(product % 256);
          carry = product / 256;
          --carry_position;
        }
      }

      // Cắt bỏ các byte thừa không cần thiết (0 phía trước)
      result.trim();

      // Trả về kết quả nhân
      return result;
    }

    /*
     * hàm bit_length() để tính số bit của BigInt
     * @return Số bit của BigInt
     */
    size_t BigInt::bit_length() const {
      // Traverse from highest byte (data_[0]) down
      for (size_t i = 0; i < data_.size(); ++i) {
        uint8_t byte = data_[i];

        if (byte != 0) {
          int bits = 0;
          // Count bits in the highest non-zero byte
          while (byte) {
            byte >>= 1;
            ++bits;
          }
          return (data_.size() - i - 1) * 8 + bits;
        }
      }
      return 0;
    }

    /*
     * Kiểm tra xem bit thứ `pos` (0-based từ LSB) có bằng 1 không.
     * @param pos Vị trí bit cần kiểm tra
     * @return true nếu bit thứ `pos` là 1, false nếu không
     */
    bool BigInt::test_bit(size_t pos) const {
      // Calculate byte index from MSB
      size_t byte_idx = data_.size() - 1 - pos / 8;
      size_t bit_idx = pos % 8;
      return (data_[byte_idx] >> bit_idx) & 1;
    }

    BigInt BigInt::operator/(const BigInt &divisor) const {
      // 1. Kiểm tra chia cho 0
      if (divisor == BigInt(0)) {
        throw std::invalid_argument("Chia cho 0");
      }

      BigInt dividend = *this;
      // 2. Các trường hợp nhanh
      if (dividend < divisor)
        return BigInt(0);
      if (dividend == divisor)
        return BigInt(1);
      if (divisor == BigInt(1))
        return dividend;

      // 3. Khởi tạo quotient và remainder
      BigInt quotient(0);
      BigInt remainder(0);

      // 4. Tổng số bit
      size_t total_bits = dividend.bit_length();

      // 5. Thuật toán chia nhị phân
      for (int i = (int)total_bits - 1; i >= 0; --i) {
        // Dịch trái remainder (nhân đôi)

        remainder = remainder + remainder;

        // Thêm bit thứ i của dividend vào remainder
        if (dividend.test_bit(i)) {
          remainder = remainder + BigInt(1);
        }

        // Dịch trái quotient (nhân đôi)
        quotient = quotient + quotient;

        // Nếu remainder >= divisor, trừ divisor và đặt bit 1 cho quotient
        if (remainder >= divisor) {
          remainder = remainder - divisor;
          // Đặt bit thấp nhất của quotient
          quotient.data_[quotient.data_.size() - 1] |= 1;
        }
      }

      // 6. Loại bỏ byte 0 thừa
      quotient.trim();

      return quotient;
    }

    BigInt BigInt::operator%(const BigInt &divisor) const {
      if (divisor == BigInt(0)) {
        throw std::invalid_argument("Division by zero");
      }

      BigInt quotient = *this / divisor;
      BigInt product = quotient * divisor;
      BigInt remainder = *this - product;

      return remainder;
    }

    BigInt &BigInt::operator+=(const BigInt &rhs) {
      *this = *this + rhs;
      return *this;
    }
    BigInt &BigInt::operator-=(const BigInt &rhs) {
      *this = *this - rhs;
      return *this;
    }
    BigInt &BigInt::operator*=(const BigInt &rhs) {
      *this = *this * rhs;
      return *this;
    }
    BigInt &BigInt::operator/=(const BigInt &rhs) {
      *this = *this / rhs;
      return *this;
    }
    BigInt &BigInt::operator%=(const BigInt &rhs) {
      *this = *this % rhs;
      return *this;
    }

    // Comparison operators (basic example)
    bool BigInt::operator==(const BigInt &rhs) const {
      // Sau khi trim, cả hai vector không thể rỗng
      return data_ == rhs.data_;
    }

    bool BigInt::operator!=(const BigInt &rhs) const {
      return !(*this == rhs);
    }

    bool BigInt::operator<(const BigInt &rhs) const {
      // So sánh kích thước trước
      if (data_.size() != rhs.data_.size()) {
        return data_.size() < rhs.data_.size();
      }

      // So sánh từng byte từ trái sang phải
      for (size_t i = 0; i < data_.size(); i++) {
        if (data_[i] != rhs.data_[i]) {
          return data_[i] < rhs.data_[i];
        }
      }

      // Bằng nhau
      return false;
    }

    bool BigInt::operator<=(const BigInt &rhs) const {
      return *this < rhs || *this == rhs;
    }

    bool BigInt::operator>(const BigInt &rhs) const {
      return !(*this <= rhs);
    }

    bool BigInt::operator>=(const BigInt &rhs) const {
      return !(*this < rhs);
    }

  }  // namespace crypto
}  // namespace shared_model

// int main(int argc, char const *argv[]) {
//   // kiểm tra 1 số lẻ chia cho 2
//   using namespace shared_model::crypto;
//   BigInt a("0x09");
//   BigInt b("0x02");
//   BigInt c = a / b;
//   std::cout << "a: " << a.to_hex() << std::endl;
//   std::cout << "b: " << b.to_hex() << std::endl;
//   std::cout << "c: " << c.to_hex() << std::endl;
//   return 0;
// }
