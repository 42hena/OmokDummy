#include "CSimpleAI.h"

int CSimpleAI::posWeight[15][15] = { 
		{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{ 0,  0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  0,  0},
		{ 0,  0, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10,  0,  0},
		{ 0,  0, 10, 11, 12, 12, 12, 12, 12, 12, 12, 11, 10,  0,  0},
		{ 0,  0, 10, 11, 12, 13, 13, 13, 13, 13, 12, 11, 10,  0,  0},
		{ 0,  0, 10, 11, 12, 13, 14, 14, 14, 13, 12, 11, 10,  0,  0},
		{ 0,  0, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10,  0,  0},
		{ 0,  0, 10, 11, 12, 13, 14, 14, 14, 13, 12, 11, 10,  0,  0},
		{ 0,  0, 10, 11, 12, 13, 13, 13, 13, 13, 12, 11, 10,  0,  0},
		{ 0,  0, 10, 11, 12, 12, 12, 12, 12, 12, 12, 11, 10,  0,  0},
		{ 0,  0, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10,  0,  0},
		{ 0,  0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  0,  0},
		{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0} };