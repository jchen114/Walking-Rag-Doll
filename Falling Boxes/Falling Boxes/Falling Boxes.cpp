// Falling Boxes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BasicTest.h" // For testing
#include "FreeGLUTCallbacks.h"
#include "FallingBoxesApplication.h"



int main(int argc, char **argv)
{

	FallingBoxesApplication fallingBoxes(ORTHOGRAPHIC);
	//FallingBoxesApplication fallingBoxes(PERSPECTIVE);
	return glutmain(argc, argv, 1024, 768, "Falling Boxes", &fallingBoxes);
}
