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
#include <ctime>
#include <sstream>

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


void parseBuf(char *buf, int bytesToRead, FILE* myHandle, char *outBuf, FILE *outHandle);

void buffer_to_file(char *Buf, uint64 i, FILE *outHandle);

bool custom_sort(const HeaderGraph &a, const HeaderGraph &b)
{
	return (a.hash) < (b.hash);
}



int num_merges(int k)
{
	int rem;
	int merge = 1;
	do
	{
		rem = k % 4;
		merge += k / 4;
		k = (k / 4) + rem;



	} while (k > 4);
	return merge;
}

BOOL write_to_file(HANDLE h, HeaderGraph *buf, BOOL h5Write)
{
	DWORD dH5,dw;
	WriteFile(h, buf, 333333 * sizeof(HeaderGraph), &dH5, NULL);
	dw = GetLastError();
	std::cout << "General failure. GetLastError returned " << std::hex
		<< dw << ".\n";
	memset(buf, 0, OUT_BUF);
	return h5Write;

}

int count_zeroes(int *k,int num)
{
	int count = 0;
	for (int i = 0; i < num; i++)
	{
		if ( k[i] == 0)
			count++;
	}
	return count;

}


void merge_files(int start_point, int *max, int a = 4) //merges two/built for four files at a time. Need to modify the inputs and outputs
{
	std::cout << "Entered merge files \n";
	int *merge_empty = new int[a]();
	//int l = 0, m = 1, n = 2, p = 3,z=1; //probably better in that other function that initiates the merge
	int *points = new int[a];
	for (int hazz = 0; hazz < a; hazz++)
		points[hazz] = start_point + hazz;
	HeaderGraph **buf = new HeaderGraph* [a];
	for(int myvar = 0; myvar < a; myvar++)
		buf[myvar] = new HeaderGraph[MAX_BUF/4];
	
	//HeaderGraph *buf1 = new HeaderGraph[MAX_BUF/4];
	//HeaderGraph *buf2 = new HeaderGraph[MAX_BUF/4];
	//HeaderGraph *buf3 = new HeaderGraph[MAX_BUF/4];
	//HeaderGraph *buf4 = new HeaderGraph[MAX_BUF/4];
	HeaderGraph *bufOut = new HeaderGraph[OUT_BUF];
	
	//uint64 ha, hb, hc, hd, ho;
	uint64 *hCount = new uint64[a]();
	//ha = hb = hc = hd = ho=0;
	uint64 ho = 0;
	uint64 *old = new uint64[4]();
	//uint64 old1, old2, old3, old4;
	DWORD dH[4];
	BOOL *hRead = new BOOL[4]();
	//BOOL h1Read, h2Read, h3Read, h4Read;
	BOOL h5Write = 0;
	HANDLE *h = new HANDLE[4]();
	std::vector<std::string> v;
	DWORD dw;
	std::string zStr = std::to_string(*max);
	HANDLE hOut = CreateFile(zStr.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	do {
		for (int aa = start_point; aa < a; aa++)
		{
			v.push_back(std::to_string(points[aa])); //cut and put in main merge function
			h[aa] = CreateFile(v[0].c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			hRead[aa] = ReadFile(h[aa], buf[aa], MAX_BUF / 4, &dH[aa], NULL);
			dw = GetLastError();
			std::cout << "General failure. GetLastError returned " << std::hex
				<< dw <<std::dec<<"for file "<<aa<<std::endl;
		}
		//	v.push_back(std::to_string(points[1]));
		//	v.push_back(std::to_string(points[2]));
		//	v.push_back(std::to_string(points[3]));

		//	h[0] = CreateFile(v[0].c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		//	h[1] = CreateFile(v[1].c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		//	h[2] = CreateFile(v[2].c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		//	h[3] = CreateFile(v[3].c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			//h1Read = ReadFile(h[0], buf1, MAX_BUF/4, &dH[0], NULL);
			//h2Read = ReadFile(h[1], buf2, MAX_BUF/4, &dH[1], NULL);
			//h3Read = ReadFile(h[2], buf3, MAX_BUF/4, &dH[2], NULL);
			//h4Read = ReadFile(h[3], buf4, MAX_BUF/4, &dH[3], NULL);

		for (int ll = 0; ll < a;ll++)
		{
		if (hCount[ll] < MAX_BUF / 48) // at least one buffer is not empty
		{
			if (sizeof(HeaderGraph)*(ho + 1) >= OUT_BUF)
			{
				h5Write = write_to_file(hOut, bufOut, h5Write);
				ho = 0;
			}

			else //still have space in outbuffer
			{
				HeaderGraph temp1, temp2, temp;
				temp1 = (buf[0][hCount[0]].hash <= buf[1][hCount[1]].hash) ? buf[0][hCount[0]] : buf[1][hCount[1]];
				temp2 = (buf[2][hCount[2]].hash <= buf[3][hCount[3]].hash) ? buf[2][hCount[2]] : buf[3][hCount[3]];
				temp = (temp1.hash <= temp2.hash) ? temp1 : temp2;
				if (temp.hash == buf[0][hCount[0]].hash)
				{
					old[0] = hCount[0];
					while (temp.hash == buf[0][++hCount[0]].hash)
						temp.len++;
					//memcpy(buf + ho, temp, *sizeof(HeaderGraph));
					//ho += ha - old1;


				}
				if (temp.hash == buf[1][hCount[1]].hash)
				{
					old[1] = hCount[1];
					while (temp.hash == buf[1][++hCount[1]].hash)
						temp.len++;
					//memcpy(buf + ho, buf2 + old2, (hb - old2)*sizeof(HeaderGraph));
					//ho += hb - old2;


				}
				if (temp.hash == buf[2][hCount[2]].hash)
				{
					old[2] = hCount[2];
					while (temp.hash == buf[2][++hCount[2]].hash)
						temp.len++;
					//memcpy(buf + ho, buf3 + old3, (hc - old3)*sizeof(HeaderGraph));
					//ho ++;


				}
				if (temp.hash == buf[3][hCount[3]].hash)
				{
					old[3] = hCount[3];
					while (temp.hash == buf[3][++hCount[3]].hash)
						temp.len++;
					//memcpy(buf + ho, buf4 + old4, (hd - old4)*sizeof(HeaderGraph));
					//ho++;
				}
				ho++;


			}
			if (hCount[0] == MAX_BUF / 48)
			{
				hRead[0] = ReadFile(h[0], buf[0], MAX_BUF / 4, &dH[0], NULL);
				hCount[0] = 0;
			}
			if (hCount[1] == MAX_BUF / 48)
			{
				hRead[1] = ReadFile(h[1], buf[1], MAX_BUF / 4, &dH[1], NULL);
				hCount[1] = 0;
			}
			if (hCount[2] == MAX_BUF / 48)
			{
				hRead[2] = ReadFile(h[2], buf[2], MAX_BUF / 4, &dH[2], NULL);
				hCount[2] = 0;
			}
			if (hCount[3] == MAX_BUF / 48)
			{
				hRead[3] = ReadFile(h[3], buf[3], MAX_BUF / 4, &dH[3], NULL);
				hCount[3] = 0;
			}

		}
	}
		if (dH[0] == 0)
		{
			CloseHandle(h[0]);
			merge_empty[0] = 1;
		}
		if (dH[1] == 0)
		{
			CloseHandle(h[1]);
			merge_empty[1] = 1;
		}
		if (dH[2] == 0)
		{
			CloseHandle(h[2]);
			merge_empty[2] = 1;
		}
		if (dH[3] == 0)
		{
			CloseHandle(h[3]);
			merge_empty[3] = 1;
		}


	} while (dH[0] || dH[1] || dH[2] || dH[3]);
	CloseHandle(hOut);

	for (int var = 0; var < 4;var++)
		if (merge_empty[var] == 0)
			CloseHandle(h[var]);
	(*max)++;

}



void merge_call(int l)
{
	std::cout << "Entered merge call\n";
	int k = num_merges(l);
	int *merger = new int[k+l*2]();
	for (int jaja = 0; jaja < l; jaja++)
		merger[jaja] = 1;
	int num = k + l;
	int *max = &num;
	int a = count_zeroes(merger, k+l);
	int i = 0;
	while (a > 1)
	{
		if (a <= 4)
		{
			merge_files(l, max, l+a);
			for (int var = 0; var < a; var++)
				merger[l + var] = 1;
		}
		else
		{
			merge_files(i, max);
			for (int var = 0; var < 4; var++)
				merger[i + var] = 1;
		}
		a = count_zeroes(merger, k + l);


	}





}


void readMyHandle(FILE *myHandle, FILE *outHandle) //reads the file till buffer ends ONCE. This is the next place to be edited
{
	//std::cout << "\nEntered RMH\n";
	char *buf=(char *)malloc(MAX_BUF*sizeof(char));   // file contents are here
	char *outBuf = (char *)malloc(OUT_BUF*sizeof(char));
	size_t success;
	try{
		do {
			//std::cout << "\nEntering retVal loop\n";
			success = fread(buf, sizeof(char), MAX_BUF, myHandle);
			//std::cout << "\n Position is " << ftell(myHandle) << " \n";
			//At this point, call new function that does the parsing
			parseBuf(buf, MAX_BUF, myHandle, outBuf,outHandle);
			//std::cout << "\ni is now " << *k <<	 "\n";
			memset(buf, 0, MAX_BUF);
		} while (success == MAX_BUF); //edited to let me copy 4x Max RAM
		free(buf);
		free(outBuf);
    }
	catch(int e)
	{
		std::cout << "\n Exception Number " << e;
	}
}


uint64 write_to_buffer(HeaderGraph *hg, char *outBuf, uint64 i, FILE *outHandle)
{		
	//i is the number of HeaderGraphs written to the outBuf so far

	if (((i+1)*sizeof(HeaderGraph)) > OUT_BUF)
	{ 
		
		buffer_to_file(outBuf, i, outHandle); //copy buffer to file
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


void buffer_to_file(char *buf, uint64 i, FILE *outHandle)
{
	fwrite(buf, sizeof(HeaderGraph), i, outHandle);
	//else std::cout << "succ = i = " << succ << " " << i << std::endl;
	return;


}

void parseBuf(char *buf, int bytesToRead, FILE* myHandle, char *outBuf, FILE *outHandle)
{
	
	uint64 off = 0;
	uint64 size = MAX_BUF;
	uint64 bufsize = OUT_BUF;
	uint64 ooff, num = 0;
	signed long int mynewpos = 0;

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
				num = write_to_buffer(&temp, outBuf,num, outHandle);
	
				//idea: return -1 to indicate time to close this handle and open a new handle

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

void copy_smaller(FILE *inF, FILE *outF)
{
	char *readBuf = (char *)malloc(sizeof(char) *MAX_BUF);
	int i = 0;
	clock_t begin = clock();
	while (i < 4)
	{
		std::cout << i << std::endl;
		fread(readBuf, sizeof(char), MAX_BUF, inF);
		fwrite(readBuf, sizeof(char), MAX_BUF, outF);
		i++;
	}
	clock_t end = clock();
	double elapsed = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << elapsed << "\n";
	return;

}


int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		std::cout << "Enter only the name of the outgraph\n.";
		return 1;
	}

	FILE *inMap, *inData, *outData;
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

		errno_t errOut;
		errOut = fopen_s(&outData, "outSample", "wb");
		//std::cout << "\nEntered else, so file opened\n";
		readMyHandle(inData, outData);
		//copy_smaller(inData, outData);
		fclose(inData);
		fclose(outData);
		//int b = 30;
		//int *a = &b;
		//std::cout << *a << std::endl;
	
		
		DWORD red,blue;
		char *myBuf = (char *)malloc(MAX_BUF*sizeof(char));
		HANDLE h = CreateFile("outSample", GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		BOOL bResult, cResult;
		HANDLE hOut;
		int l = 0;
		uint64 hgNum = MAX_BUF / sizeof(HeaderGraph);
		do {
			bResult = ReadFile(h, myBuf, MAX_BUF, &red, NULL);
			if (!bResult || red == 0)
			{
				std::cout << bResult << " " << red << std::endl;
				DWORD dw = GetLastError();
				std::cout << "General failure. GetLastError returned " << std::hex
					<< dw << ".\n";

			}
			HeaderGraph *hgBuf = (HeaderGraph *)myBuf;
			clock_t begin = clock();
			std::sort(hgBuf, hgBuf + hgNum, custom_sort);
			clock_t end = clock();
			double elapsed = double(end - begin) / CLOCKS_PER_SEC;
			std::cout << "l = " << l << std::endl << "\nred= " << red;
			const std::string lStr = std::to_string(l);
			hOut = CreateFile(lStr.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			DWORD dw = GetLastError();
			std::cout << "General failure. To create, GetLastError returned " << std::hex
				<< dw << ".\n";
			cResult = WriteFile(hOut, hgBuf, MAX_BUF, &blue, NULL);
			DWORD dwl = GetLastError();
			std::cout << "General failure. GetLastError returned " << std::hex
				<< dwl << "\n";
			if (!cResult || blue == 0)
			{
				std::cout <<"\n"<< cResult << " " << blue << std::endl;
			}
			l++;
			CloseHandle(hOut);
		} while (bResult && red!=0);
		
		//merge using l
		clock_t begin = clock();
		merge_call(l);
		clock_t end = clock();
		double elapsed = double(end - begin) / CLOCKS_PER_SEC;
		std::cout << elapsed << std::endl;
		free(myBuf);
	} 
	std::cout << "\nDONE!\n";
	getchar();

}