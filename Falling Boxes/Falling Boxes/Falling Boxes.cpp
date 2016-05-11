// Falling Boxes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BasicTest.h" // For testing
#include "FreeGLUTCallbacks.h"



int main(int argc, char **argv)
{
	BasicTest test;

	return glutmain(argc, argv, 1024, 768, "Basic test", &test);
}
