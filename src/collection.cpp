#include "collection.h"
#include "memory.h"
#include "util.h"
#include <string.h>

LinkedList* LinkedListCreate()
{
	LinkedList *linkedList = (LinkedList*)malloc_s(sizeof(LinkedList));
	_LinkedListNode *head = (_LinkedListNode*)malloc_s(sizeof(_LinkedListNode));
	LinkedListIterator *iterator = (LinkedListIterator *)malloc_s(sizeof(LinkedListIterator));
	iterator->host=linkedList;
	head->value=(void*)iterator;
	linkedList->headNode=head;
	linkedList->lastNode=head;
	head->next=NULL;
	head->last=NULL;
	linkedList->length=0;
	return linkedList;
}

unsigned long LinkedListAdd(LinkedList* linkedList,void* value)
{
	_LinkedListNode *node = (_LinkedListNode*)malloc_s(sizeof(_LinkedListNode));
	node->value=value;
	linkedList->lastNode->next=node;
	node->last=linkedList->lastNode;
	linkedList->lastNode=node;
	node->next=NULL;
	return linkedList->length++;
}

unsigned long LinkedListInsert(LinkedList* linkedList,unsigned long index,void* value)
{
	int result;
	_LinkedListNode *node=NULL;
	_LinkedListNode *insertedNode = (_LinkedListNode*)LinkedListGet(linkedList,index,&result);
	if(result)
		return LinkedListAdd(linkedList,value);
	node = (_LinkedListNode*)malloc_s(sizeof(_LinkedListNode));
	node->value=value;
	node->next=insertedNode;
	insertedNode->last->next=node;
	node->last=insertedNode->last;
	insertedNode->last=node;
	linkedList->length++;
	return index;
}
void* LinkedListGet(LinkedList* linkedList,unsigned long index,BOOL* result)
{
	//XXX:其实可以判断index与length的关系,然后选择性地做倒序搜索.
	_LinkedListNode *node;
	if(index>=linkedList->length)
	{
		if(result!=NULL)
			*result = FALSE;
		return NULL;
	}
	node=linkedList->headNode;
	for(;index>=0;index--) //因为headNode是不使用的,所以是>=
	{
		node=node->next;
	}
	if(result!=NULL)
		*result = TRUE;
	return node->value;
}

void* LinkedListRemoveNode(LinkedList *linkedList,_LinkedListNode *node)
{
	void* v;
	//_LinkedListNode *temp;
	v=node->value;
	if(linkedList->lastNode==node)
	{
		linkedList->lastNode=node->last;
		node->last->next=NULL;
	}
	else
	{
		node->last->next=node->next;
		node->next->last=node->last;
	}
	free_s(node);
	linkedList->length--;
	return v;
}

void* LinkedListRemove(LinkedList* linkedList,void* value,BOOL* result)
{
	_LinkedListNode *node=linkedList->headNode;
	while((node=node->next)!=NULL)
	{
		if(node->value==value)
		{
			if(result!=NULL)
				*result=TRUE;
			return LinkedListRemoveNode(linkedList,node);;
		}
	}
	if(result!=NULL)
		*result=FALSE;
	return NULL;
}

void* LinkedListRemoveByIndex(LinkedList* linkedList,unsigned long index,BOOL* result)
{
	int r;
	_LinkedListNode *node=(_LinkedListNode*)LinkedListGet(linkedList,index,&r);
	if(r)
	{
		if(result!=NULL)
			*result=FALSE;
		return NULL;
	}
	if(result!=NULL)
		*result=TRUE;
	return LinkedListRemoveNode(linkedList,node);
}

void* LinkedListPop(LinkedList* linkedList,BOOL* result)
{
	if(linkedList->length>0)
	{
		if(result!=NULL)
			*result=TRUE;
		return LinkedListRemoveNode(linkedList,linkedList->lastNode);
	}
	else
	{
		if(result!=NULL)
			*result=FALSE;
		return NULL;
	}
}

void* LinkedListPoll(LinkedList* linkedList,BOOL* result)
{
	if(linkedList->length>0)
	{
		if(result!=NULL)
			*result=TRUE;
		return LinkedListRemoveNode(linkedList,linkedList->headNode->next);
	}
	else
	{
		if(result!=NULL)
			*result=FALSE;
		return NULL;
	}
}

unsigned long LinkedListDestory(LinkedList* linkedList,int (*callbackFunction)(void* ))
{
	_LinkedListNode *deleted = NULL;
	_LinkedListNode *node;
	if(linkedList==NULL)
		return 0;
	node = linkedList->headNode->next;
	while(node!=NULL)
	{
		if(callbackFunction==NULL)
		{
			free_s(node->value);
		}
		else
		{
			if(callbackFunction(node->value))
			{
				linkedList->headNode->next=node;
				node->last=linkedList->headNode;
				return linkedList->length;
			}
			else
			{
				deleted=node;
				node=node->next;
				free_s(deleted);
				linkedList->length--;
			}
		}
	}
	free_s(linkedList->headNode->value);
	free_s(linkedList->headNode);
	free_s(linkedList);
	return 0;
}

