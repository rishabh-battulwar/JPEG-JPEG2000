#ifndef HASH_H
#define HASH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "afxwin.h"


#include <string>

class Hash
{
	private:
		static const int tableSize = 64;
		
		struct item
		{
			std::string status;
			int value;
			int x;
			int y;
			item *next;
		};

		item *HashTable[tableSize];

	public:
		Hash();
		int hash(int key);
		void AddItem(int key, int x, int y);
		int NumberOfItemsInIndex(int index);
		void PrintTable();
		void FindIndex(int key, int &x, int &y);

};

#endif HASH_H