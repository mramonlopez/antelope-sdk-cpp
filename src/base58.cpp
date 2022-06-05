//
//  base58.cpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 5/6/22.
//

#include <eosclient/base58.hpp>
#include <random>
#include <string>
#include <algorithm>
#include <iomanip>
#include <iostream>

std::string Base58Encode(const std::vector<uint8_t>& data, CodecMapping mapping)
{
  std::vector<uint8_t> digits((data.size() * 138 / 100) + 1);
  size_t digitslen = 1;
  for (size_t i = 0; i < data.size(); i++)
  {
    uint32_t carry = static_cast<uint32_t>(data[i]);
    for (size_t j = 0; j < digitslen; j++)
    {
      carry = carry + static_cast<uint32_t>(digits[j] << 8);
      digits[j] = static_cast<uint8_t>(carry % 58);
      carry /= 58;
    }
    for (; carry; carry /= 58)
      digits[digitslen++] = static_cast<uint8_t>(carry % 58);
  }
  std::string result;
  for (size_t i = 0; i < (data.size() - 1) && !data[i]; i++)
    result.push_back(mapping.BaseMapping[0]);
  for (size_t i = 0; i < digitslen; i++)
    result.push_back(mapping.BaseMapping[digits[digitslen - 1 - i]]);
  return result;
}

std::vector<uint8_t> Base58Decode(const std::string& data, CodecMapping mapping)
{
  std::vector<uint8_t> result((data.size() * 138 / 100) + 1);
  size_t resultlen = 1;
  for (size_t i = 0; i < data.size(); i++)
  {
    uint32_t carry = static_cast<uint32_t>(mapping.AlphaMapping[data[i] & 0x7f]);
    for (size_t j = 0; j < resultlen; j++, carry >>= 8)
    {
      carry += static_cast<uint32_t>(result[j] * 58);
      result[j] = static_cast<uint8_t>(carry);
    }
    for (; carry; carry >>=8)
      result[resultlen++] = static_cast<uint8_t>(carry);
  }
  result.resize(resultlen);
  for (size_t i = 0; i < (data.size() - 1) && data[i] == mapping.BaseMapping[0]; i++)
    result.push_back(0);
  std::reverse(result.begin(), result.end());
  return result;
}
