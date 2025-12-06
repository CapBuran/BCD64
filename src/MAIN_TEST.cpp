#include <iostream>
#include <vector>
#include <set>

#include "BCD_SORM.h"

using namespace BCD;

struct TestCase {
    const char* str;
    const char* pattern;
    bool expected;
    const char* description;
};

const std::vector<TestCase> tests = {
    // === БАЗОВЫЕ ТЕСТЫ ===
    {"123", "123", true, "Точное совпадение"},
    {"123", "124", false, "Точное несовпадение"},
    {"", "", true, "Две пустые строки"},

    // === ТЕСТЫ НА '?' ===
    {"123", "1?3", true, "Один '?' в середине"},
    {"123", "1??", true, "Два '?' в конце"},
    {"123", "???", true, "Три '?' полное совпадение"},
    {"123", "????", false, "Слишком много '?'"},
    {"123", "?23", true, "'?' в начале"},
    {"1", "?", true, "Один '?' для одного символа"},
    {"", "?", false, "'?' для пустой строки"},

    // === ТЕСТЫ НА '*' ===
    {"123", "*", true, "Один '*' для любой строки"},
    {"", "*", true, "'*' для пустой строки"},
    {"123", "1*", true, "'*' в конце"},
    {"123", "*3", true, "'*' в начале"},
    {"123", "1*3", true, "'*' в середине"},
    {"123", "1*2", false, "'*' но неправильный конец"},
    {"123456", "1*6", true, "'*' для длинной строки"},

    // === КОМБИНАЦИИ '*' И '?' ===
    {"123", "*?", true, "'*?' - минимум один символ"},
    {"123", "?*", true, "'?*' - минимум один символ"},
    {"123", "1*?", true, "Комбинация '*' и '?' в конце"},
    {"123", "?*3", true, "Комбинация '?' и '*' в начале"},
    {"123", "1?*3", true, "Комбинация в середине"},
    {"123", "*?*", true, "Множественные '*' и '?'"},
    {"123", "?*?", true, "Чередование '?' и '*'"},

    // === МНОЖЕСТВЕННЫЕ '*' ===
    {"123456", "1**6", true, "Два подряд '*'"},
    {"123456", "*2*6", true, "Несколько '*'"},
    {"123456", "1*3*6", true, "Разделенные '*'"},
    {"123456", "**", true, "Только '*'"},
    {"123", "*1*3*", true, "'*' вокруг символов"},

    // === СЛОЖНЫЕ КОМБИНАЦИИ ===
    {"123456789", "1*5*9", true, "Сложный паттерн с двумя '*'"},
    {"123456789", "1?3*7?9", true, "Смешанный сложный паттерн"},
    {"123456789", "*2*4*6*8*", true, "Множественные '*'"},
    {"12345", "1*3*5*", true, "'*' в конце паттерна"},

    // === ГРАНИЧНЫЕ СЛУЧАИ ===
    {"9", "*9", true, "Короткая строка с '*'"},
    {"19", "*9", true, "Строка заканчивается на 9"},
    {"129", "*9", true, "Длиннее строка заканчивается на 9"},
    {"123456789", "*9", true, "Длинная строка заканчивается на 9"},
    {"123456780", "*9", false, "Не заканчивается на 9"},
    {"9", "?9", false, "'?' не может покрыть всю строку"},
    {"19", "?9", true, "'?' покрывает первый символ"},

    // === МАКСИМАЛЬНАЯ ДЛИНА ===
    {"12567890123456", "1*6", true, "Максимальная длина строки"},
    {"12567890123456", "1************6", true, "Много '*'"},
    {"12567890123456", "?2567890123456", true, "'?' в начале длинной строки"},

    // === СПЕЦИАЛЬНЫЕ КЕЙСЫ ===
    {"111", "1*1", true, "Повторяющиеся символы с '*'"},
    {"112233", "1*3", true, "Повторения с '*'"},
    {"123123", "*123", true, "Повторяющаяся подстрока"},
    {"121212", "1?1?1?", true, "Повторения с '?'"},

    // === НЕГАТИВНЫЕ ТЕСТЫ ===
    {"123", "1*4", false, "'*' но неправильный конец"},
    {"123", "1?4", false, "'?' но неправильный символ"},
    {"123", "12", false, "Слишком короткий паттерн"},
    {"12", "123", false, "Слишком длинный паттерн"},
    {"", "1*", false, "Пустая строка не подходит под не-* паттерн"},
    {"123", "*4", false, "Не заканчивается на нужный символ"},
    {"123", "1*2*4", false, "Множественные '*' но несовпадение"},

    // === ЭКСТРЕМАЛЬНЫЕ КЕЙСЫ ===
    {"99999999999999", "9*9", true, "Все символы одинаковые"},
    {"123456789", "1*9", true, "Очень широкий '*'"},
    {"1", "1*", true, "'*' после одного символа"},
    {"1", "*1", true, "'*' перед одним символом"},
    {"1234", "1*3*", true, "'*' на границе конца строки"},

    // === ТЕСТЫ ПРОИЗВОДИТЕЛЬНОСТИ ===
    {"12345678903456", "1************6", true, "Много '*' в середине"},
    {"12345678901456", "**************", true, "Только '*'"},
    {"12345678901234", "1?3?5?7?9?1?3?", true, "Много '?'"},

    // === ТЕСТЫ ПРОИЗВОДИТЕЛЬНОСТИ ===
    {"12567890123456", "*765*", false, "*765*"},
    {"9876543210", "*765*", true, "*765*"},
    {"97653210123456", "*765*", true, "*765*"},

    {"98543210123456", "*432*12345*", true, "*432*12345*"},
    {"87432101234567", "*432*12345*", true, "*432*12345*"},
    {"98543210123456", "*432*12345?", true, "*432*12345?"},
    {"98543210123456", "*432*12345", false, "*432*12345"},

    {"98543210123456", "8432*12345*", false, "8432*12345*"},
    {"87432101234567", "*432*12345*", true, "*432*12345*"},
    {"98543210123456", "*432*12345?", true, "*432*12345?"},
    {"98543210123456", "*432*12345", false, "*432*12345"},

    { "999999999999992", "9*92", true, "Все символы одинаковые" },

    { "999999999999992", "9*9?2", true, "Все символы одинаковые" },
    { "9999999999999912", "9*9?2", true, "Все символы одинаковые" },
    { "9999999999999912", "9999999999999912", true, "Все символы одинаковые" },

    { "1111111111111111", "****************", true, "Все символы одинаковые" },
    { "1111111111111111", "????????????????", true, "Все символы одинаковые" },

};


