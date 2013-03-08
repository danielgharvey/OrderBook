/*
 * GenerateData.cpp
 *
 *  Created on: 6 Mar 2013
 *      Author: chaste
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <map>
#include <float.h>
#include <cmath>

int main (int argc, char* argv[])
{
	std::ofstream fout;
	fout.open("big_data.txt");

	int num_ops = 1e4;

	for (int i=0; i<=num_ops; i++)
	{
		fout << i << " " << "I" << " " << i << " " << (double)(i+1);
		if (i<num_ops)
		{
			fout << "\n";
		}
	}

	int j=num_ops+1;
	for (int i=num_ops; i >= 0; i--)
	{
		fout << "\n" <<  j << " " << "E" << " " << i;
		j++;
	}
}
