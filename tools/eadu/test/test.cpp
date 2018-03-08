#include <iostream>

const char test(0xFF);
const int test2(0);

int main(int argc, char *argv[])
{
	std::cout << sizeof(test2);
	return 0;
}