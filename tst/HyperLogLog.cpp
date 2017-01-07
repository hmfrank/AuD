#include <cmath>
#include "../lib/catch.hpp"

extern "C"
{
#include "../src/HyperLogLog.h"
}

void hash(void *item, size_t h, void *buffer)
{
	char *char_ptr = (char*)buffer;

	srand((unsigned)item);

	for (size_t i = 0; i < h; i++)
	{
		char_ptr[i] = (char)rand();
	}
}

int isClose(double x, double y, double error)
{
	return x > y - y * error && x < y + y * error;
}

TEST_CASE("HyperLogLog", "[src/HyperLogLog.h]")
{
	struct HyperLogLog set;

	REQUIRE(hllInit(NULL, MEDIUM, 11, &hash) == 1);
	REQUIRE(hllInit(&set, 5, 11, &hash) == 2);
	// this assert might fail on a > 64bit machine
	REQUIRE(hllInit(&set, LARGE, 64, &hash) == 3);
	REQUIRE(hllInit(&set, SMALL, 11, NULL) == 4);
	REQUIRE(hllInit(&set, MEDIUM, 11, &hash) == 0);

	REQUIRE(std::isnan(hllCount(NULL)));

	hllAdd(&set, (void*)0);
	REQUIRE(isClose(hllCount(&set), 1, 0.026));

	for (int i = 0; i < 1000000; i++)
		hllAdd(&set, (void*)i);

	REQUIRE(isClose(hllCount(&set), 1000000, 0.026));

	for (int i = 0; i < 10000; i++)
		hllAdd(&set, (void*)i);

	REQUIRE(isClose(hllCount(&set), 1000000, 0.026));

	for (int i = 1000000; i < 10001000; i++)
		hllAdd(&set, (void*)i);

	REQUIRE(isClose(hllCount(&set), 1001000, 0.026));

	hllFree(&set);
}