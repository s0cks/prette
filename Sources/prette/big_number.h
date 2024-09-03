#ifndef PRT_BIG_NUMBER_H
#define PRT_BIG_NUMBER_H

#include <string>
#include <cstdint>
#include <iomanip>

#include "prette/platform.h"

namespace prt {
  class BigNumber {
  protected:
    BigNumber() = default;
  public:
    virtual ~BigNumber() = default;
    virtual const uword* data() const = 0;
    virtual uword size() const = 0;
    virtual void clear() = 0;

    const uword* const_begin() const {
      return data();
    }

    const uword* const_end() const {
      return data() + size();
    }
  };

  template<const uword NumberOfBits>
  class BigNumberTemplate : public BigNumber {
  public:
    static constexpr const auto kSizeInBits = NumberOfBits;
    static constexpr const auto kSizeInBytes = kSizeInBits / kBitsPerByte;
    static constexpr const auto kSizeInWords = kSizeInBits / kBitsPerWord;
  protected:
    uword data_[kSizeInWords];

    constexpr BigNumberTemplate():
      BigNumber(),
      data_() {
      memset(&data_[0], 0, sizeof(data_));
    }
    constexpr BigNumberTemplate(const uint8_t* bytes, const uword num_bytes):
      BigNumber(),
      data_() {
      CopyFrom(bytes, num_bytes);
    }

    inline void
    CopyFrom(const uint8_t* bytes, const uword num_bytes) {
      memset(&data_[0], 0, kSizeInBytes);
      const auto total_bytes = std::min(num_bytes, kSizeInBytes);
      memcpy(&data_[0], &bytes[0], total_bytes);
    }

    inline void
    CopyFrom(const BigNumberTemplate<NumberOfBits>& rhs) {
      return CopyFrom((const uint8_t*) rhs.data(), rhs.size());
    }
  public:
    ~BigNumberTemplate() override = default;

    const uword* data() const override {
      return data_;
    }

    uword size() const override {
      return kSizeInWords;
    }

    void clear() override {
      memset(&data_[0], 0, sizeof(data_));
    }

    virtual std::string ToHexString() const {
      std::stringstream ss;
      ss << std::hex;
      for(auto i = 0; i < size(); ++i )
          ss << data_[i];
      return ss.str();
    }
    
    virtual std::string ToBinaryString() const {
      std::stringstream ss;
      std::bitset<kSizeInBits> bits;
      for(auto idx = 0; idx < size(); idx++) {
        bits <<= kBitsPerWord;
        bits |= std::bitset<kSizeInBits>(data_[idx]);
      }
      ss << bits;
      return ss.str();
    }
  };
}

#endif //PRT_BIG_NUMBER_H