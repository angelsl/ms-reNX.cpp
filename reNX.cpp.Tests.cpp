#include <reNX.cpp.hpp>
#include <iostream>

#if defined RENXCPP_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define NXPATH "D:\\Misc\\Development\\NX\\PKG4.nx"
reNX::NXFile *f = new reNX::NXFile(NXPATH);
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
reNX::NXFile *f = new reNX::NXFile(NXPATH);
void test(const char *name, void (*m)(), bool deletef) {
	setbuf(stdout, NULL);
	uint64_t best = -1;
	uint64_t c = 0;
	do {
		struct timespec a, b;
		clock_gettime(CLOCK_MONOTONIC_RAW, &b);
		m();
		clock_gettime(CLOCK_MONOTONIC_RAW, &a);
		if(deletef) delete f;
		uint64_t dif = (a.tv_sec-b.tv_sec)*1000000 + (a.tv_nsec-b.tv_nsec)/1000;
		if(dif < best) best = dif;
		++c;
	} while (c < (2000000/best));
	printf("%s: %luus ", name, best);
}
#endif

reNX::NXNode n = f->base()["Map"]["Map"]["Map1"]["105060000.img"]["1"]["tile"];
uint64_t dent = 0;

void Ld() {
	reNX::NXFile df(NXPATH);
}

void Ss() {
	for(auto b : n) if(n[b.nxname()] != b) throw b;
}

inline void recurse(const reNX::NXNode& N) {
	for(auto c : N) { recurse(c); }
}

void Pr() {
	f = new reNX::NXFile(NXPATH);
	recurse(f->base());
}

void Re() {
	recurse(f->base());
}

int main()
{
	test("Ld", Ld, false);
	test("Ss", Ss, false);
	delete f;
	test("Pr", Pr, true);
	f = new reNX::NXFile(NXPATH);
	test("Re", Re, false);
	delete f;
	printf("\n");
}
