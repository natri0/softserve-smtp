#ifndef BASE_64
#define BASE_64

#include <vector>

class Base64 {
public:
  static std::vector<unsigned char> Encode(const std::vector<unsigned char> &data);
  static std::vector<unsigned char> Decode(const std::vector<unsigned char> &encoded);
};

#endif