#ifndef DATA_H
#define DATA_H

#include <stdio.h>      
#include <stdlib.h>     

#define MAX_VALUE
/*###############STRUCTURES#################*/
typedef struct dataNode {
	unsigned int value;
	struct dataNode* next;
} dataNode;

typedef struct {
	dataNode* head;
	int length;
} dataList;

extern dataList data_list;

/* 
 Adds data (char or int) to data list
 */
int addData(unsigned int);
/* return the length of cmd_list */
int getDataLength(void);
/* reset data table */
void resetDataTable(void);


#endif