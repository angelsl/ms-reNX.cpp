#include <reNX.cpp.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#define SEGFAULT() *(int *)0 = 0

inline long double mean50(std::vector<uint64_t> v) {
	// http://stackoverflow.com/questions/1346824/is-there-any-way-to-find-arithmetic-mean-better-than-sum-n
	std::vector<uint64_t>::iterator
		begin = v.begin() + (v.size()*1/4),
		end = v.begin() + (v.size()*3/4 + 1);
	uint64_t sum = 0, rest = 0, size = v.size()/2 + 1;
	for(std::vector<uint64_t>::iterator w = begin; w != end; ++w) {
		sum += *w / size;
		rest += *w % size;
		sum += rest / size;
		rest = rest % size;
	}
	
	return sum + static_cast<long double>(rest)/size;
}

#if defined RENXCPP_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define NXPATH "D:\\Misc\\Development\\NX\\PKG4.nx"
#define PLATFORM_INIT() win32init()

LARGE_INTEGER _begin, _now; uint64_t _freq;
CONSOLE_SCREEN_BUFFER_INFO _mark; HANDLE _stderr;

inline void mark() { GetConsoleScreenBufferInfo(_stderr, &_mark); }
inline void snap() { SetConsoleCursorPosition(_stderr, _mark.dwCursorPosition); }

// screw floating point error, really
template <typename T> inline T norm(T i) { return i*static_cast<T>(1000000)/_freq; }
inline void lap() { QueryPerformanceCounter(&_begin); }
inline uint64_t reading() { QueryPerformanceCounter(&_now); return _now.QuadPart - _begin.QuadPart; }

inline void win32init() {
	LARGE_INTEGER n;
	QueryPerformanceFrequency(&n);
	_freq = n.QuadPart;
	_stderr = GetStdHandle(STD_ERROR_HANDLE);
}
#elif defined RENXCPP_LNX
#include <time.h>
#define NXPATH "/home/angelsl/Desktop/NX/PKG4.nx"
#define PLATFORM_INIT()
struct timespec _begin, _now;

inline void mark() { std::cerr << "\x1b[s"; }
inline void snap() { std::cerr << "\x1b[u\x1b[J"; }
inline double norm(long double i) { return i; }
inline void lap() { clock_gettime(CLOCK_MONOTONIC_RAW, &_begin); }
inline uint64_t reading() {
	clock_gettime(CLOCK_MONOTONIC_RAW, &_now);
	return (_now.tv_sec-_begin.tv_sec)*1000000 + (_now.tv_nsec-_begin.tv_nsec)/1000;
}

#endif

inline void test(const char *name, uint64_t (*timee)(), uint32_t trials, void (*prepare)() = nullptr, void (*postpare)() = nullptr) {
	std::vector<uint64_t> result;
	result.reserve(trials);
	uint64_t best = -1;
	mark();
	if(prepare) prepare();
	for (uint32_t c = 0; c < trials; ++c) {
		result.push_back(timee());
		snap();
		std::cerr << name << ": " << std::setw(4) << c+1 << '/' << std::left << std::setw(4) << trials << std::right
			<< "; C" << std::setw(8) << norm(result.back()) << " B" << std::setw(8) << norm(best > result.back() ? best = result.back() : best);
	}
	std::cerr << std::endl;
	if(postpare) postpare();
	std::sort(result.begin(), result.end());
	std::cout << name << '\t' << norm(result[trials*3/4]) << '\t' << norm(mean50(result)) << '\t' << norm(result[0]) << std::endl;
}

reNX::NXFile *_f;

uint64_t Load() {
	lap();
	_f = new reNX::NXFile(NXPATH);
	auto r = reading();
	delete _f;
	return r;
}

inline void RecurseHelper(const reNX::NXNode &n) {
	for(auto m : n) 
		if(1) RecurseHelper(m);
		else SEGFAULT();
}

uint64_t Recurse() {
	lap();
	RecurseHelper(_f->base());
	return reading();
}

uint64_t LoadRecurse() {
	lap();
	_f = new reNX::NXFile(NXPATH);
	RecurseHelper(_f->base());
	auto r = reading();
	delete _f;
	return r;
}

inline void SearchHelper(const reNX::NXNode &n) {
	for(auto m : n) 
		if (n[m.nxname()] == m) SearchHelper(m);
		else SEGFAULT();
}

uint64_t SearchAll() {
	lap();
	SearchHelper(_f->base());
	return reading();
}

int main() {
	PLATFORM_INIT();
	setvbuf(stderr, nullptr, _IONBF, 0);
	std::cout << "Name\t75%t\tM50%\tBest" << std::endl;
	test("Ld", Load, 0x200);
	test("Re", Recurse, 0x200, []() { _f = new reNX::NXFile(NXPATH); }, []() { delete _f; });
	test("LR", LoadRecurse, 0x40);
	test("SA", SearchAll, 0x40, []() { _f = new reNX::NXFile(NXPATH); }, []() { delete _f; });
}