// Расширенное тестирование
void comprehensive_bcd_test3() {

    std::cout << "COMPREHENSIVE BCD MATCH TESTING\n";
    std::cout << "================================\n\n";

    int passed = 0;
    int failed = 0;

    for (size_t i = 0; i < tests.size(); i++) {
        const auto& test = tests[i];
        BCD64 bcd(test.str), filter(test.pattern);

        const bool result = bcd.MatchByFilter(filter);
        const bool test_passed = (result == test.expected);

        std::cout << "Test " << (i + 1) << ": " << (test_passed ? "PASS" : "FAIL") << "\n";
        std::cout << "  Desc: " << test.description << "\n";
        std::cout << "  Input: \"" << test.str << "\" ~ \"" << test.pattern << "\"\n";
        std::cout << "  BCD::ToUint64: " << bcd.ToUint64();
        std::cout << "  Expected: " << (test.expected ? "true" : "false")
            << ", Got: " << (result ? "true" : "false") << "\n";

        uint64_t OK = 0;
        uint64_t BCD_64 = 0;
        if (std::string(test.str) != "")
        {
            OK = std::strtoull(test.str, NULL, 0);
            BCD_64 = bcd.ToUint64();
        }

        bool NET_OK = true;
        {
          uint64_t NET = bcd.ToNet();
          BCD64 d3(NET);
          NET_OK = d3.ToNet() == NET;
        }

        bool LENGTCH_OK = true;
        {
          std::string s = test.str;
          LENGTCH_OK = s.length() == bcd.GetLength();
          s = test.pattern;
          LENGTCH_OK = LENGTCH_OK && s.length() == filter.GetLength();
        }

        if (!test_passed || OK != BCD_64 || !NET_OK || !LENGTCH_OK) {
            bool result2 = bcd.MatchByFilter(filter);

            std::string s = test.str;
            LENGTCH_OK = s.length() == bcd.GetLength();

            s = test.pattern;
            LENGTCH_OK = LENGTCH_OK && s.length() == filter.GetLength();

            BCD_64 = bcd.ToUint64();

            std::cout << "  >>> TEST FAILED! <<<\n";
            failed++;
        }
        else {
            passed++;
        }
        std::cout << std::endl;
    }

    std::cout << "================================\n";
    std::cout << "FINAL RESULTS: " << passed << "/" << (passed + failed)
        << " tests passed (" << (passed * 100.0 / (passed + failed)) << "%)\n";

    if (failed == 0) {
        std::cout << " ALL TESTS PASSED! Your function is production ready! \n";
    }
    else {
        std::cout << "  " << failed << " tests failed. Needs investigation.\n";
    }
}


