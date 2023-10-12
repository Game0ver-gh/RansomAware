#include "include/include.hpp"

//https://logrhythm.com/blog/a-technical-analysis-of-wannacry-ransomware/

//RansomAware - main dropper soft

int main(int argc, char** argv)
{
	printf("%s\n", "Normal string");
	printf("%s\n", (char*)_("RansomAware"));
}