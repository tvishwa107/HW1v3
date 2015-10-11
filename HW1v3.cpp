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

#define MAX_BUF 495000000
#define OUT_BUF   4000000

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


void parseBuf(char *buf, int bytesToRead, FILE* myHandle, char *outBuf, int *k);

void buffer_to_file(char *Buf, uint64 i, int *k, FILE *outHandle);

void sort_file()
{




}


//When is makeHandle called? When the buffer becomes full in the middle of processing
FILE *make_Handle(int *k, FILE *optHandle = NULL)
{
	if (optHandle)
	{
		fclose(optHandle);
		std::cout << "\nclosing file " << *k << " and opening file " << (*k)+1<<std::endl;
	}
	const std::string kStr = std::to_string(*k);
	FILE *outHandle = fopen(kStr.c_str(), "wb");
	std::cout<<"Opened file " << (*k) + 1 << std::endl;
	return outHandle;
}

void readMyHandle(FILE *myHandle) //reads the file till buffer ends ONCE. This is the next place to be edited
{
	//std::cout << "\nEntered RMH\n";
	char *buf=(char *)malloc(MAX_BUF*sizeof(char));   // file contents are here
	char *outBuf = (char *)malloc(OUT_BUF*sizeof(char));
	size_t success;
	int i = 0;
	int *k = &i;
	try{
		do {
			//std::cout << "\nEntering retVal loop\n";
			success = fread(buf, sizeof(char), MAX_BUF, myHandle);
			//std::cout << "\n Position is " << ftell(myHandle) << " \n";
			//At this point, call new function that does the parsing
			parseBuf(buf, MAX_BUF, myHandle, outBuf, k);
			//std::cout << "\ni is now " << *k <<	 "\n";
			memset(buf, 0, MAX_BUF);
		} while (success == MAX_BUF);
    }
	catch(int e)
	{
		std::cout << "\n Exception Number " << e;
	}
	
}


uint64 write_to_buffer(HeaderGraph *hg, char *outBuf, uint64 i, int *k, FILE *outHandle)
{		
	//i is the number of HeaderGraphs written, k is the number n of the file name eg. 0,1,2
	
//option 1: do what he did, but mine's already slower
//2: rerun mine, with a limit on how big the file can be. i'll try 2

	if(ftell(outHandle)+sizeof(HeaderGraph)>=MAX_BUF)
	{
		//std::cout << "outHandle = ";
		//std::cout << ftell(outHandle);
		//std::cout << "\ni=" << i << std::endl;
		//system("pause");
		buffer_to_file(outBuf, i, k, outHandle); //copy buffer to file
												 //std::cout << "Done";
		++*k;
		outHandle = make_Handle(k,outHandle);
		memset(outBuf, 0, OUT_BUF);   //reset buffer
		return 0; //reset count of number in buffer
//once you've added this, after the first time, you need a way to signal that it's time to close and open a new file
	}
	if (((i+1)*sizeof(HeaderGraph)) > OUT_BUF)
	{ 
		//std::cout << "\ni=" << i << std::endl;
		
		buffer_to_file(outBuf, i, k, outHandle); //copy buffer to file
		
		//std::cout << "Done";
		//system("pause");
		
		memset(outBuf, 0, OUT_BUF);   //reset buffer
		return 0; //reset count of number in buffer
	}
	else //buffer can hold one more at least
	{
		//std::cout << "\nEse in W2B\n";
	    memcpy(outBuf + i*sizeof(HeaderGraph), hg, sizeof(HeaderGraph)); 
		//std::cout << "\ni = " << i;
		return ++i;
	}
}


void buffer_to_file(char *buf, uint64 i, int *k, FILE *outHandle)
{
	size_t succ = fwrite(buf, sizeof(HeaderGraph), i, outHandle);
	if(succ==0)
	{
		//std::cout << "succ = " << succ << " and i = " << i;
		//system("pause");
		return;
	}
	//else std::cout << "succ = i = " << succ << " " << i << std::endl;
	return;


}

void parseBuf(char *buf, int bytesToRead, FILE* myHandle, char *outBuf, int *k)
{
	
	uint64 off = 0;
	uint64 size = MAX_BUF;
	uint64 bufsize = OUT_BUF;
	uint64 ooff, num = 0;
	signed long int mynewpos = 0;
	//std::cout << "\noff = "<<off<<" and size = "<<size<<std::endl;
	

	FILE *outHandle = make_Handle(k);

	while (off < size - sizeof(HeaderGraph))
	{
		// the header fits in the buffer, so we can read it   
		HeaderGraph *hg = (HeaderGraph *)(buf + off);
		ooff = off;
		off += sizeof(HeaderGraph) + hg->len * sizeof(uint64);
		//std::cout << "\noff is now increased from "<<ooff<<" by "<< sizeof(HeaderGraph) <<"+"<< hg->len * sizeof(uint64) <<" \n";
		ooff = off;
		if (off <= size)
		{
			//std::cout << "\nEntered if loop in PB\n";
			//printf("Node %I64u, degree %d\n", hg->hash, hg->len);
			uint64 *neighbors = (uint64*)(hg + 1);
			for (int i = 0; i < hg->len; i++)
			{
				//printf("  %I64u\n", neighbors[i]);
				HeaderGraph temp;
				temp.hash = neighbors[i];
				temp.len = 1;
				num = write_to_buffer(&temp, outBuf,num, k, outHandle);
				

			}
		}
		else
			// neighbor list overflows buffer; handle boundary case 
		{
			//std::cout << "\nEntered else loop in PB\n";
			//system("pause");
			uint64 mypos;
			
			int res;
			size_t succ;

			mynewpos = off - (sizeof(HeaderGraph) + hg->len*sizeof(uint64));
			//std::cout << "\nval = " <<  off- (sizeof(HeaderGraph) + hg->len*sizeof(uint64));
			res = fseek(myHandle, -1*(MAX_BUF-mynewpos), SEEK_CUR);
			//std::cout << "	\nPB went else and is returning 0 and file pointer is now at posn "<<ftell(myHandle)<<"\n";
			//system("pause");
			return;
		}

	}
	//std::cout << "Failed headertest: off = " <<off<<"ooff = "<<ooff<<", size - HeaderGraph = "<<size<<"-"<<sizeof(HeaderGraph);
	fseek(myHandle, -1 * (MAX_BUF - (ooff)), SEEK_CUR);
	//std::cout<<std::endl<<ftell(myHandle)<<"\n";
	fclose(outHandle);
	return;
}

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