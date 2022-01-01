// lgPBMonior.cpp: 定义应用程序的入口点。
//

#include <string.h>
#include <algorithm>
#include <vector>
#include <memory>

#include <stdio.h>

#include "lgPBMonior.h"

#include "debug.h"

#include "board.h"

#define _ENSURE(expr) __ed=ENSURE(expr)
#define _CHECK_ED() if(!__ed.what().empty())return false

typedef std::function<void(const std::string&, void*)> run_program_callback_func;
#if _WIN32
#include <Windows.h>
bool run_program(
	const std::string& cmdline, size_t maxlen,
	run_program_callback_func func, void* userdata
){
	if (cmdline.empty())return false;

	BOOL bError = FALSE;
	errno_t ErrorCode = 0;

	HANDLE pip_read = NULL;
	HANDLE pip_write = NULL;

	SECURITY_ATTRIBUTES sa = { 0 };

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	//std::string recv_data;
	DWORD recv_size = 0;
	std::string szOutputLine;

	std::string cmd = cmdline;

	ON_SCOPE_EXIT([&](){
		if (NULL != pi.hProcess)CloseHandle(pi.hProcess);
		if (NULL != pi.hThread)CloseHandle(pi.hThread);
		if (NULL != pip_read)CloseHandle(pip_read);
		if (NULL != pip_write)CloseHandle(pip_write);
	});

	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	_ENSURE(CreatePipe(&pip_read, &pip_write, &sa, 0))(&sa); _CHECK_ED();
	_ENSURE(pip_read != NULL && pip_write != NULL)(pip_read)(pip_write);
	_CHECK_ED();

	si.cb = sizeof(si);
	si.hStdOutput = pip_write;
	si.hStdError = pip_write;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	_ENSURE(CreateProcessA(NULL, (char*)cmd.data(),
		NULL, NULL, TRUE, CREATE_NO_WINDOW,
		NULL, NULL, &si, &pi
	))(cmd.data())(&si)(&pi);
	_CHECK_ED();

	if (NULL != pip_write)
	{
		CloseHandle(pip_write);
		pip_write = NULL;
	}

	//recv_data.resize(maxlen);
	//char recv_data[5000];
	std::vector<char>recv_data;
	recv_data.resize(maxlen + 1);

	while (ReadFile(
		pip_read, (char*)recv_data.data(),
		DWORD(maxlen), &recv_size, NULL)
	){
		recv_data[recv_size] = '\0';
		func(recv_data.data(), userdata);
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	bError = TRUE;

	if (NULL != pi.hProcess)CloseHandle(pi.hProcess), pi.hProcess = NULL;
	if (NULL != pi.hThread)CloseHandle(pi.hThread), pi.hThread = NULL;

	CLOSE_SCOPE_EXIT_FUNC();

	return true;
}
#else
#include <unistd.h>
bool run_program(
	const std::string& cmdline, size_t maxlen,
	run_program_callback_func func, void* userdata
) {
	if (cmdline.empty())return false;
	ON_SCOPE_EXIT([]() {

	});
	std::string recv_data;
	recv_data.resize(maxlen);
	FILE* fp = popen(cmdline.c_str(), "r");
	_ENSURE(fp != NULL); _CHECK_ED();
	while (fread((void*)recv_data.data(), sizeof(char), maxlen, fp))
	{
		func(recv_data, userdata);
	}
	return true;
}
#endif

using _rgb = board::_rgb;

std::vector<_rgb>ct =
{
	_rgb(0, 0, 0),
	_rgb(255, 255, 255),
	_rgb(170, 170, 170),
	_rgb(85, 85, 85),
	_rgb(254, 211, 199),
	_rgb(255, 196, 206),
	_rgb(250, 172, 142),
	_rgb(255, 139, 131),
	_rgb(244, 67, 54),
	_rgb(233, 30, 99),
	_rgb(226, 102, 158),
	_rgb(156, 39, 176),
	_rgb(103, 58, 183),
	_rgb(63, 81, 181),
	_rgb(0, 70, 112),
	_rgb(5, 113, 151),
	_rgb(33, 150, 243),
	_rgb(0, 188, 212),
	_rgb(59, 229, 219),
	_rgb(151, 253, 220),
	_rgb(22, 115, 0),
	_rgb(55, 169, 60),
	_rgb(137, 230, 66),
	_rgb(215, 255, 7),
	_rgb(255, 246, 209),
	_rgb(248, 203, 140),
	_rgb(255, 235, 59),
	_rgb(255, 193, 7),
	_rgb(255, 152, 0),
	_rgb(255, 87, 34),
	_rgb(184, 63, 39),
	_rgb(121, 85, 72)

};

int get_change_count(const std::string& a, const std::string& b)
{
	int cnt = 0;
	int minn = std::min(a.size(), b.size());
	for (int i = 0; i < minn; ++i)
	{
		if (a[i] != b[i])++cnt;
	}
	if (a.size() > minn)cnt += a.size() - minn;
	if (b.size() > minn)cnt += b.size() - minn;
	return cnt;
}

int main()
{

	constexpr int PBW = 1000, PBH = 600;

	constexpr int SW = 390;


	std::string m, lastm;
	m.reserve(PBH * PBW + PBW);
	lastm.reserve(PBH * PBW + PBW);
	//printf("%d\n%s\n", m.size(),m.c_str());
	board::img_t img1(PBW, PBH, 1, 3, 0);
	board::img_t scrolling_img(SW, 200, 1, 3, 0);
	board::img_t showimg(1400, PBH, 1, 3);
	cimg_library::CImgDisplay disp(showimg, "LuoguWinterPaintBoard-Monior");

	unsigned char col_textfore[] = { 40,40,40 };
	unsigned char col_back[] = { 205,205,205 };
	unsigned char col_[] = { 205,205,205 };

	auto cimg_color = [](unsigned char r, unsigned char g, unsigned char b)
	{
		unsigned char* res = new unsigned char[3];
		res[0] = r, res[1] = g, res[2] = b;
		return std::shared_ptr<unsigned char>(res);
	};

	std::string change_count_info;
	int change_cnt = 0;
	int last_change_cnt;

	int lastclock, curclock;

	int times = 0;
	int sum = 0, avg;

	//disp.

	while (!disp.is_closed())
	{
		lastclock = clock();

		lastm = m;
		m.clear();
		run_program("curl -s https://www.luogu.com.cn/paintboard/board", 8000,
			[&m](const std::string& s, void* p) {
				m += s;
			},
			NULL
		);
		get_img_from_map(ct, m, img1);
		last_change_cnt = change_cnt;
		if (lastm.empty())change_cnt = 0;
		else change_cnt = get_change_count(lastm, m);

		curclock = clock();

		showimg.fill(205);
		showimg.draw_image(img1);
		showimg.draw_text(
			PBW + 10, 10, "Changes: %d(%d)\nTime spent: %d(%d) ms",
			col_textfore, col_back, 1, 15,
			change_cnt, sum,
			curclock - lastclock, curclock
		);

		times++;

		int blank = 30;

		sum += change_cnt;
		avg = sum / times;

		scrolling_img.draw_image(scrolling_img.get_crop(blank, 0, SW - 1, 199));
		scrolling_img.draw_rectangle(SW - blank, 0, SW - 1, 199,cimg_color(0,0,0).get());

		int pos1 = 200.0 * (last_change_cnt / 100.0), pos2 = 200.0 * (change_cnt / 100.0);
		pos1 = 200 - pos1, pos2 = 200 - pos2;
		scrolling_img.draw_line(SW - blank, pos1, SW - 1, pos2, cimg_color(0, 255, 0).get());
		scrolling_img.draw_line(SW - blank, 100, SW - 1, 100, cimg_color(0, 100, 0).get());

		scrolling_img.draw_line(SW - 1, 0, SW - 1, 199, cimg_color(0, 20, 0).get());
		scrolling_img.draw_text(SW - blank, 180, "%d", col_textfore, 0, 1, 13, times);

		showimg.draw_image(PBW + 5, 45, scrolling_img);

		pos1 = 200.0 * (avg / 100.0); pos1 = 200 - pos1;
		showimg.draw_line(PBW + 5, 45 + pos1, PBW + SW + 4, 45 + pos1, cimg_color(150, 0, 0).get());
		showimg.draw_text(PBW + 5, 45 + pos1 - 6, "avg: %d", cimg_color(250, 60, 50).get(), 0, 1, 14, avg);

		//scrolling_img.draw_image(0, 0, );

		showimg.display(disp);
		Sleep(5);
	}
	return 0;
}
