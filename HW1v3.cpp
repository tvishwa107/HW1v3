// HW1v3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <errno.h>
#include <fstream>
#include <String>
#include <exception>
#include <tuple>
#include <vector>
#include <map>
#include <algorithm>
#include <Psapi.h>

#define MAX_RAM 500000000

typedef unsigned __int64 uint64;
typedef unsigned uint;
#pragma pack(push,1)   // change struct packing to 1 byte 
class HeaderGraph {
public:
	uint64 hash;
	uint len;

};
#pragma pack(pop) 

typedef unsigned short ushort;
#pragma pack(push,1)   // change struct packing to 1 byte 
class HeaderMap {
public:
	uint64 hash;
	ushort len;
};
#pragma pack(pop) 


// Custom comparator function for std::sort
bool vecSort(std::pair<uint64, int> &firstElem, std::pair<uint64, int> &secondElem)
{
	return firstElem.second > secondElem.second;
}


//These two incomplete functions were meant to make reading and parsing of data smoother
void parseBuf(char *buf, int bytesToRead, FILE* myHandle, std::map<uint64, int> &ref);
void write_to_file(std::map<uint64, int> &edgeMap, int i);
void readMyHandle(FILE *myHandle) //reads the file till buffer ends ONCE. This is the next place to be edited
{
	std::map<uint64, int> edges;
	//std::cout << "\nEntered RMH\n";
	char *buf=(char *)malloc(MAX_RAM*sizeof(char));   // file contents are here

	size_t success, retVal = 1;
	int i = 0;
	try{
		while (!feof(myHandle)) //the end of the while loop is hitting the end of the buffer (ie full MAX_RAM absorbed)
		{
			std::cout << "\nEntering retVal loop\n";
			success = fread(buf, sizeof(char), MAX_RAM, myHandle);
			//std::cout << "\n Position is " << ftell(myHandle) << " \n";
			//At this point, call new function that does the parsing
			parseBuf(buf, MAX_RAM, myHandle, edges);
			//std::cout << "\i is now " << i << "\n";
			write_to_file(edges, i);
			i++;
			std::cout << "\i is now " << i << "\n";
			memset(buf, 0, MAX_RAM);
			std::cout << "Retval final= " << retVal << std::endl;
		}
    }
	catch(int e)
	{
		std::cout << "\n Exception Number " << e;
	}
	
}

void write_to_file(std::map<uint64, int> &edgeMap, int i)
{		
	std::vector<std::pair<uint64, int > > myVec(edgeMap.begin(), edgeMap.end());
	std::cout << myVec.size();
	std::sort(myVec.begin(), myVec.end(), vecSort);
	//system("pause");
	std::cout << "Entered file write phase";
	//for (int i = 0; i < myVec.size(); i++)
	//{
	//	std::cout << myVec[i].first << " - " << myVec[i].second << std::endl;
	//}
	//system("pause");
	std::vector <std::pair <uint64, int >>::const_iterator cIter;
	const std::string iStr = std::to_string(i);
	FILE *outHandle = fopen(iStr.c_str(), "wb");
	if (outHandle == NULL)
	{
		std::cout << "File didn't open";
		return;
	}
	for (cIter = myVec.begin(); cIter != myVec.end(); ++cIter)
	{
		fprintf(outHandle, "%I64u %d", cIter->first, cIter->second);
	}
	fclose(outHandle);
	return;
}

void parseBuf(char *buf, int bytesToRead, FILE* myHandle, std::map<uint64,int> &ref)
{
	
	uint64 off = 0;
	uint64 size = MAX_RAM;
	uint64 ooff;
	std::map<uint64, int>::iterator it;
	std::cout << "\noff = "<<off<<" and size = "<<size<<std::endl;
	//if (off < size - sizeof(HeaderGraph))
	//	std::cout << "\nCondition met with off, size-HG = " << off << " " << size-sizeof(HeaderGraph)<<" \n";
	//std::cout << "\nPosn at beginning of loop is is " << ftell(myHandle) << std::endl;
	while (off < size - sizeof(HeaderGraph))
	{
		// the header fits in the buffer, so we can read it   
		HeaderGraph *hg = (HeaderGraph *)(buf + off);
		ooff = off;
		off += sizeof(HeaderGraph) + hg->len * sizeof(uint64);
		std::cout << "\noff is now increased from "<<ooff<<" by "<< sizeof(HeaderGraph) <<"+"<< hg->len * sizeof(uint64) <<" \n";
		if (off <= size)
		{
			std::cout << "\nEntered if loop in PB\n";
			//printf("Node %I64u, degree %d\n", hg->hash, hg->len);
			uint64 *neighbors = (uint64*)(hg + 1);
			for (int i = 0; i < hg->len; i++)
			{
				printf("  %I64u\n", neighbors[i]);
				it = ref.find(neighbors[i]);
				if (it == ref.end())
				{
					ref[neighbors[i]] = 1;	
				}
				else
				{
					it->second++;
				}

			}
		}
		else
			// neighbor list overflows buffer; handle boundary case 
		{
			std::cout << "\nEntered else loop in PB\n";
			system("pause");
			uint64 mypos;
			signed long int mynewpos=0;
			int res;
			size_t succ;
			//mypos = ftell(myHandle);  mypos is always MAX_RAM, so subtract that from off each time. Subtract That
			
			//std::cout << "\nmypos = " << mypos <<" and off = "<<off<<"\n";
		//	std::cout << off - sizeof(HeaderGraph) - hg->len*sizeof(uint64);
			//mynewpos -= SEEK_CUR+(sizeof(HeaderGraph) + hg->len*sizeof(uint64))-off-1;
			mynewpos = off - (sizeof(HeaderGraph) + hg->len*sizeof(uint64));
			//std::cout << "\nval = " <<  off- (sizeof(HeaderGraph) + hg->len*sizeof(uint64));
			res = fseek(myHandle, -1*(MAX_RAM-mynewpos), SEEK_CUR);
			std::cout << "	\nPB went else and is returning 0 and file pointer is now at posn "<<ftell(myHandle)<<"\n";
			//system("pause");
			return;
		}

	}
	return;
}

/*void merge_files()
{
		

}

*/
int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		std::cout << "Enter only the name of the outgraph\n.";
		return 1;
	}

	FILE *inMap, *inData;
	errno_t errMap, errData;
	//errMap = fopen_s(&inMap, "C:\\Users\\tvish_000\\Downloads\\PLD-map-(1).dat", "rb");
	errData = fopen_s(&inData, argv[1], "rb");

	//if (errMap)
	//{
	//	std::cerr << "Can't open input map file " <<std::endl;
	//}
	if (errData)
	{
		std::cerr << "Can't open input file " << argv[2] << std::endl;
		return 1;
	}
	else 
	{
		std::cout << "\nEntered else, so file opened\n";
		readMyHandle(inData);
		
	} 
	getchar();

}