#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <queue>
#include <string>
using namespace std;
#define     DRAM_SIZE       (64*1024*1024)
#define     CACHE_SIZE      (64*1024)
unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */
unsigned int rand_()
{
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + m_w;  /* 32-bit result */
}
unsigned int memGen1()
{
	static unsigned int addr = 0;
	return (addr += (16 * 1024)) % (256 * 1024);
}
unsigned int memGen2()
{
	static unsigned int addr = 0;
	return (addr += 4) % (DRAM_SIZE);
}
unsigned int memGen3()
{
	return rand_() % (256 * 1024);
}
void emptcache(int* cache, int nlines)
{
	for (int i = 0; i < nlines; i++)
		cache[i] = -1;
}
struct cache {
	int freq;
	int tag;
	int lruCounter;
	bool free;
};
void exp1(int cachesize, int blocksize)
{
	unsigned int addr;
	float ncalls = 1000000; //inits the number of time the cache is called
	float nhits[3] = { 0,0,0 }, nmisses[3] = { 0,0,0 }, hitratio[3];
	int tag; //tag of the block
	int index; //the index of the adress generated in the cache array 
	int nlines = cachesize / blocksize;
	int* cache = new int[nlines];
	emptcache(cache, nlines);
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen1();
		tag = addr / blocksize;
		index = (addr / blocksize) % nlines;
		if (cache[index] == tag)
			nhits[0]++;
		else
			cache[index] = tag;
	}
	hitratio[0] = nhits[0] / ncalls;
	//end of the memgen1 test 
	emptcache(cache, nlines); //reempt the cache by assigning -1 to all the slots 
	cout << "The hit ratio memgen1 for exp1 is: " << hitratio[0] << endl;
	for (int i = 0; i < 1000000; i++)
	{
		addr = memGen2();
		tag = addr / blocksize;
		index = (addr / blocksize) % nlines;
		if (cache[index] == tag)
			nhits[1]++;
		else
		{
			nmisses[1]++;
			cache[index] = tag;
		}
	}
	hitratio[1] = nhits[1] / ncalls;
	emptcache(cache, nlines);
	cout << "The hit ratio memgen 2 for exp1 is: " << hitratio[1] << endl;
	for (int i = 0; i < 1000000; i++)
	{
		addr = memGen3();
		tag = addr / blocksize;
		index = (addr / blocksize) % nlines;
		if (cache[index] == tag)
			nhits[2]++;
		else
		{
			nmisses[2]++;
			cache[index] = tag;
		}
	}
	hitratio[2] = nhits[2] / ncalls;
	emptcache(cache, nlines);
	cout << "The hit ratio memgen3 for exp 1 is: " << hitratio[2] << endl;
}
void exp2()
{
	//sim for a 1L cache 
	unsigned int addr;
	float ncalls = 1000000; //inits the number of time the cache is called
	int access_time1L = 0;
	int tag; //tag of the block
	int index; //the index of the adress generated in the cache array 
	int nlines = 256 * 1024 / 64;
	int* cache = new int[nlines];
	emptcache(cache, nlines);
	// ----START OF THE MEMGEN1 TEST FOR 1L CACHE----
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen1();
		tag = addr / 64;
		index = (addr / 64) % nlines;
		access_time1L += 11;
		if (cache[index] != tag)
		{
			cache[index] = tag;
			access_time1L += 100;
		}
	}
	cout << "the access time for a 1L cache using memgen1 is: " << access_time1L << endl;
	access_time1L = 0;
	emptcache(cache, nlines);
	// ----START OF THE MEMGEN2 TEST FOR 1L CACHE----
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen2();
		tag = addr / 64;
		index = (addr / 64) % nlines;
		access_time1L += 11;
		if (cache[index] != tag)
		{
			cache[index] = tag;
			access_time1L += 100;
		}
	}
	cout << "the access time for a 1L cache using memgen2 is: " << access_time1L << endl;
	access_time1L = 0;
	emptcache(cache, nlines);
	// ----START OF THE MEMGEN3 TEST FOR 1L CACHE----
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen3();
		tag = addr / 64;
		index = (addr / 64) % nlines;
		access_time1L += 11;
		if (cache[index] != tag)
		{
			cache[index] = tag;
			access_time1L += 100;
		}
	}
	cout << "the access time for a 1L cache using memgen3 is: " << access_time1L << endl;
	access_time1L = 0;
	emptcache(cache, nlines);
	//---------------------------------------------------------------------
	//end of sim for a 1L cache
	//START SIM FOR 2L CACHE
	// Start memgen1 loop
	int cacheL1[512];
	int cacheL2[4096];
	emptcache(cacheL1, 512);
	emptcache(cacheL2, 4096);
	int access_time2L = 0;
	int indexL1;
	int indexL2;
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen1();
		tag = addr / 64;
		indexL1 = (addr / 64) % 512;
		indexL2 = (addr / 64) % 4092;
		if (cacheL1[indexL1] == tag)
			access_time2L += 4;
		else if (cacheL1[indexL1] == -1)
		{
			access_time2L += 4;
			cacheL1[indexL1] = tag;
		}
		else
		{
			access_time2L += 15;
			if (cacheL2[indexL2] == tag)
				cacheL1[indexL1] = tag;
			else
			{
				access_time2L += 100;
				cacheL1[indexL1] = tag;
				cacheL2[indexL2] = tag;
			}
		}
	}
	cout << "The access time for a two level cache using memgen 1 is: " << access_time2L << endl;
	emptcache(cacheL1, 512);
	emptcache(cacheL2, 4096);
	access_time2L = 0;
	// Start memgen2 loop 
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen2();
		tag = addr / 64;
		indexL1 = (addr / 64) % 512;
		indexL2 = (addr / 64) % 4096;
		if (cacheL1[indexL1] == tag)
			access_time2L += 4;
		else if (cacheL1[indexL1] == -1)
		{
			access_time2L += 4;
			cacheL1[indexL1] = tag;
		}
		else
		{
			access_time2L += 15;
			if (cacheL2[indexL2] == tag)
				cacheL1[indexL1] = tag;
			else
			{
				access_time2L += 100;
				cacheL1[indexL1] = tag;
				cacheL2[indexL2] = tag;
			}
		}
	}
	cout << "The access time for a two level cache using memgen2 is: " << access_time2L << endl;
	emptcache(cacheL1, 512);
	emptcache(cacheL2, 4096);
	access_time2L = 0;
	// Start memgen3 loop 
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen3();
		tag = addr / 64;
		indexL1 = (addr / 64) % 512;
		indexL2 = (addr / 64) % 4096;
		if (cacheL1[indexL1] == tag)
			access_time2L += 4;
		else if (cacheL1[indexL1] == -1)
		{
			access_time2L += 4;
			cacheL1[indexL1] = tag;
		}
		else
		{
			access_time2L += 15;
			if (cacheL2[indexL2] == tag)
				cacheL1[indexL1] = tag;
			else
			{
				access_time2L += 100;
				cacheL1[indexL1] = tag;
				cacheL2[indexL2] = tag;
			}
		}
	}
	cout << "The access time for a two level cache using memgen 3 is: " << access_time2L << endl;
	emptcache(cacheL1, 512);
	emptcache(cacheL2, 4096);
	//START SIM FOR 3L CACHE
	// Start memgen1 loop
	int cacheL3[131072];
	emptcache(cacheL3, 4096);
	int indexL3;
	int access_time3L = 0;
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen1();
		tag = addr / 64;
		indexL1 = (addr / 64) % 512;
		indexL2 = (addr / 64) % 4096;
		indexL3 = (addr / 64) % 131072;
		if (cacheL1[indexL1] == tag)
			access_time3L += 4;
		else if (cacheL1[indexL1] == -1)
		{
			access_time3L += 4;
			cacheL1[indexL1] = tag;
		}
		else
		{
			access_time3L += 15;
			if (cacheL2[indexL2] == tag)
				cacheL1[indexL1] = tag;
			else
			{
				access_time3L += 30;
				if (cacheL3[indexL3] == tag)
				{
					cacheL1[indexL1] = tag;
					cacheL2[indexL2] = tag;
				}
				else
				{
					access_time3L += 100;
					cacheL1[indexL1] = tag;
					cacheL2[indexL2] = tag;
					cacheL3[indexL3] = tag;
				}
			}
		}
	}
	cout << "The access time for a three level cache using memgen 1 is: " << access_time3L << endl;
	emptcache(cacheL1, 512);
	emptcache(cacheL2, 4096);
	emptcache(cacheL3, 131072);
	access_time3L = 0;
	// // Start memgen2 loop
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen2();
		tag = addr / 64;
		indexL1 = (addr / 64) % 512;
		indexL2 = (addr / 64) % 4096;
		indexL3 = (addr / 64) % 131072;
		if (cacheL1[indexL1] == tag)
			access_time3L += 4;
		else if (cacheL1[indexL1] == -1)
		{
			access_time3L += 4;
			cacheL1[indexL1] = tag;
		}
		else
		{
			access_time3L += 15;
			if (cacheL2[indexL2] == tag)
				cacheL1[indexL1] = tag;
			else
			{
				access_time3L += 30;
				if (cacheL3[indexL3] == tag)
				{
					cacheL1[indexL1] = tag;
					cacheL2[indexL2] = tag;
				}
				else
				{
					access_time3L += 100;
					cacheL1[indexL1] = tag;
					cacheL2[indexL2] = tag;
					cacheL3[indexL3] = tag;
				}
			}
		}
	}
	cout << "The access time for a three level cache using memgen 2 is: " << access_time3L << endl;
	emptcache(cacheL1, 512);
	emptcache(cacheL2, 4096);
	emptcache(cacheL3, 131072);
	access_time3L = 0;
	// // Start memgen2 loop
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen3();
		tag = addr / 64;
		indexL1 = (addr / 64) % 512;
		indexL2 = (addr / 64) % 4096;
		indexL3 = (addr / 64) % 131072;
		if (cacheL1[indexL1] == tag)
			access_time3L += 4;
		else if (cacheL1[indexL1] == -1)
		{
			access_time3L += 4;
			cacheL1[indexL1] = tag;
		}
		else
		{
			access_time3L += 15;
			if (cacheL2[indexL2] == tag)
				cacheL1[indexL1] = tag;
			else
			{
				access_time3L += 30;
				if (cacheL3[indexL3] == tag)
				{
					cacheL1[indexL1] = tag;
					cacheL2[indexL2] = tag;
				}
				else
				{
					access_time3L += 100;
					cacheL1[indexL1] = tag;
					cacheL2[indexL2] = tag;
					cacheL3[indexL3] = tag;
				}
			}
		}
	}
	cout << "The access time for a three level cache using memgen 3 is: " << access_time3L << endl;
	emptcache(cacheL1, 512);
	emptcache(cacheL2, 4096);
	emptcache(cacheL3, 131072);
	access_time3L = 0;
}
void exp3(int cacheSize, int hitCounter[], float hitRatio[])
{
	hitCounter[0] = 0;
	int address, tag, mru, noOfBlocks = cacheSize / 16, nCalls = 1000000;
	int *cache = new int[noOfBlocks];
	emptcache(cache, noOfBlocks);
	for (int i = 0; i < nCalls; i++)
	{
		address = memGen1();
		tag = address >> 4;
		for (int i = 0; i < noOfBlocks; i++)
		{
			if (cache[i] == tag)
			{
				mru = i;
				hitCounter[0]++;
				break;
			}
			else if (cache[i] == -1)
			{
				mru = i;
				cache[i] = tag;
				break;
			}
		}
		cache[mru] = tag;
	}
	hitRatio[0] = float(hitCounter[0]) / float(nCalls);
	emptcache(cache, noOfBlocks);
	hitCounter[1] = 0;
	for (int i = 0; i < nCalls; i++)
	{
		address = memGen2();
		tag = address >> 4;
		for (int i = 0; i < noOfBlocks; i++)
		{
			if (cache[i] == tag)
			{
				mru = i;
				hitCounter[1]++;
				break;
			}
			else if (cache[i] == -1)
			{
				mru = i;
				cache[i] = tag;
				break;
			}
		}
		cache[mru] = tag;
	}
	hitRatio[1] = float(hitCounter[1]) / float(nCalls);
	emptcache(cache, noOfBlocks);
	hitCounter[2] = 0;
	for (int i = 0; i < nCalls; i++)
	{
		address = memGen3();
		tag = address >> 4;
		for (int i = 0; i < noOfBlocks; i++)
		{
			if (cache[i] == tag)
			{
				mru = i;
				hitCounter[2]++;
				break;
			}
			else if (cache[i] == -1)
			{
				mru = i;
				cache[i] = tag;
				break;
			}
		}
		cache[mru] = tag;
	}
	hitRatio[2] = float(hitCounter[2]) / float(nCalls);
	return;
}
void exp4(int mode)
{
	string replacement = "";
	if (mode == 1)
		replacement = "Random";
	else if (mode == 2)
		replacement = "LRU";
	else if (mode == 3)
		replacement = "LFU";
	else if (mode == 4)
		replacement = "Adaptive LRU/LFU";
	cache cache[8192];
	int lru_item, maxLRUcount, maxLRUindex; //variable for the index of the line that is last used 
	int hits = 0, ncalls = 1000000, addr, tag, index, nlines = 8192, n_insertions = 0, tc = 0;
	bool out = false;
	bool full = true;
	int min_freq_index; //int min_freq_index = 0;
	int min_freq;		//int min_freq = 0;
	//memgen 1 loop
	for (int i = 0; i < nlines; i++)
	{
		cache[i].free = true;
		cache[i].tag = -1;
		cache[i].freq = 0; 
		cache[i].lruCounter = 0;
	}
	for (int i = 0; i < ncalls; i++)
	{
		//for lru replacement, we increment all elements' counter here, reset it to zero later if it is free or there's a hit
		for (int y = 0; y < nlines; y++)
			cache[y].lruCounter++;
		addr = memGen1();
		tag = addr / 32;
		out = false;
		full = true;
		for (int j = 0; j < 8192; j++)
			if (cache[j].free)
			{
				full = false;
				break;
			}
		for (int k = 0; k < 8192; k++)
			if (cache[k].tag == tag)
			{
				hits++;
				cache[k].lruCounter = 0;
				cache[k].freq++;
				out = true;
				break;
			}
		if (out)
			continue;
		if (full)
		{
			switch (mode)
			{
			case 1:			//random overwriting
				int pos;	//the index of the cache line to overwrite
				srand(time(0));
				pos = rand() % nlines;
				cache[pos].tag = tag;
				cache[pos].freq = 1;
				break;
			case 2:			//LRU policy
				maxLRUcount = cache[0].lruCounter;
				maxLRUindex = 0;
				for (int y = 1; y < nlines; y++)
					if (cache[y].lruCounter > maxLRUcount)
					{
						maxLRUcount = cache[y].lruCounter;
						maxLRUindex = y;
					}
				cache[maxLRUindex].tag = tag;
				cache[maxLRUindex].lruCounter++;
				break;
			case 3:			//LFU policy
				for (int i = 0; i < nlines; i++)
					if (cache[i].freq < min_freq)
					{
						min_freq_index = i;
						min_freq = cache[i].freq;
					}
				cache[min_freq_index].tag = tag;
				cache[min_freq_index].freq = 1;
				break;
			case 4:
			{				//adaptive
				int adaptive_ncalls = 0;
				int case4_mode = 0;
				int period_check = ncalls / 100;
				switch (case4_mode)
				{
				case 0:
					maxLRUcount = cache[0].lruCounter;
					maxLRUindex = 0;
					for (int y = 1; y < nlines; y++)
						if (cache[y].lruCounter > maxLRUcount)
						{
							maxLRUcount = cache[y].lruCounter;
							maxLRUindex = y;
						}
					cache[maxLRUindex].tag = tag;
					cache[maxLRUindex].lruCounter++;
					adaptive_ncalls++;
					break;
				case 1:
				{
					for (int i = 0; i < nlines; i++)
						if (cache[i].freq < min_freq)
						{
							min_freq_index = i;
							min_freq = cache[i].freq;
						}
					cache[min_freq_index].tag = tag;
					cache[min_freq_index].freq = 1;
					adaptive_ncalls++;
					break;
				}
				}
				if (adaptive_ncalls > period_check)
				{
					if (float(hits) / i < 0.3)
					{
						if (case4_mode == 0)
							case4_mode = 1;
						else
							case4_mode = 1;
					}
					adaptive_ncalls = 0;
				}
			}
			break;
			default:
				break;
			}
		}
		else
			for (int m = 0; m < nlines; m++)
				if (cache[m].free)
				{
					cache[m].tag = tag;
					cache[m].lruCounter = 0;
					cache[m].freq = 1;
					cache[m].free = false;
					tc++;
					break;
				}
	}
	cout << "The hit ratio for experiment 4 for " << replacement << " replacement using memgen1 is: " << float(hits) / ncalls << endl;
	for (int i = 0; i < nlines; i++)
	{			//emptying the cache
		cache[i].free = true;
		cache[i].tag = -1;
		cache[i].freq = 0;
		cache[i].lruCounter = 0;
	}
	hits = 0;
	out = false;
	full = true;
	min_freq = ncalls + 1;
	// start memgen 2
	for (int i = 0; i < ncalls; i++)
	{
		//for lru replacement, we increment all elements' counter here, reset it to zero later if it is free or there's a hit
		for (int y = 0; y < nlines; y++)
			cache[y].lruCounter++;
		addr = memGen2();
		tag = addr / 32;
		out = false;
		full = true;
		for (int j = 0; j < 8192; j++)
			if (cache[j].free)
			{
				full = false;
				break;
			}
		for (int k = 0; k < 8192; k++)
			if (cache[k].tag == tag)
			{
				hits++;
				cache[k].lruCounter = 0;
				cache[k].freq++;
				out = true;
				break;
			}
		if (out)
			continue;
		if (full)
		{
			switch (mode)
			{
			case 1:			//random overwriting
				int pos;	//the index of the cache line to overwrite
				srand(time(0));
				pos = rand() % nlines;
				cache[pos].tag = tag;
				cache[pos].freq = 1;
				break;
			case 2:			//LRU policy
				maxLRUcount = cache[0].lruCounter;
				maxLRUindex = 0;
				for (int y = 1; y < nlines; y++)
					if (cache[y].lruCounter > maxLRUcount)
					{
						maxLRUcount = cache[y].lruCounter;
						maxLRUindex = y;
					}
				cache[maxLRUindex].tag = tag;
				cache[maxLRUindex].lruCounter++;
				break;
			case 3:			//LFU policy
				for (int i = 0; i < nlines; i++)
					if (cache[i].freq < min_freq)
					{
						min_freq_index = i;
						min_freq = cache[i].freq;
					}
				cache[min_freq_index].tag = tag;
				cache[min_freq_index].freq = 1;
				break;
			case 4:
			{				//adaptive
				int adaptive_ncalls = 0;
				int case4_mode = 0;
				int period_check = ncalls / 100;
				switch (case4_mode)
				{
				case 0:
					maxLRUcount = cache[0].lruCounter;
					maxLRUindex = 0;
					for (int y = 1; y < nlines; y++)
						if (cache[y].lruCounter > maxLRUcount)
						{
							maxLRUcount = cache[y].lruCounter;
							maxLRUindex = y;
						}
					cache[maxLRUindex].tag = tag;
					cache[maxLRUindex].lruCounter++;
					adaptive_ncalls++;
					break;
				case 1:
				{
					for (int i = 0; i < nlines; i++)
						if (cache[i].freq < min_freq)
						{
							min_freq_index = i;
							min_freq = cache[i].freq;
						}
					cache[min_freq_index].tag = tag;
					cache[min_freq_index].freq = 1;
					adaptive_ncalls++;
					break;
				}
				}
				if (adaptive_ncalls > period_check)
				{
					if (float(hits) / i < 0.3)
					{
						if (case4_mode == 0)
							case4_mode = 1;
						else
							case4_mode = 1;
					}
					adaptive_ncalls = 0;
				}
			}
			break;
			default:
				break;
			}
		}
		else
			for (int m = 0; m < nlines; m++)
				if (cache[m].free)
				{
					cache[m].tag = tag;
					cache[m].lruCounter = 0;
					cache[m].freq = 1;
					cache[m].free = false;
					tc++;
					break;
				}
	}
	cout << "The hit ratio for experiment 4 for " << replacement << " replacement using memgen2 is: " << float(hits) / ncalls << endl;
	for (int i = 0; i < nlines; i++)
	{			//emptying the cache
		cache[i].free = true;
		cache[i].tag = -1;
		cache[i].freq = 0;
		cache[i].lruCounter = 0;
	}
	hits = 0;
	out = false;
	full = true;
	min_freq = ncalls + 1;
	//start memgen 3
	for (int i = 0; i < ncalls; i++)
	{
		addr = memGen3();
		tag = addr / 32;
		out = false;
		full = true;
		for (int j = 0; j < 8192; j++)
			if (cache[j].free)
			{
				full = false;
				break;
			}
		for (int k = 0; k < 8192; k++)
			if (cache[k].tag == tag)
			{
				hits++;
				cache[k].lruCounter = 0;
				cache[k].freq++;
				out = true;
				break;
			}
		if (out)
			continue;
		if (full)
		{
			switch (mode)
			{
			case 1:			//random overwriting
				int pos;	//the index of the cache line to overwrite
				srand(time(0));
				pos = rand() % nlines;
				cache[pos].tag = tag;
				cache[pos].freq = 1;
				break;
			case 2:			//LRU policy
				maxLRUcount = cache[0].lruCounter;
				maxLRUindex = 0;
				for (int y = 1; y < nlines; y++)
					if (cache[y].lruCounter > maxLRUcount)
					{
						maxLRUcount = cache[y].lruCounter;
						maxLRUindex = y;
					}
				cache[maxLRUindex].tag = tag;
				cache[maxLRUindex].lruCounter++;
				break;
			case 3:			//LFU policy
				for (int i = 0; i < nlines; i++)
					if (cache[i].freq < min_freq)
					{
						min_freq_index = i;
						min_freq = cache[i].freq;
					}
				cache[min_freq_index].tag = tag;
				cache[min_freq_index].freq = 1;
				break;
			case 4:
			{			//adaptive
				int adaptive_ncalls = 0;
				int case4_mode = 0;
				int period_check = ncalls / 100;
				switch (case4_mode)
				{
				case 0:
					maxLRUcount = cache[0].lruCounter;
					maxLRUindex = 0;
					for (int y = 1; y < nlines; y++)
						if (cache[y].lruCounter > maxLRUcount)
						{
							maxLRUcount = cache[y].lruCounter;
							maxLRUindex = y;
						}
					cache[maxLRUindex].tag = tag;
					cache[maxLRUindex].lruCounter++;
					adaptive_ncalls++;
					break;
				case 1:
				{
					for (int i = 0; i < nlines; i++)
						if (cache[i].freq < min_freq)
						{
							min_freq_index = i;
							min_freq = cache[i].freq;
						}
					cache[min_freq_index].tag = tag;
					cache[min_freq_index].freq = 1;
					adaptive_ncalls++;
					break;
				}
				}
				if (adaptive_ncalls > period_check)
				{
					if (float(hits) / i < 0.3)
					{
						if (case4_mode == 0)
							case4_mode = 1;
						else
							case4_mode = 1;
					}
					adaptive_ncalls = 0;
				}
			}
			break;
			default:
				break;
			}
		}
		else
		{
			for (int m = 0; m < nlines; m++)
				if (cache[m].free)
				{
					cache[m].tag = tag;
					cache[m].lruCounter = 0;
					cache[m].freq = 1;
					cache[m].free = false;
					tc++;
					break;
				}
		}
	}
	cout << "The hit ratio for experiment 4  for " << replacement << " replacement using memgen3 is: " << float(hits) / ncalls << endl;
	for (int i = 0; i < nlines; i++)
	{			//emptying the cache
		cache[i].free = true;
		cache[i].tag = -1;
		cache[i].freq = 0;
		cache[i].lruCounter = 0;
	}
	hits = 0;
	out = false;
	full = true;
	min_freq = ncalls + 1;
}
void exp5(int noOfWays, int hitCounter[], float hitRatio[])
{
	//Study how the hit ratio changes with the number of ways : 2, 4, 8 and 16
	//(block size : 8 bytes, cache size : 16KB, FIFO(First In First Out) replacement).
	//int *cache = new int[noOfBlocks];
	int address, tag, blockOffset, blockAddress, setIndex, nCalls = 1000000;
	int cacheSize = 16384, noOfBlocks = cacheSize / 8; // = 2048
	int noOfSets = noOfBlocks / noOfWays, blocksPerSet = noOfBlocks / noOfSets;
	int counter = 0;
	bool there = false;
	queue<int> cacheQ;
	//cout << "CacheSize: " << cacheSize << "\nnoOfBlocks: " << noOfBlocks <<
		//"\nnoOfWays: " << noOfWays << "\nSets: " << noOfSets << "\nblocksPerSet: " << blocksPerSet << endl << endl;
	int** sets = new int*[noOfSets];
	for (int i = 0; i < noOfSets; ++i)
		sets[i] = new int[blocksPerSet];
	for (int i = 0; i < noOfSets; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets[i][j] = -1;
	hitCounter[0] = 0;
	for (int i = 0; i < nCalls; i++)
	{
		/*address = memGen1();
		blockOffset = address % 8;
		blockAddress = address / 8;
		setIndex = (blockAddress) % noOfSets;
		tag = blockAddress / setIndex; */
		address = memGen1();
		blockOffset = address % 8;
		//cout << "Address: " << address << "\nBlockOffset: " << blockOffset;
		address = address >> 3;
		setIndex = address % noOfSets;
		//cout << "\nAddress: " << address << "\nsetIndex: " << setIndex;
		tag = address >> int(log2(noOfSets));
		there = false;
		//cout << "\ntag: " << tag << "\n";
		//cout << "outer i: " << i << endl;
		for (int j = 0; j < blocksPerSet; j++)
		{
			//cout << "inner i: " << i << endl;
			if (sets[setIndex][j] == tag)
			{
				hitCounter[0]++;
				break;
				there = true;
			}
			else if (sets[setIndex][j] == -1)
			{
				sets[setIndex][j] = tag;
				there = true;
				cacheQ.push(j);
				break;
			}
		}
		if (!there)
		{
			//cout << "\n" << counter++ << " cache front is: " << cacheQ.front() << "\n";
			sets[setIndex][cacheQ.front()] = tag;
			cacheQ.push(cacheQ.front());
			cacheQ.pop();
		}
	}
	hitRatio[0] = float(hitCounter[0]) / float(nCalls);

	while (!cacheQ.empty())
		cacheQ.pop();
	for (int i = 0; i < noOfSets; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets[i][j] = -1;
	hitCounter[1] = 0;
	for (int i = 0; i < nCalls; i++)
	{
		address = memGen2();
		blockOffset = address % 8;
		//cout << "Address: " << address << "\nBlockOffset: " << blockOffset;
		address = address >> 3;
		setIndex = address % noOfSets;
		//cout << "\nAddress: " << address << "\nsetIndex: " << setIndex;
		tag = address >> int(log2(noOfSets));
		//cout << "\ntag: " << tag << "\n";
		//cout << "outer i: " << i << endl;
		there = false;
		for (int i = 0; i < blocksPerSet; i++)
		{
			//cout << "inner i: " << i << endl;
			if (sets[setIndex][i] == tag)
			{
				hitCounter[1]++;
				there = true;
				break;
			}
			else if (sets[setIndex][i] == -1)
			{
				sets[setIndex][i] = tag;
				there = true;
				cacheQ.push(i);
				break;
			}
		}
		if (!there)
		{
			sets[setIndex][cacheQ.front()] = tag;
			cacheQ.push(cacheQ.front());
			cacheQ.pop();
		}
	}
	hitRatio[1] = float(hitCounter[1]) / float(nCalls);

	while (!cacheQ.empty())
		cacheQ.pop();
	for (int i = 0; i < noOfSets; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets[i][j] = -1;
	hitCounter[2] = 0;
	for (int i = 0; i < nCalls; i++)
	{
		address = memGen3();
		blockOffset = address % 8;
		address = address >> 3;
		setIndex = address % noOfSets;
		tag = address >> int(log2(noOfSets));
		for (int i = 0; i < blocksPerSet; i++)
		{
			if (sets[setIndex][i] == tag)
			{
				hitCounter[2]++;
				there = true;
				break;
			}
			else if (sets[setIndex][i] == -1)
			{
				sets[setIndex][i] = tag;
				there = true;
				cacheQ.push(i);
				break;
			}
		}
		if (!there)
		{
			sets[setIndex][cacheQ.front()] = tag;
			cacheQ.push(cacheQ.front());
			cacheQ.pop();
		}
	}
	hitRatio[2] = float(hitCounter[2]) / float(nCalls);
	return;
}
void exp6()
{
	//int cacheSize = 262144, noOfBlocks = 32768;
	//int nlines = 256 * 1024 / 64, noOfSets = nlines / 8, blocksPerSet = nlines / noOfSets;
	//int nlines = 4096, noOfSets = 512, blocksPerSet = 8, cacheSize = 262144;
	//int counter = 0;
	int addr[3], tag[3], blockOffset[3], blockAddress[3], setIndex[3], ncalls = 1000000;
	int cacheSize[3] = { 256 * 1024 }, noOfBlocks[3] = { cacheSize[0] / 64 };
	int noOfSets[3] = { noOfBlocks[0] / 8 }, blocksPerSet = 8, counter = 0;
	int access_time1L = 0, access_time2L = 0, access_time3L = 0, index;
	bool found = false;
	//sim for a 1L cache 
	//---------------------------------------------------------------------
	int** sets = new int*[noOfSets[0]];
	for (int i = 0; i < noOfSets[0]; ++i)
		sets[i] = new int[blocksPerSet];
	for (int i = 0; i < noOfSets[0]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets[i][j] = -1;
	// ----START OF THE MEMGEN1 TEST FOR 1L CACHE----
	for (int i = 0; i < ncalls; i++)
	{
		addr[0] = memGen1();
		blockOffset[0] = addr[0] % 64;
		addr[0] = addr[0] >> 6;
		setIndex[0] = addr[0] % noOfSets[0];
		tag[0] = addr[0] >> int(log2(noOfSets[0]));
		access_time1L += 11;
		int j;
		for (j = 0; j < blocksPerSet; j++)
			if (sets[setIndex[0]][j] == -1)
			{
				sets[setIndex[0]][j] = tag[0];
				access_time1L += 100;
			}
		/*if (sets[setIndex[0]][j] == -1)
			{
				sets[setIndex[0]][j] = tag[0];
				access_time1L += 11;
				break;
			}
			else
			{
				sets[setIndex[0]][j] = tag[0];
				access_time1L += 100;
			}
			if (sets[setIndex[0]][j] == -1 || sets[setIndex[0]][j] == tag[0])
			{
				access_time1L += 11;
				sets[setIndex[0]][j] = tag[0];
				found = true;
				break;
			}
		}
		if (!found)
		{
			access_time1L += 100;
			sets[setIndex[0]][j] = tag[0];
		} */
	}
	cout << "The access time for a 1L cache using memgen1 is: " << access_time1L << endl;
	access_time1L = 0;
	for (int i = 0; i < noOfSets[0]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets[i][j] = -1;
	// ----START OF THE MEMGEN2 TEST FOR 1L CACHE----
	for (int i = 0; i < ncalls; i++)
	{
		addr[0] = memGen2();
		blockOffset[0] = addr[0] % 64;
		addr[0] = addr[0] >> 6;
		setIndex[0] = addr[0] % noOfSets[0];
		tag[0] = addr[0] >> int(log2(noOfSets[0]));
		access_time1L += 11;
		for (int j = 0; j < blocksPerSet; j++)
			if (sets[setIndex[0]][j] == -1)
			{
				sets[setIndex[0]][j] = tag[0];
				access_time1L += 100;
			}
	}
	cout << "The access time for a 1L cache using memgen2 is: " << access_time1L << endl;
	access_time1L = 0;
	for (int i = 0; i < noOfSets[0]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets[i][j] = -1;
	// ----START OF THE MEMGEN3 TEST FOR 1L CACHE----
	for (int i = 0; i < ncalls; i++)
	{
		addr[0] = memGen3();
		blockOffset[0] = addr[0] % 64;
		addr[0] = addr[0] >> 6;
		setIndex[0] = addr[0] % noOfSets[0];
		tag[0] = addr[0] >> int(log2(noOfSets[0]));
		access_time1L += 11;
		for (int j = 0; j < blocksPerSet; j++)
			if (sets[setIndex[0]][j] == -1)
			{
				sets[setIndex[0]][j] = tag[0];
				access_time1L += 100;
			}
	}
	cout << "The access time for a 1L cache using memgen3 is: " << access_time1L << endl;
	//---------------------------------------------------------------------
	//end of sim for a 1L cache
	//START SIM FOR 2L CACHE
	// Start memgen1 loop
	//	sets_1	->	L1	&&	sets ->	 L2
	for (int i = 0; i < noOfSets[0]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets[i][j] = -1;
	cacheSize[1] = 32 * 1024;
	noOfBlocks[1] = cacheSize[1] / 64;
	noOfSets[1] = noOfBlocks[1] / 8;
	int** sets_1 = new int*[noOfSets[1]];
	for (int i = 0; i < noOfSets[1]; ++i)
		sets_1[i] = new int[blocksPerSet];
	for (int i = 0; i < noOfSets[1]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets_1[i][j] = -1;
	for (int i = 0; i < ncalls; i++)
	{
		addr[1] = memGen1();
		blockOffset[1] = addr[1] % 64;
		addr[1] = addr[1] >> 6;
		setIndex[1] = addr[1] % noOfSets[1];
		tag[1] = addr[1] >> int(log2(noOfSets[1]));
		for (int j = 0; j < blocksPerSet; j++)
		{
			if (sets_1[setIndex[1]][j] == tag[1])
				access_time2L += 4;
			else if (sets_1[setIndex[1]][j] == -1)
			{
				sets_1[setIndex[1]][j] == tag[1];
				access_time2L += 4;
			}
			else
			{
				access_time2L += 15;
				for (int j = 0; j < blocksPerSet; j++)
				{
					if (sets[setIndex[0]][j] == tag[1])
						sets_1[setIndex[1]][j] = tag[1];
					else
					{
						access_time2L += 100;
						sets_1[setIndex[1]][j] = tag[1];
						sets[setIndex[0]][j] = tag[1];
					}
				}
			}
		}
	}
	cout << "The access time for a two level cache using memgen 1 is: " << access_time2L << endl;
	for (int i = 0; i < noOfSets[1]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets_1[i][j] = -1;
	for (int i = 0; i < noOfSets[0]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets[i][j] = -1;
	access_time2L = 0;
	// Start memgen2 loop
	//	sets_1	->	L1	&&	sets ->	 L2
	cacheSize[1] = 32 * 1024;
	noOfBlocks[1] = cacheSize[1] / 64;
	noOfSets[1] = noOfBlocks[1] / 8;
	for (int i = 0; i < ncalls; i++)
	{
		addr[1] = memGen2();
		blockOffset[1] = addr[1] % 64;
		addr[1] = addr[1] >> 6;
		setIndex[1] = addr[1] % noOfSets[1];
		tag[1] = addr[1] >> int(log2(noOfSets[1]));
		for (int j = 0; j < blocksPerSet; j++)
		{
			if (sets_1[setIndex[1]][j] == tag[1])
				access_time2L += 4;
			else if (sets_1[setIndex[1]][j] == -1)
			{
				sets_1[setIndex[1]][j] == tag[1];
				access_time2L += 4;
			}
			else
			{
				access_time2L += 15;
				for (int j = 0; j < blocksPerSet; j++)
				{
					if (sets[setIndex[0]][j] == tag[1])
						sets_1[setIndex[1]][j] = tag[1];
					else
					{
						access_time2L += 100;
						sets_1[setIndex[1]][j] = tag[1];
						sets[setIndex[0]][j] = tag[1];
					}
				}
			}
		}
	}
	cout << "The access time for a two level cache using memgen 2 is: " << access_time2L << endl;
	for (int i = 0; i < noOfSets[1]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets_1[i][j] = -1;
	for (int i = 0; i < noOfSets[0]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets[i][j] = -1;
	access_time2L = 0;
	// Start memgen1 loop
	//	sets_1	->	L1	&&	sets ->	 L2
	cacheSize[1] = 32 * 1024;
	noOfBlocks[1] = cacheSize[1] / 64;
	noOfSets[1] = noOfBlocks[1] / 8;
	for (int i = 0; i < ncalls; i++)
	{
		addr[1] = memGen3();
		blockOffset[1] = addr[1] % 64;
		addr[1] = addr[1] >> 6;
		setIndex[1] = addr[1] % noOfSets[1];
		tag[1] = addr[1] >> int(log2(noOfSets[1]));
		for (int j = 0; j < blocksPerSet; j++)
		{
			if (sets_1[setIndex[1]][j] == tag[1])
				access_time2L += 4;
			else if (sets_1[setIndex[1]][j] == -1)
			{
				sets_1[setIndex[1]][j] == tag[1];
				access_time2L += 4;
			}
			else
			{
				access_time2L += 15;
				for (int j = 0; j < blocksPerSet; j++)
				{
					if (sets[setIndex[0]][j] == tag[1])
						sets_1[setIndex[1]][j] = tag[1];
					else
					{
						access_time2L += 100;
						sets_1[setIndex[1]][j] = tag[1];
						sets[setIndex[0]][j] = tag[1];
					}
				}
			}
		}
	}
	cout << "The access time for a two level cache using memgen 3 is: " << access_time2L << endl;
	for (int i = 0; i < noOfSets[1]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets_1[i][j] = -1;
	for (int i = 0; i < noOfSets[0]; i++)
		for (int j = 0; j < blocksPerSet; j++)
			sets[i][j] = -1;
	access_time2L = 0;
}
int main()
{
	cout << "Exp1:\n";
	exp1(64 * 1024, 4);
	exp1(64 * 1024, 8);
	exp1(64 * 1024, 16);
	exp1(64 * 1024, 32);
	exp1(64 * 1024, 64);
	exp1(64 * 1024, 128);
	cout << "\nExp2:\n";
	exp2();
	int noOfHits[3] = { 0, 0, 0 };
	float hitRatio[3] = { 0.0, 0.0, 0.0 };
	cout << "\nExp3:\n";
	for (int i = 4; i < 65; i = i * 2)
	{
		exp3(i * 1024, noOfHits, hitRatio);
		cout << "Hit ratio for memGen1 for cache size " << i << " KB is " << hitRatio[0] << " with " << noOfHits[0] << " hits." << endl;
		cout << "Hit ratio for memGen2 for cache size " << i << " KB is " << hitRatio[1] << " with " << noOfHits[1] << " hits." << endl;
		cout << "Hit ratio for memGen3 for cache size " << i << " KB is " << hitRatio[2] << " with " << noOfHits[2] << " hits." << endl << endl;
	}
	noOfHits[0] = 0;
	noOfHits[1] = 0;
	noOfHits[2] = 0;
	hitRatio[0] = 0.0;
	hitRatio[1] = 0.0;
	hitRatio[2] = 0.0;
	cout << "\nExp4:\n";
	exp4(1);
	exp4(2);
	exp4(3);
	exp4(4);
	cout << "\nExp5:\n";
	for (int way = 2; way < 17; way *= 2)
	{
		exp5(way, noOfHits, hitRatio);
		cout << "Hit ratio for memGen1 for " << way << " ways is " << hitRatio[0] << " with " << noOfHits[0] << " hits." << endl;
		cout << "Hit ratio for memGen2 for " << way << " ways is " << hitRatio[1] << " with " << noOfHits[1] << " hits." << endl;
		cout << "Hit ratio for memGen3 for " << way << " ways is " << hitRatio[2] << " with " << noOfHits[2] << " hits." << endl << endl;
	}
	cout << "\nExp6:\n";
	exp6();
	system("pause");
	return 0;
}
