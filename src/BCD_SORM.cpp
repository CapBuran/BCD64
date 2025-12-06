// BCD_SORM.cpp: определяет точку входа для приложения.
//

#include "BCD_SORM.h"
namespace BCD
{
  BCD64::BCD64(std::string str)
  {
    FromString(str);
  }

  BCD64::BCD64(uint64_t net)
  {
    FromNet(net);
  }

  void BCD64::FromUint64(uint64_t host64)
  {
    FromString(std::to_string(host64));
  }

  uint64_t BCD64::ToUint64() const
  {
    uint64_t Result = 0;
    for (uint8_t i = 0; i < 16; i++)
    {
      const uint8_t d = (*this)[i];
      switch (d)
      {
      case WILDCARD_STAR:
        i = 64;
        Result = 0;
        break;
      case WILDCARD_QUESTION:
        i = 64;
        Result = 0;
        break;
      case EMPTY_SYMBOL:
        i = 64;
        break;
      default:
        Result = Result * 10 + d;
        break;
      }
    }
    return Result;
  }

  uint64_t BCD64::ToNet() const
  {
    uint64_t Net = Data >> 4;
    uint8_t* swaps = reinterpret_cast<uint8_t*>(&Net);
    std::swap(swaps[0], swaps[7]), std::swap(swaps[1], swaps[6]),
      std::swap(swaps[2], swaps[5]), std::swap(swaps[3], swaps[4]);
    swaps[0] |= (swaps[0] >> 4) | 0x80;
    return Net;
  }

  void BCD64::FromNet(uint64_t net)
  {
    uint8_t* swaps = reinterpret_cast<uint8_t*>(&net);
    std::swap(swaps[0], swaps[7]), std::swap(swaps[1], swaps[6]),
      std::swap(swaps[2], swaps[5]), std::swap(swaps[3], swaps[4]);
    Data = (net << 4) | 0x0f;
  }

  uint8_t BCD64::GetLength() const
  {
    for (uint8_t i = 0; i < 16; i++) {
      if ((*this)[i] == EMPTY_SYMBOL)
      {
        return i;
      }
    }
    return 16;
  }

  bool BCD64::operator<(const BCD64& other) const
  {
    uint8_t str_nibble = 0;
    uint8_t other_nibble = 0;
    for (uint8_t i = 0; i < 16; i++)
    {
      str_nibble = (*this)[i];
      other_nibble = other[i];

      if (str_nibble == other_nibble)
        continue;

      if (str_nibble == 0x0f && other_nibble != 0x0f)
        return true;

      if (str_nibble != 0x0f && other_nibble == 0x0f)
        return false;

      break;
    }

    return str_nibble < other_nibble;
  }

  bool BCD64::operator==(const BCD64& other) const
  {
    return Data == other.Data;
  }

  bool BCD64::MatchByDigit(const BCD64& digit) const
  {
    BCD64 filter(*this);
    return digit.MatchByFilter(filter);
  }

  bool BCD64::MatchByFilter(const BCD64& filter) const
  {
    const char* str_bcd_2 = reinterpret_cast<const char*>(&Data);
    const char* pattern_bcd_2 = reinterpret_cast<const char*>(&filter.Data);

    int8_t str_pos = 0;
    int8_t pattern_pos = 0;
    int8_t str_backup_pos = -1;
    int8_t pattern_backup_pos = -1;

    while (str_pos < 16) {
      uint8_t str_nibble = (*this)[str_pos];
      uint8_t pattern_nibble = filter[pattern_pos];

      // Если достигли конца шаблона
      if (pattern_pos >= 16 || pattern_nibble == EMPTY_SYMBOL) {
        // Если и строка закончилась - совпадение
        if (str_pos >= 16 || str_nibble == EMPTY_SYMBOL) {
          return true;
        }
        // Если есть backup позиция - backtracking
        if (pattern_backup_pos != -1) {
          pattern_pos = pattern_backup_pos;
          str_pos = ++str_backup_pos;
          continue;
        }
        return false;
      }

      // Обработка wildcards
      if (pattern_nibble == WILDCARD_STAR) {
        // Пропускаем последовательные '*'
        while (pattern_pos < 16 && filter[pattern_pos] == WILDCARD_STAR) {
          pattern_pos++;
        }

        // Если после '*' конец шаблона - любая строка подходит
        if (pattern_pos >= 16 || filter[pattern_pos] == EMPTY_SYMBOL) {
          return true;
        }

        // Сохраняем позицию для backtracking
        str_backup_pos = str_pos;
        pattern_backup_pos = pattern_pos;
        continue;
      }

      // Проверка совпадения символов
      bool match_found = false;

      if (pattern_nibble == WILDCARD_QUESTION) {
        // '?' совпадает с любым символом, кроме конца строки
        if (str_pos < 16 && str_nibble != EMPTY_SYMBOL) {
          match_found = true;
        }
      }
      else {
        // Обычное сравнение BCD символов
        if (str_pos < 16 && str_nibble == pattern_nibble) {
          match_found = true;
        }
      }

      if (match_found) {
        // Совпадение - двигаемся дальше
        str_pos++;
        pattern_pos++;
      }
      else {
        // Несовпадение - пробуем backtracking если есть '*'
        if (pattern_backup_pos != -1) {
          pattern_pos = pattern_backup_pos;
          str_pos = ++str_backup_pos;
        }
        else {
          return false;
        }
      }
    }

    // Пропускаем оставшиеся '*' в шаблоне
    while (pattern_pos < 16) {
      uint8_t pattern_nibble = filter[pattern_pos];
      if (pattern_nibble != WILDCARD_STAR && pattern_nibble != EMPTY_SYMBOL) {
        break;
      }
      pattern_pos++;
    }

    // Проверяем, что достигли конца шаблона
    return pattern_pos >= 16 || filter[pattern_pos] == EMPTY_SYMBOL;
  }

  void BCD64::FromString(std::string str)
  {
    uint64_t result = 0;
    size_t length = str.length();

    //// Сначала считаем длину
    //while (str[length] != '\0' && length < 16) {
    //  length++;
    //}

    // Заполняем слева, выравнивая по старшим nibble
    for (int i = 0; i < 16; i++) {
      uint8_t nibble;
      if (i < length) {
        if (str[i] == '*') {
          nibble = WILDCARD_STAR;
        }
        else if (str[i] == '?') {
          nibble = WILDCARD_QUESTION;
        }
        else if (str[i] >= '0' && str[i] <= '9') {
          nibble = str[i] - '0';
        }
        else {
          nibble = EMPTY_SYMBOL;
        }
      }
      else {
        nibble = EMPTY_SYMBOL;
      }
      result = (result << 4) | nibble;
    }

    Data = result;
  }

  std::string BCD64::ToString() const
  {
    std::string Result;
    Result.reserve(8);
    for (uint8_t i = 0; i < 16; i++)
    {
      const uint8_t d = (*this)[i];
      switch (d)
      {
      case WILDCARD_STAR:
        Result.push_back('*');
        break;
      case WILDCARD_QUESTION:
        Result.push_back('?');
        break;
      case EMPTY_SYMBOL:
        i = 16;
        break;
      default:
        Result.push_back(static_cast<char>(d + '0'));
        break;
      }
    }
    return Result;
  }
}
