
//#include "afxwin.h"

#include "Hash.h"
#include <iostream>


Hash::Hash()
{
	for (int i = 0; i < tableSize; i++)
	{
		HashTable[i] = new item;
		HashTable[i]->status = "empty";
		HashTable[i]->value = 0;
		HashTable[i]->x = 0;
		HashTable[i]->y = 0;
		HashTable[i]->next = NULL;
	}
}

int Hash::hash(int key)
{
	int hash = 0;
	int index;

	index = key % tableSize;

	return index;
}

void Hash::AddItem(int key, int x, int y)
{
	int index = hash(key);

	if (HashTable[index]->status == "empty")
	{
		HashTable[index]->status = "filled";
		HashTable[index]->value = key;
		HashTable[index]->x = x;
		HashTable[index]->y = y;
		HashTable[index]->next = NULL;
	}
	else
	{
		item* Ptr = HashTable[index];
		item* n = new item;
		n->status = "filled";
		n->value = key;
		n->x = x;
		n->y = y;
		n->next = NULL;
		while (Ptr->next != NULL)
		{
			Ptr = Ptr->next;
		}
		Ptr->next = n;
	}
}


int Hash::NumberOfItemsInIndex(int index)
{
	int count = 0;

	if (HashTable[index]->status == "empty")
	{
		return count;
	}
	else
	{
		count++;
		item *Ptr = HashTable[index];
		while (Ptr->next != NULL)
		{
			count++;
			Ptr = Ptr->next;
		}
		return count;
	}
}


void Hash::PrintTable()
{
	int number;
	for (int i = 0; i < tableSize; i++)
	{
		number = NumberOfItemsInIndex(i);
		std::cout << "-------------------------------\n";
		std::cout << "index = " << i << std::endl;
		std::cout << HashTable[i]->value << std::endl;
		std::cout << HashTable[i]->x << std::endl;
		std::cout << HashTable[i]->y << std::endl;
		std::cout << "# of items = " << number << std::endl;
		std::cout << "--------------------------------\n";
	}
}

void Hash::FindIndex(int key, int &x, int &y)
{
	int index = hash(key);
	bool foundIndex = false;
	
	item* Ptr = HashTable[index];
	while (Ptr != NULL || foundIndex != true)
	{
		if (Ptr->value == key)
		{
			foundIndex = true;
			x = Ptr->x;
			y = Ptr->y;
		}
		Ptr = Ptr->next;
	}
}