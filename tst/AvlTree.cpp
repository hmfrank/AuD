#include <catch.hpp>

extern "C"
{
#include <stddef.h>
#include "../inc/AvlTree.h"
}

int compare(const void *a, const void *b)
{
	ptrdiff_t d = (const char*)a - (const char *)b;

	if (d < 0) return -1;
	else return d > 0;
}

TEST_CASE("avl tree contains, delete, insert, is empty", "[inc/AvlTree.h/avlContains, inc/AvlTree.h/avlDelete, inc/AvlTree.h/avlInsert, inc/AvlTree.h/avlIsEmpty]")
{
	struct AvlTree tree;

	// corner case arguments
	REQUIRE_NOTHROW(avlInit(NULL, NULL));
	REQUIRE_NOTHROW(avlInit(NULL, compare));
	avlInit(&tree, NULL);
	REQUIRE(tree.compare(NULL, NULL) == 0);

	REQUIRE_NOTHROW(avlFree(NULL));
	avlFree(&tree);

	REQUIRE_FALSE(avlContains(NULL, (void *) 1));
	REQUIRE_FALSE(avlInsert(NULL, (void *) 1));
	REQUIRE(avlIsEmpty(NULL));

	// actual tests
	avlInit(&tree, &compare);
	REQUIRE(avlIsEmpty(&tree));

	for (int i = 0; i < 10; i++)
		REQUIRE(avlInsert(&tree, (void*)i));

	for (int i = 0; i < 10; i++)
		REQUIRE(avlContains(&tree, (void*)i));

	REQUIRE_FALSE(avlIsEmpty(&tree));
	REQUIRE_FALSE(avlInsert(&tree, (void*)2));
	REQUIRE_FALSE(avlContains(&tree, (void*)-1));

	avlFree(&tree);
}
