#pragma once

#include <iostream>
#include <cstdint>
#include <string>

namespace BCD
{
	// Константы для специальных символов
	constexpr uint8_t WILDCARD_STAR = 0x0B;    // '*' в BCD
	constexpr uint8_t WILDCARD_QUESTION = 0x0C; // '?' в BCD  
	constexpr uint8_t EMPTY_SYMBOL = 0x0F;      // Пустой символ

	class BCD64
	{
	public:
		BCD64() = default;
		BCD64(std::string str);
		BCD64(uint64_t net);

		void FromUint64(uint64_t host64);
		uint64_t ToUint64() const;

		void FromNet(uint64_t net);
		uint64_t ToNet() const;

		void FromString(std::string str);
		std::string ToString() const;

		uint8_t GetLength() const;

		inline uint8_t operator[](int8_t position) const
		{
			return (Data >> (60 - (position << 2))) & 0x0F;
		}
		bool operator<(const BCD64& other) const;
		bool operator==(const BCD64& other) const;

		bool MatchByFilter(const BCD64& filter) const;
		bool MatchByDigit(const BCD64& digit) const;
	private:
		uint64_t Data = 0;
	};

	static_assert(sizeof(BCD64) == 8, "sizeof(BCD64) != 8!!!");
	static_assert(sizeof(BCD64) == sizeof(uint64_t), "sizeof(BCD64) != uint64_t!!!");
}