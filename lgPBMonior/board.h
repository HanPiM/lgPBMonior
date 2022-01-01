#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include "CImg/CImg.h"

#include <string>
#include <vector>

class board
{
public:
	struct _rgb
	{
		unsigned char r, g, b;
		_rgb() :r(0), g(0), b(0) {}
		_rgb(unsigned char r, unsigned char g, unsigned char b) :
			r(r), g(g), b(b) {}
		bool operator==(_rgb c)const { return r == c.r && g == c.g && b == c.b; }
	};
	using color_table = std::vector<_rgb>;

	using img_t = cimg_library::CImg<unsigned char>;

};

int get_img_from_map(
	_In_ const board::color_table& ct, _In_ const std::string& str,
	_Out_ board::img_t& img
);
