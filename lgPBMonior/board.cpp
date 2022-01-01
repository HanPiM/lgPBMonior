#include <sstream>
#include "board.h"

int get_img_from_map(
	_In_ const board::color_table& ct, _In_ const std::string& str,
	_Out_ board::img_t& img
)
{
	std::string x;
	std::istringstream iss(str);
	std::vector<int>tmp, data;

	int w = 0, h = 0;

	while (iss >> x)
	{
		for (auto ch : x)
			tmp.push_back(isdigit(ch) ? ch - '0' : ch - 'a' + 10);
		h++;
	}
	w = x.size();

	///* Ðý×ª */
	int maxx = std::max(w, h);
	//int minn = std::min(w, h);
	int oldw = w, oldh = h;
	std::swap(w, h);

	//tmp.resize(maxx * maxx + 1);
	//data.resize(tmp.size());

	//for (int i = 0; i < maxx; ++i)
	//{
	//	for (int j = 0; j < maxx; ++j)
	//	{
	//		if (i >= minn && j >= minn)continue;
	//		data[j * w + i] = tmp[i * oldw + j];
	//	}
	//}

	if (img.width() < w || img.height() < h || img.depth() != 1)
	{
		img = board::img_t(w, h, 1, 3);
	}

	//std::vector<unsigned char>bits;
	unsigned char* bits = img.data();
	int offest = w * h, pos;
	board::_rgb c;
	//bits.resize(offest * 3);
	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			pos = j * w + i;
			c = ct[tmp[i * oldw + j]];
			//c = board::_rgb(0, 1, 0);
			bits[pos] = c.r;
			bits[pos + offest] = c.g;
			bits[pos + offest * 2] = c.b;
		}
	}
	//img = board::img_t(bits.data(), w, h, 1, 3);
	return 0;
}