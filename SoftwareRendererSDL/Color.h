#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>

class Color
{
	public:
		uint8_t R, G, B, A;

		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);


		uint32_t ToUInt32() const;

		int toRGB() const;

		int ToInt16() const;
		uint32_t fromByte() const;

		Color operator + (const Color &c) const;
		Color operator - (const Color &c) const;
		Color operator * (float f) const;
};

#endif /* __COLOR_H__ */

