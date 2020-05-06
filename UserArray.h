#ifndef USER_ARRAY_PAGE
#define USER_ARRAY_PAGE

#define CHUNK_SIZE 62
#define SINGLE_THREAD_LIMIT 300

#include <iostream>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>

template<class elementType>
class UserArray
{
public:
bool debugger;

UserArray(int newNum)
{
debugger = false;
if(debugger) std::cout << "Address of THIS object in Array constructor: " << this << " with an array size of " << newNum << std::endl;

size = newNum;
if(newNum < 0)
{
std::cout << "Error: Array size must be >= 0\n";
exit(1);
}
else if(newNum == 0)
internalArray = NULL;

else
{
internalArray = new elementType[newNum];
for(int i = 0; i < newNum; ++i)
internalArray[i] = 0;
}

numberOfReadingThreads = new std::atomic<int>(0);
isWriteWaiting = false;
writeLock = new std::mutex();
}

~UserArray()
{
delete numberOfReadingThreads;
delete writeLock;
if(internalArray != NULL)
{
delete [] internalArray;
internalArray = NULL;
}
}

bool contains(elementType myElement)
{
if(debugger) std::cout << "In contains for Array with THIS of " << this << std::endl;
writeLock->lock();
++(*numberOfReadingThreads);
for(int i = 0; i < size; ++i)
{
if(internalArray[i] == myElement)
{
--(*numberOfReadingThreads);
writeLock->unlock();
return true;
}
}
--(*numberOfReadingThreads);
writeLock->unlock();
return false;
}

int length()
{
if(debugger)std::cout << "in length() function of Array with THIS of " << this << std::endl;

return size;
}

void update(int varIndex, elementType myElement)
{
if(debugger)std::cout << "in update for Array with THIS of " << this << std::endl;
if(varIndex < 0 || varIndex >= size)
{
std::cout << "Error: in update function, varIndex must be >= 0 and < size. varIndex was " << varIndex << " and size was " << size << std::endl;
exit(1);
}
writeLock->lock();
internalArray[varIndex] = myElement;
writeLock->unlock();
return;
}

elementType at(int index)
{
if(debugger) std::cout << "in at for ARRAY with THIS of " << this << std::endl;
elementType returnResult;
if(index < 0 || index >= size)
{
std::cout << "Error: index of " << index << " was out of array bounds, which had size " << size << std::endl;
exit(1);
}
writeLock->lock();
returnResult = internalArray[index];
writeLock->unlock();
return returnResult;
}


int get(elementType myElement)
{
if(debugger) std::cout << "in get for ARRAY with THIS of " << this << std::endl;
writeLock->lock();

for(int i = 0; i < size; ++i)
{
if(internalArray[i] == myElement)
{
writeLock->unlock();
return i;
}
}
writeLock->unlock();
return -1;
}


void timsort(int startIndex, int endIndex)
{
if(endIndex - startIndex >= CHUNK_SIZE)
{
timsort(startIndex, (startIndex + endIndex)/2);
timsort( (startIndex + endIndex)/2 + 1, endIndex);
merge(startIndex, (startIndex + endIndex)/2, endIndex);
return;
}

//if we're down here, then the chunk we are in is smaller than CHUNK_SIZE
//as such, we perform insertion sort on the chunk down here.

elementType currentVal;
int j;
for(int i = startIndex + 1; i <= endIndex; ++i)
{
currentVal = internalArray[i];
j = i -1;

while(j >= startIndex && internalArray[j] > currentVal)
{
	internalArray[j + 1] = internalArray[j];
	j = j - 1;
}
internalArray[j + 1] = currentVal;
}

return;
}

void merge(int startIndex, int lastIndexOfFirstHalf, int endIndex)
{
int firstArrSize = lastIndexOfFirstHalf - startIndex + 1;
int secondArrSize = endIndex - lastIndexOfFirstHalf;
elementType* firstSubArray = new elementType[firstArrSize];
elementType* secondSubArray = new elementType[secondArrSize];


for(int i = 0; i < firstArrSize; ++i)
	firstSubArray[i] = internalArray[startIndex + i];

for(int i = 0; i < secondArrSize; ++i)
	secondSubArray[i] = internalArray[lastIndexOfFirstHalf + 1 + i];

int firstIndex = 0;
int secondIndex = 0;
int finalWriteIndex = startIndex;

while(firstIndex < firstArrSize && secondIndex < secondArrSize)
{
	if(firstSubArray[firstIndex] <= secondSubArray[secondIndex])
	{
		internalArray[finalWriteIndex] = firstSubArray[firstIndex];
		++finalWriteIndex;
		++firstIndex;
	}
	
	else
	{
		internalArray[finalWriteIndex] = secondSubArray[secondIndex];
		++finalWriteIndex;
		++secondIndex;
	}
} 

while(firstIndex < firstArrSize)
{
	internalArray[finalWriteIndex] = firstSubArray[firstIndex];
	++firstIndex;
	++finalWriteIndex;
}

while(secondIndex < secondArrSize)
{
	internalArray[finalWriteIndex] = secondSubArray[secondIndex];
	++secondIndex;
	++finalWriteIndex;
}



delete [] firstSubArray;
delete [] secondSubArray;
return;
}

void sort()
{
if(debugger) std::cout << "in sort() for ARRAY with THIS of " << this << std::endl;
if(size == 0)
return;

if(size < SINGLE_THREAD_LIMIT)
{
writeLock->lock();
timsort(0, size - 1);
writeLock->unlock();
return;
}
else
{
writeLock->lock();
std::thread otherThread(&UserArray::timsort, this, 0, size / 2);
timsort(size/2 + 1, size - 1);
otherThread.join();
merge(0, size / 2, size - 1);
writeLock->unlock();
return;
}

}

elementType* internalArray;
int size;
std::atomic<int>* numberOfReadingThreads;
std::mutex* writeLock;
bool isWriteWaiting;
};

#endif
