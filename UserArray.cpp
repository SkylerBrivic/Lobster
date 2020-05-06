#include "UserArray.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <sys/time.h>
double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

//runs 1000 tests with sorting arrays of size 100,000 with a chunk size of size, and returns the average runtime
double testChunkSize(int size)
{
const int ARRAY_SIZE = 100000;
double totalTime = 0.0;

double startTime, endTime;

for(int i = 0; i < 1000; ++i)
{
UserArray x = UserArray(ARRAY_SIZE);
	for(int j = 0; j < ARRAY_SIZE; ++j)
	{
	x.internalArray[j] = rand();
	}

startTime = get_wall_time();
x.sort();
endTime = get_wall_time();
totalTime += (endTime - startTime);
}

return totalTime / 1000;
}

int main()
{
srand(std::time(0));

//testing chunk sizes 10 - 500
int lowestChunkSize = -1;
double currentLowest = 1000;
double currentTime;

for(int i = 10; i <= 70; ++i)
{
currentTime = testChunkSize(i);
if(currentTime < currentLowest)
{
lowestChunkSize = i;
currentLowest = currentTime;
}

std::cout << "Chunk Size " << i << ": " << currentTime << " seconds\n";
}

std::cout << std::endl << "The shortest run time occured with a chunk size of " << lowestChunkSize << ", with an average runtime of " << currentLowest << "seconds\n";

return 0;
}

