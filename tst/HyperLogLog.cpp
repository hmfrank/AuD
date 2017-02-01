#include <cmath>
#include <catch.hpp>

extern "C"
{
#include "../inc/HyperLogLog.h"
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
	return x >= y - y * error && x <= y + y * error;
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

	double sum = 0;
	double avg = 0;

	REQUIRE(hllCount(&set) == 0);

	for (int i = 1; i <= 100; i++)
	{
		hllAdd(&set, (void*)i);
		sum += (hllCount(&set) - i) / i;
	}
	avg = sum / 100;
	REQUIRE(avg <= 0.026);

	for (int i = 101; i <= 1000; i++)
	{
		hllAdd(&set, (void*)i);
		sum += (hllCount(&set) - i) / i;
	}
	avg = sum / 1000;
	REQUIRE(avg <= 0.026);

	for (int i = 1001; i <= 10000; i++)
	{
		hllAdd(&set, (void*)i);
		sum += (hllCount(&set) - i) / i;
	}
	avg = sum / 10000;
	REQUIRE(avg <= 0.026);

	// adding the same items again should not count
	for (int i = 0; i < 1000; i++)
	{
		hllAdd(&set, (void*)i);
	}
	REQUIRE(hllCount(&set) <= 10300);

	hllFree(&set);
}