int main()
{
  comprehensive_bcd_test3();

  BCD64 sss("123"), sss2("1234");

  std::string sss_str = sss.ToString();


  union dd
  {
      uint64_t d64;
      uint8_t d8[8];
  };
    
  dd ddd; ddd.d64 = 0;

  ddd.d8[0] = 0x81;
  ddd.d8[1] = 0x23;
  ddd.d8[2] = 0x00;
  ddd.d8[3] = 0x00;
  ddd.d8[4] = 0x00;
  ddd.d8[5] = 0x00;
  ddd.d8[6] = 0x04;
  ddd.d8[7] = 0x56;


  sss.FromNet(ddd.d64);

  sss.FromString("1234567890");


  bool IsOkNet = ddd.d64 == sss.ToNet();
  uint64_t Net = sss.ToNet();
  uint64_t d = sss.ToUint64();

  sss_str = sss.ToString();



  BCD64 s1_1("123"), s1_2("123");
  bool d1 = s1_1 < s1_2;//false

  BCD64 s2_1("123"), s2_2("1234");
  bool d2 = s2_1 < s2_2;//true

  BCD64 s3_1("1233"), s3_2("1234");
  bool d3 = s3_1 < s3_2;//true

  BCD64 s4_1("1"), s4_2("*456");
  bool d4 = s4_1 < s4_2;//true

  BCD64 s5_1(""), s5_2("*");
  bool d5 = s5_1 < s5_2;//true

  BCD64 s6_1("50"), s6_2("60");
  bool d6 = s6_1 < s6_2;//true

  BCD64 s7_1("9"), s7_2("");
  bool d7 = s7_1 < s7_2;

  BCD64 s8_1(""), s8_2("0");
  bool d8 = s8_1 < s8_2;

  BCD64 s9_1("123"), s9_2("1234");
  bool d9 = s9_1 < s9_2;

  BCD64 s0_1("123"), s0_2("1234");
  bool d0 = s0_1 < s0_2;

  bool r1 = s1_1 == s1_2;

  std::set<BCD64> BCDs;
  BCDs.insert(s1_1);
  BCDs.insert(s2_1);
  BCDs.insert(s3_1);
  BCDs.insert(s4_1);
  BCDs.insert(s5_1);
  BCDs.insert(s6_1);
  BCDs.insert(s7_1);
  BCDs.insert(s8_1);
  BCDs.insert(s9_1);
  BCDs.insert(s0_1);

  BCDs.insert(s1_2);
  BCDs.insert(s2_2);
  BCDs.insert(s3_2);
  BCDs.insert(s4_2);
  BCDs.insert(s5_2);
  BCDs.insert(s6_2);
  BCDs.insert(s7_2);
  BCDs.insert(s8_2);
  BCDs.insert(s9_2);
  BCDs.insert(s0_2);

  std::cout << "Hello CMake." << std::endl;
	return 0;
}
