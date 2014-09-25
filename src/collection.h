#pragma once
#include "game.h"
#include "util.h"

#define LinkedListOffer(linkedList,value) LinkedListAdd(linkedList,value)
#define LinkedListPush(linkedList,value) LinkedListAdd(linkedList,value)

struct implLinkedList{
	_LinkedListNode *headNode;
	_LinkedListNode *lastNode;
	unsigned long length;
};

struct implLinkedListIterator{
	LinkedList *host;
	_LinkedListNode *currentNode;
	_LinkedListNode *nextNode;
};

struct implLinkedListNode
{
	_LinkedListNode *last;
	_LinkedListNode *next;
	void *value;
};

LinkedList* LinkedListCreate();
unsigned long LinkedListAdd(LinkedList* linkedList,void* value);
unsigned long LinkedListInsert(LinkedList* linkedList,unsigned long index,void* value);
void* LinkedListGet(LinkedList* linkedList,unsigned long index,BOOL* result);
void* LinkedListRemove(LinkedList* linkedList,void* value,BOOL* result);
void* LinkedListRemoveByIndex(LinkedList* linkedList,unsigned long index,BOOL* result);
void* LinkedListPop(LinkedList* linkedList,BOOL* result);
void* LinkedListPoll(LinkedList* linkedList,BOOL* result);
unsigned long LinkedListDestory(LinkedList* linkedList,int (*callbackFunction)(void* ));
LinkedListIterator* LinkedListGetIterator(LinkedList* linkedList);
void* LinkedListIteratorGetNext(LinkedListIterator* iterator);
BOOL LinkedListIteratorHasNext(LinkedListIterator* iterator);
void* LinkedListIteratorDeleteCurrent(LinkedListIterator* iterator);
void LinkedListIteratorPullUpCurrent(LinkedListIterator* iterator);

#define DEFAULT_HBST_ROOT 500000000
#define MAX_HBST_DEPTH 1023

struct implHashTreeNode{
	unsigned long hash;
	char *key;
	void *value;
	_HashTreeNode *parentNode;
	_HashTreeNode *leftNode;
	_HashTreeNode *rightNode;
	_HashTreeNode *nextNode; //当发生Hash冲突时,节点就会变成一个链表
};

struct implHashTree{
	int length;
	_HashTreeNode *rootNode;
};

HashTree* HashTreeCreate();
BOOL HashTreeAdd(HashTree* ht,char* key,void* value);
BOOL HashTreeSet(HashTree* ht,char* key,void* value);
void* HashTreeRemove(HashTree* ht,char* key,BOOL* result);
void* HashTreeGet(HashTree* ht,char* key,BOOL* result);
BOOL HashTreeDestroy(HashTree* ht,void (*callbackFunction)(void* ));