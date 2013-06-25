#include <reNX.cpp.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>

inline double_t mean50(std::vector<uint64_t> v) {
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
	
	return sum + static_cast<double_t>(rest)/size;
}

#if defined RENXCPP_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define NXPATH "D:\\Misc\\Development\\NX\\PKG4.nx"
#error Out of date.
inline uint64_t gethpc() {
	LARGE_INTEGER n;
	QueryPerformanceCounter(&n);
	return n.QuadPart;
}

inline uint64_t getfreq() {
	LARGE_INTEGER n;
	QueryPerformanceFrequency(&n);
	return n.QuadPart;
}

void test(const char *name, void (*m)(), bool deletef) {
	uint64_t freq = getfreq();
	uint64_t best = -1;
	uint64_t c0 = gethpc();
	do {
		uint64_t c1 = gethpc();
		m();
		uint64_t c2 = gethpc();
		if(deletef) delete f;
		uint64_t dif = c2 - c1;
		if (dif < best) best = dif;
	} while (gethpc() - c0 < freq >> 1);
	printf("%s: %lluus ", name, best * 1000000ULL / freq);
}
#elif defined RENXCPP_LNX
#include <time.h>
#define NXPATH "/home/angelsl/Desktop/NX/PKG4.nx"
struct timespec _begin, _now;

inline void mark() { std::cerr << "\x1b[s"; }
inline void snap() { std::cerr << "\x1b[u\x1b[J"; }
inline double_t norm(double_t i) { return i; }
inline void lap() { clock_gettime(CLOCK_MONOTONIC_RAW, &_begin); }
inline uint64_t reading() {
	clock_gettime(CLOCK_MONOTONIC_RAW, &_now);
	return (_now.tv_sec-_begin.tv_sec)*1000000 + (_now.tv_nsec-_begin.tv_nsec)/1000;
}

#endif

inline void test(const char *name, uint64_t (*timee)(), uint32_t trials, uint32_t step = 1, void (*prepare)() = nullptr, void (*postpare)() = nullptr) {
	setbuf(stderr, NULL);
	std::vector<uint64_t> result;
	result.reserve(trials);
	uint64_t best = -1;
	mark();
	if(prepare) prepare();
	for (uint32_t c = 0; c < trials;) {
		for (uint d = 0; d < step && c < trials; ++c, ++d) result.push_back(timee());
		snap();
		std::cerr << name << ": " << std::setw(4) << c << '/' << std::left << std::setw(4) << trials << std::right;
		if (step == 1) std::cerr << "; C" << std::setw(8) << result.back() << " B" << std::setw(8) << (best > result.back() ? best = result.back() : best);
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
	for(auto m : n) RecurseHelper(m);
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
		else *reinterpret_cast<uint64_t *>(0) = 0xDEADDEADDEADDEAD;
}

uint64_t SearchAll() {
	lap();
	SearchHelper(_f->base());
	return reading();
}

int main()
{
	std::cout << "Name\t75%t\tM50%\tBest" << std::endl;
	test("Ld", Load, 0x200);
	test("Re", Recurse, 0x200, 1, []() { _f = new reNX::NXFile(NXPATH); }, []() { delete _f; });
	test("LR", LoadRecurse, 0x40);
	test("SA", SearchAll, 0x40, 1, []() { _f = new reNX::NXFile(NXPATH); }, []() { delete _f; });
}
