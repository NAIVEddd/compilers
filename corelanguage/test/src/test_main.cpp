#include"lexTest.h"
#include"parseTest.h"
#include"exprTest.h"
#include"templateInstantiationTest.h"

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}