
class UserDefinedObject
{
public:

UserDefinedObject(int myNewSize)
{
if(myNewSize < 0)
{
std::cout << "Error: num of member of variables was less than 0\n";
exit(1);
}
size = myNewSize;
internalArray = new long long[size];
for(int i = 0; i < size; ++i)
internalArray[i] = 0;
}

~UserDefinedObject()
{
delete [] internalArray;
internalArray = NULL;
}

long long* internalArray;
int size;
};