LinkedListIterator* LinkedListGetIterator(LinkedList* linkedList)
{
	LinkedListIterator* iterator = (LinkedListIterator*)(linkedList->headNode->value);
	iterator->currentNode=linkedList->headNode;
	iterator->nextNode=iterator->currentNode->next;
	return iterator;
}

void* LinkedListIteratorGetNext(LinkedListIterator* iterator)
{
	iterator->currentNode=iterator->nextNode;
	iterator->nextNode=iterator->currentNode->next;
	return iterator->currentNode->value;
}

BOOL LinkedListIteratorHasNext(LinkedListIterator* iterator)
{
	return iterator->nextNode!=NULL;
}

void* LinkedListIteratorDeleteCurrent(LinkedListIterator* iterator)
{
	return LinkedListRemoveNode(iterator->host,iterator->currentNode);
}

void LinkedListIteratorPullUpCurrent( LinkedListIterator* iterator )
{
	_LinkedListNode *node = iterator->currentNode;
	if(iterator->host->lastNode==node)
	{
		return;
	}
	iterator->currentNode=iterator->nextNode;
	iterator->nextNode=iterator->currentNode==NULL?NULL:iterator->currentNode->next;
	node->last->next=iterator->currentNode;
	if(iterator->currentNode!=NULL)
	{
		iterator->currentNode->last=node->last;
	}
	node->next=iterator->host->headNode->next;
	node->next->last=node;
	iterator->host->headNode->next=node;
	node->last=iterator->host->headNode;
}

_HashTreeNode* HashTreeNodeCreate(_HashTreeNode* parent,char* name,Hash hashCode,void* data);

