#include <atomic>
#include <mutex>

class UserQueue
{
public:
//this class uses an implementation of the Michael and Scott Concurrent Queue to ensure fast multi-threading
class QueueNode
{
public:

QueueNode()
{
valPointer = NULL;
nextPointer = NULL;
}

QueueNode(void* newVal, QueueNode* newNext)
{
valPointer = newVal;
nextPointer = newNext;
}

void* valPointer;
QueueNode* nextPointer;
};

UserQueue()
{
headLock = new std::mutex();
tailLock = new std::mutex();
headPointer = tailPointer = new QueueNode();
size = 0;
}

~UserQueue()
{
QueueNode* travel = headPointer;

while(travel != NULL)
{
headPointer = headPointer->nextPointer;
delete travel;
travel = headPointer;
}

headPointer = tailPointer = NULL;
delete headLock;
delete tailLock;
size = 0;
}

void push(void* newVal)
{
QueueNode* temp = new QueueNode(newVal, NULL);
tailLock->lock();
tailPointer->nextPointer = temp;
tailPointer = temp;
++size;
tailLock->unlock();
}

void* pop()
{
QueueNode* oldHead;
void* returnVal;
headLock->lock();
oldHead = headPointer;
QueueNode* newHead = headPointer->nextPointer;

if(newHead == NULL)
{
headLock->unlock();
return NULL;
}

returnVal = newHead->valPointer;
headPointer = newHead;
--size;
headLock->unlock();
delete oldHead;
return returnVal;
}

void* front()
{
void* returnVal;
headLock->lock();
if(headPointer->nextPointer == NULL)
{
headLock->unlock();
return NULL;
}
returnVal = headPointer->nextPointer->valPointer;
headLock->unlock();
return returnVal;
}

int length()
{
return size;
}

std::mutex* headLock;
std::mutex* tailLock;
QueueNode* headPointer;
QueueNode* tailPointer;
std::atomic<int> size;
};
