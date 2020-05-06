//GET, PUT, FIND, CONTAINS, AT, SORT, LENGTH, UPDATE
class UserList
{
public:

class Node
{
public:
Node()
{
valPointer = NULL;
nextPointer = NULL;
}

Node(void* val, Node* next)
{
valPointer = val;
nextPointer = next;
}

void* valPointer;
Node* nextPointer;
};

UserList()
{
headPointer = tailPointer = NULL;
size = 0;
memLock = new std::mutex();
}

~UserList()
{
Node* travel = headPointer;

while(travel != NULL)
{
travel = headPointer->nextPointer;
delete headPointer;
headPointer = travel;
}
headPointer = tailPointer = NULL;
delete memLock;
}

int length()
{
return size;
}

void put(int index, void* otherVal)
{
std::cout << "Error: put function is undefined for Lists\n";
exit(1);
}

void sort()
{
std::cout << "Error: sort function is undefined for Lists\n";
exit(1);
}

bool contains(void* otherVal)
{
memLock->lock();
Node* travel = headPointer;
while(travel != NULL)
{
if(travel->valPointer == otherVal)
{
memLock->unlock();
return true;
}
travel = travel->nextPointer;
}

memLock->unlock();
return false;
}

void* at(int index)
{
Node* travel;
void* returnVal;
int tempIndex = 0;
memLock->lock();
if(index < 0 || index >= size)
{
std::cout << "Error: Index of " << index << " was out of bounds for AT function in List with size of " << size << std::endl;
memLock->unlock();
exit(1);
}
travel = headPointer;
while(tempIndex < index)
{
travel = travel->nextPointer;
++tempIndex;
}
returnVal = travel->valPointer;
memLock->unlock();
return returnVal;
}

int get(void* otherVal)
{
int index = 0;
memLock->lock();
Node* travel = headPointer;
while(travel != NULL)
{
if(travel->valPointer == otherVal)
{
memLock->unlock();
return index;
}
++index;
travel = travel->nextPointer;
}
memLock->unlock();
return -1;
}

void push(void* newVal)
{
memLock->lock();
if(headPointer == NULL)
{
headPointer = tailPointer = new Node(newVal, NULL);
++size;
memLock->unlock();
return;
}

tailPointer->nextPointer = new Node(newVal, NULL);
++size;
tailPointer = tailPointer->nextPointer;
memLock->unlock();
return;
}

void* pop()
{
void* returnVal;
memLock->lock();
if(headPointer == NULL)
{
std::cout << "Error: Cannot pop an empty list\n";
memLock->unlock();
exit(1);
}

returnVal = headPointer->valPointer;
if(headPointer == tailPointer)
{
delete headPointer;
headPointer = tailPointer = NULL;
size = 0;
memLock->unlock();
return returnVal;
}

Node* temp = headPointer;
headPointer = headPointer->nextPointer;
--size;
delete temp;
memLock->unlock();
return returnVal;
}


void* front()
{
void* returnVal;
memLock->lock();
if(headPointer == NULL)
{
std::cout << "Error: in List function front(), list contained no elements\n";
memLock->unlock();
exit(1);
}
returnVal = headPointer->valPointer;
memLock->unlock();
return returnVal;
}

void update(int index, void* desiredVal)
{
int tempIndex = 0;

if(index < 0)
{
std::cout << "Error: index of " << index << " is out of bounds in list function update\n";
exit(1);
}

memLock->lock();
Node* travelPointer = headPointer;

while(tempIndex < index && travelPointer != NULL)
{
++tempIndex;
travelPointer = travelPointer->nextPointer;
}

if(travelPointer == NULL)
{
std::cout << "Error: update called with index of " << index << ", which was out of bounds\n";
memLock->unlock();
exit(1);
}
travelPointer->valPointer = desiredVal;
memLock->unlock();
return;
}

Node* headPointer;
Node* tailPointer;
int size;
std::mutex* memLock;

};