HashTree* HashTreeCreate()
{
	HashTree *ht = (HashTree*)malloc_s(sizeof(HashTree));
	ht->rootNode = HashTreeNodeCreate(NULL,NULL,DEFAULT_HBST_ROOT,NULL);
	ht->length=0;
	return ht;
}
BOOL _HashTreeInsert(_HashTreeNode* inserted,_HashTreeNode* inserting)
{
	_HashTreeNode *curNode = inserted;
	_HashTreeNode *newNode = inserting;
	while(TRUE)
	{
		if(inserting->hash < curNode->hash)
		{
			if(curNode->leftNode==NULL)
			{
				curNode->leftNode = newNode;
				newNode->parentNode=curNode;
				break;
			}
			curNode=curNode->leftNode;
		}
		else if(inserting->hash > curNode->hash)
		{
			if(curNode->rightNode==NULL)
			{
				curNode->rightNode = newNode;
				newNode->parentNode=curNode;
				break;
			}
			curNode=curNode->rightNode;
		}
		//我假设在二次插入时不存在Hash碰撞现象
	}
	return TRUE;
}
BOOL _HashTreeSet(HashTree* ht,char* key,void* value,BOOL allowOverwrite)
{
	_HashTreeNode *curNode = ht->rootNode;
	_HashTreeNode *newNode = NULL;
	Hash hash = HashCode(key);
	int depth = 0;
	while(TRUE)
	{
		if(depth>MAX_HBST_DEPTH)
			return FALSE;
		if(hash < curNode->hash)
		{
			if(curNode->leftNode==NULL)
			{
				newNode = HashTreeNodeCreate(curNode,key,hash,value);
				if(newNode==NULL)
					return FALSE;
				curNode->leftNode = newNode;
				break;
			}
			curNode=curNode->leftNode;
		}
		else if(hash > curNode->hash)
		{
			if(curNode->rightNode==NULL)
			{
				newNode = HashTreeNodeCreate(curNode,key,hash,value);
				if(newNode==NULL)
					return FALSE;
				curNode->rightNode = newNode;
				break;
			}
			curNode=curNode->rightNode;
		}
		else
		{
			if(strcmp(curNode->key,key)==0)
			{
				if(allowOverwrite)
				{
					newNode = HashTreeNodeCreate(curNode->parentNode,key,hash,value);
					if(newNode==NULL)
						return FALSE;
					newNode->nextNode = curNode->nextNode;
					newNode->leftNode = curNode->leftNode;
					newNode->rightNode = curNode->rightNode;
					if(curNode->parentNode->leftNode==curNode)
					{
						curNode->parentNode->leftNode=newNode;
					}
					else if(curNode->parentNode->rightNode==curNode)
					{
						curNode->parentNode->rightNode=newNode;
					}
					else
					{
						curNode->parentNode->nextNode=newNode;
					}
					free_s(curNode);
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			if(curNode->nextNode==NULL)
			{
				newNode = HashTreeNodeCreate(curNode,key,hash,value);
				if(newNode==NULL)
					return FALSE;
				curNode->nextNode = newNode;
				break;
			}
			curNode=curNode->nextNode;
		}
		depth++;
	}
	return TRUE;
}
BOOL HashTreeAdd(HashTree* ht,char* key,void* value)
{
	return _HashTreeSet(ht,key,value,FALSE);
}
BOOL HashTreeSet(HashTree* ht,char* key,void* value)
{
	return _HashTreeSet(ht,key,value,TRUE);
}
_HashTreeNode* _HashTreeGet(HashTree* ht,char* key,BOOL* result)
{
	_HashTreeNode* node = ht->rootNode;
	Hash hash = HashCode(key);
	while(TRUE)
	{
		if(node==NULL)
		{
			if(result!=NULL)
				*result = FALSE;
			return NULL;
		}			
		if(hash < node->hash)
		{
			node=node->leftNode;
		}
		else if(hash > node->hash)
		{
			node=node->rightNode;
		}
		else
		{
			if(strcmp(key,node->key)==0)
			{
				if(result!=NULL)
					*result = TRUE;
				return node;
			}
			node=node->nextNode;
		}
	}
}
void* HashTreeRemove(HashTree* ht,char* key,BOOL* result)
{
	_HashTreeNode* node = NULL;
	_HashTreeNode* parent = NULL;
	void* v = NULL;
	node = _HashTreeGet(ht,key,NULL);
	if(node==NULL)
	{
		if(result!=NULL)
			*result = TRUE;
		return NULL;
	}
	parent = node->parentNode;
	if(parent->nextNode!=node && node->nextNode!=NULL) //node不是链表中的节点,但node带有链表
	{
		if(parent->leftNode==node)
		{
			parent->leftNode=node->nextNode;
		}
		else if(parent->rightNode==node)
		{
			parent->rightNode=node->nextNode;
		}
		node->nextNode->parentNode=parent;
		node->nextNode->leftNode=node->leftNode;
		node->nextNode->rightNode=node->rightNode;
		v=node->value;
		if(result!=NULL)
			*result = TRUE;
		free(node);
		return v;
	}
	else if(parent->leftNode==node) //若node是左子树
	{
		if(node->rightNode==NULL)
		{
			parent->leftNode=node->leftNode;
			node->leftNode->parentNode = parent;
		}
		else
		{
			parent->leftNode=node->rightNode;
			node->rightNode->parentNode = parent;
			_HashTreeInsert(node->rightNode,node->leftNode);
		}
		v=node->value;
		if(result!=NULL)
			*result = TRUE;
		free(node);
		return v;
	}
	else if(parent->rightNode==node) //若node是右子树
	{
		if(node->leftNode==NULL)
		{
			parent->rightNode=node->rightNode;
			node->rightNode->parentNode = parent;
		}
		else
		{
			parent->rightNode=node->leftNode;
			node->leftNode->parentNode = parent;
			_HashTreeInsert(node->leftNode,node->rightNode);
		}
		v=node->value;
		if(result!=NULL)
			*result = TRUE;
		free(node);
		return v;
	}
	else if(parent->nextNode==node)
	{
		parent->nextNode=node->nextNode;
		if(node->nextNode!=NULL)
			node->nextNode=parent;
		v=node->value;
		if(result!=NULL)
			*result = TRUE;
		free(node);
		return v;
	}
	if(result!=NULL)
		*result = FALSE;
	return NULL;
}
void* HashTreeGet(HashTree* ht,char* key,BOOL* result)
{
	_HashTreeNode* node = NULL;
	node = _HashTreeGet(ht,key,result);
	if(node==NULL)
		return NULL;
	return node->value;
}
//int HashTreeLength(HashTree* ht,char* key);
BOOL _HashTreeNodeDestroy(_HashTreeNode* node,void (*callbackFunction)(void* ),BOOL isRoot)
{
	char i = 0;
	if(node->nextNode!=NULL)
		i += _HashTreeNodeDestroy(node->nextNode,callbackFunction,FALSE);
	else
		i++;
	if(node->leftNode!=NULL)
		i += _HashTreeNodeDestroy(node->leftNode,callbackFunction,FALSE);
	else
		i++;
	if(node->rightNode!=NULL)
		i += _HashTreeNodeDestroy(node->rightNode,callbackFunction,FALSE);
	else
		i++;
	if(i!=3)
		return FALSE;
	if(isRoot==FALSE)
		callbackFunction(node->value);
	free(node);
	return TRUE;
}
BOOL HashTreeDestroy(HashTree* ht,void (*callbackFunction)(void* ))
{
	BOOL result;
	if(ht==NULL)
		return TRUE;
	result = _HashTreeNodeDestroy(ht->rootNode,callbackFunction,TRUE);
	if(result == TRUE)
		free(ht);
	return result;
}

_HashTreeNode* HashTreeNodeCreate(_HashTreeNode* parent,char* name,Hash hashCode,void* data)
{
	_HashTreeNode* node = (_HashTreeNode*)malloc_s(sizeof(_HashTreeNode));
	node->hash=hashCode;
	node->key=name;
	node->leftNode=NULL;
	node->rightNode=NULL;
	node->parentNode=parent;
	node->nextNode=NULL;
	node->value=data;
	return node;
}