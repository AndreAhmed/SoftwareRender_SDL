#include "Color.h"
Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	R = r;
	G = g;
	B = b;
	A = a;
}

uint32_t Color::fromByte() const
{
	uint32_t color = (A << 24) | (R << 16) | (G << 8) | B;

	return  color;
}
uint32_t
Color::ToUInt32() const
{
	uint32_t r = (uint32_t)(R * 255.0f);
	uint32_t g = (uint32_t)(G * 255.0f);
	uint32_t b = (uint32_t)(B * 255.0f);
	uint32_t a = (uint32_t)(A * 255.0f);

	return (a << 24) | (r << 16) | (g << 8) | b;
}
int Color::toRGB() const {
	unsigned r = (int)(3 * (R < 0 ? 0 : this->R));
	unsigned g = (int)(15 * (G < 0 ? 0 : this->G));
	unsigned b = (int)(3 * (B < 0 ? 0 : this->B));
	return (r << 6) | (g << 2) | b;
}
Color
Color::operator + (const Color &c) const
{
	return Color(R + c.R, G + c.G, B + c.B, A + c.A);
}

Color
Color::operator - (const Color &c) const
{
	return Color(R - c.R, G - c.G, B - c.B, A - c.A);
}

Color
Color::operator * (float f) const
{
	return Color(R * f, G * f, B * f, A * f);
}
