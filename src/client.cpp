#include "client.hpp"

#include <cctype>
#include <cstdlib>
#include <string>

client::client() : sent_(0), last_active_(time(NULL)) {}

void client::appendData(const char *data, size_t len) {
  buffer_.append(data, len);
  last_active_ = time(NULL);
}

bool client::isSendFinshed() const { return sent_ >= response_.size(); }

// 文字列を小文字にしたコピーを返す
static std::string toLower(const std::string &s) {
  std::string r(s);
  for (size_t i = 0; i < r.size(); ++i)
    r[i] = std::tolower(static_cast<unsigned char>(r[i]));
  return r;
}

// chunked の本文が最後まで届いているか（pos は本文の開始位置）
static bool isChunkedComplete(const std::string &buf, size_t pos) {
  while (true) {
    size_t line_end = buf.find("\r\n", pos);
    if (line_end == std::string::npos)
      return false; // 長さの行がまだ途中
    size_t size = std::strtoul(buf.c_str() + pos, NULL, 16);
    pos = line_end + 2; // 中身の先頭へ
    if (size == 0)      // 長さ 0 = 最後の chunk
      return buf.size() >= pos + 2 && buf.compare(pos, 2, "\r\n") == 0;
    if (buf.size() < pos + size + 2)
      return false;  // 中身がまだ途中
    pos += size + 2; // 中身と末尾の \r\n を飛ばす
  }
}

bool client::isComplete() const {
  // 1段階目：ヘッダーの終わり（空行）を探す
  std::string::size_type header_end = buffer_.find("\r\n\r\n");
  if (header_end == std::string::npos)
    return false;

  std::string headers = toLower(buffer_.substr(0, header_end));
  size_t body_start = header_end + 4;

  // 2段階目：本文の長さの決まり方で分ける
  if (headers.find("\r\ntransfer-encoding: chunked") != std::string::npos)
    return isChunkedComplete(buffer_, body_start);

  std::string::size_type pos = headers.find("\r\ncontent-length:");
  if (pos == std::string::npos)
    return true; // 本文なし

  size_t expected = std::strtoul(headers.c_str() + pos + 17, NULL, 10);
  return buffer_.size() - body_start >= expected;
}
