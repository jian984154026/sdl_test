#include "iostream"
#include "SdlTest.h"

#ifdef main
#undef main
#endif 

using namespace std;

int main(int argc, char* argv[])
{
	CSdlTest test;
	test.LoadConfig("test.ini");
	test.YuvTest();
	test.Destroy();

	return 0;
}
