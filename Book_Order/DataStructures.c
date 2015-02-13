#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "DataStructures.h"

#define SIZE 100

CustomerList C_Hash[SIZE]; 	/** Customer hash map for fast look up */
CustomerList C_List;		/** Custoemr list */
BookQueue *bookQueue;		/** Book queue */
Category *categoriesList;	/** Category list */
	
float revenue = 0;		/** Total Revenue */
int QueueSize = 0;		/** Queue size */
int numOfCategory = 0;		/** Number of category */
int error = 0;
int invalidID;

/** Create book Node */
void createBook(Book **book, BookOrder **bookOrder) {

	*book = (Book*)malloc(sizeof(Book));
	(*book) -> title = (char*)malloc(strlen((*bookOrder) -> title) +1);
	strcpy((*book) -> title, (*bookOrder) -> title);
	(*book) -> title[strlen((*bookOrder) -> title)] = '\0';
	(*book) -> category = (char*)malloc(strlen((*bookOrder) -> category) +1);
	strcpy((*book) -> category, (*bookOrder) -> category);
	(*book) -> category[strlen((*bookOrder) -> category)] = '\0';
	(*book) -> price = (*bookOrder) -> price;
	(*book) -> balance = 0;
	(*book) -> ID = (*bookOrder) -> ID;
	(*book) -> nextBook = NULL;
}

/** Add book to customer if he/she has enough balance */
void addBookToCustomer(Book **book, int index) {

	Customer *curr = C_Hash[index].head; /** Get the customer in hash map */

	while(curr != NULL) {
		/** Check if it is target customer */
		if(curr -> ID == (*book) -> ID) {
			/** Check if customer has enough balance */
			if((*book) -> price <= curr -> balance) {
				/** Store book in successful order list */
				revenue = revenue + (*book) -> price;
				float newBalance = curr -> balance - (*book) -> price;
				(*book) -> balance = newBalance;
				curr -> balance  = newBalance;
				if(curr -> S_OTail == NULL) {
					curr -> successfulOrder = (*book);
					curr -> S_OTail = (*book);	
				}
				else {
					curr -> S_OTail -> nextBook = (*book);
					curr -> S_OTail = (*book);
				}
				/** Print order confirmation */
				printf("\nOrder Confirmation:\nTitle: %s\nPrice: %.2f\nCustomer: %s\nAddress: %s\nState: %s\nZip: %s\n", (*book) -> title, (*book) -> price, curr -> name, curr -> address, curr -> state, curr -> zip);
			}
			/** If customer does not have enough balance store it in unsuccessful order list */
			else {
				if(curr -> U_OTail == NULL) {
					curr -> unsuccessfulOrder = (*book);
					curr -> U_OTail = (*book);
				}
				else {
					curr -> U_OTail -> nextBook = (*book);
					curr -> U_OTail = (*book);			
				}
				/** Print order rejection */
				printf("\nOrder Rejection:\nTitle: %s\nPrice: %.2f\nCategory: %s\nCustomer: %s\n", (*book) -> title, (*book) -> price, (*book) -> category, curr -> name);
			}
			return;
		}
		curr = curr -> nextCustomer;
	}
	error = 1; 
	invalidID = (*book) -> ID;
}
/** Create book order node */
void createBookOrder(BookOrder **bookOrder, char *bookInfo[]) {

	*bookOrder = (BookOrder*)malloc(sizeof(BookOrder));
	(*bookOrder) -> title = (char*)malloc(strlen(bookInfo[0]) + 1);
	strcpy((*bookOrder) -> title, bookInfo[0]);
	(*bookOrder) -> title[strlen(bookInfo[0])] = '\0';
	(*bookOrder) -> category = (char*)malloc(strlen(bookInfo[3]) + 1);
	strcpy((*bookOrder) -> category, bookInfo[3]);
	(*bookOrder) -> category[strlen(bookInfo[3])] = '\0';
	sscanf(bookInfo[2], "%d", &((*bookOrder) -> ID));
	sscanf(bookInfo[1], "%f", &((*bookOrder) -> price));
	(*bookOrder) -> nextBookOrder = NULL;
}

/** Add book order to queue */
void addToBookQueueList(BookOrder **bookOrder) {

	if(bookQueue -> bookOrders == NULL) {
		bookQueue -> bookOrders = *bookOrder; 
		bookQueue -> tail = *bookOrder;
	}
	else {
		bookQueue -> tail -> nextBookOrder = *bookOrder; 
		bookQueue -> tail = *bookOrder;
	}
	QueueSize++;
}

/** Create book queue */
void createQueue() {

	bookQueue = (BookQueue*)malloc(sizeof(BookQueue));
	bookQueue -> bookOrders = NULL;
	bookQueue -> tail = NULL;
}

/** Create customer Node */
void createCustomer(Customer **customer, char *info[]) {
	
	*customer = (Customer*)malloc(sizeof(Customer));
	(*customer) -> name = (char*)malloc(strlen(info[0]) + 1);
	strcpy((*customer) -> name, info[0]);
	(*customer) -> name[strlen(info[0])] = '\0';	
	(*customer) -> address = (char*)malloc(strlen(info[3]) + 1);
	strcpy((*customer) -> address, info[3]);
	(*customer) -> address[strlen(info[3])] = '\0';
	(*customer) -> state = (char*)malloc(strlen(info[4]) + 1);
	strcpy((*customer) -> state, info[4]);
	(*customer) -> state[strlen(info[4])] = '\0';
	(*customer) -> zip = (char*)malloc(strlen(info[5]) + 1);
	strcpy((*customer) -> zip, info[5]);
	(*customer) -> zip[strlen(info[5])] = '\0';
	sscanf(info[1], "%d", &((*customer) -> ID));
	sscanf(info[2], "%f", &((*customer) -> creditLimit));
	(*customer) -> balance = (*customer) -> creditLimit;
	(*customer) -> successfulOrder = NULL;
	(*customer) -> unsuccessfulOrder = NULL;
	(*customer) -> S_OTail = NULL;
	(*customer) -> U_OTail = NULL;
	(*customer) -> nextCustomer = NULL;
	(*customer) -> nextCustomerL = NULL;
}

/** Add customer node to customer list and customer hash map*/
void addCustomer(Customer **customer, int index) {

	Customer *head = C_Hash[index].head;
	
	/** Add to customer hash map */
	if(head == NULL) 
		C_Hash[index].head = *customer;
	else {
		(*customer) -> nextCustomer = C_Hash[index].head;
		C_Hash[index].head = *customer;
	}

	/** Add to customer list */
	head = C_List.head;
	if(head == NULL) 
		C_List.head = *customer;
	else {
		(*customer) -> nextCustomerL = C_List.head;
		C_List.head = *customer;
	}
}

/** Add category node to category list */
void addToCategoriesList(Category **cat) {

	if(categoriesList == NULL) {
		categoriesList = *cat;
	}
	else {
		(*cat) -> nextCategory = categoriesList;
		categoriesList = *cat;
	}
}

/** Create category node */
void createCategoryNode(Category **cat, char *category) {

	*cat = (Category*)malloc(sizeof(Category));
	(*cat) -> category = (char*)malloc(strlen(category) + 1);
	strcpy((*cat) -> category, category);
	(*cat) -> category[strlen(category)] = '\0';
	(*cat) -> nextCategory = NULL;
}

/** Initialize book queue */
void initializeBookQueue() {

	bookQueue = NULL;
}

/** Initialize customer list */
void initializeCustomerList() {

	C_List.head = NULL;
}

/** Initialize customer hash map */
void initializeCustomerHashMap() {

	int i;
	for(i = 0; i < SIZE; i++) 
		C_Hash[i].head = NULL;
}

/** Initialize category list */
void initializeCategoriesList() {

	categoriesList = NULL;
}

/** Destroy book queue */
void destroyQueue() {

	bookQueue -> bookOrders = NULL;
	bookQueue -> tail = NULL;
	free(bookQueue);
	bookQueue = NULL;
}

/** Destroy category list */
void destroyCategoriesList() {

	Category *temp, *curr = categoriesList;
	while(curr != NULL) {
		temp = curr -> nextCategory;
		free(curr -> category);
		curr -> category = NULL;
		free(curr);
		curr = temp;
	}
	categoriesList = NULL;
}

/** Destroy book order node */		
void destroyBookOrder(BookOrder **bookOrder) {

	free((*bookOrder) -> title);
	(*bookOrder) -> title = NULL;
	free((*bookOrder) -> category);
	(*bookOrder) -> category = NULL;
	(*bookOrder) -> nextBookOrder = NULL;
	free(*bookOrder);
}

/** Destroy book */
void destroyBook(Book **book) {

	Book *temp, *curr = *book;
	while(curr != NULL) {
		temp = curr -> nextBook;		
		free(curr -> title);
		curr-> title = NULL;
		free(curr);
		curr = temp;
	}
}

/** Destroy customer List */
void destroyC_List() {

	int i;
	Customer *curr, *temp;
	curr = C_List.head;
	while(curr != NULL) {
		temp = curr -> nextCustomerL;
		free(curr -> name);
		curr -> name = NULL;
		free(curr -> address);
		curr -> address =  NULL;
		free(curr -> state);
		curr -> state = NULL;
		free(curr -> zip);
		curr -> zip = NULL;
		curr -> nextCustomer = NULL;
		curr -> nextCustomerL = NULL;
		curr -> S_OTail = NULL;
		curr -> U_OTail = NULL;
		destroyBook(&(curr -> successfulOrder));
		destroyBook(&(curr -> unsuccessfulOrder));
		curr -> successfulOrder = NULL;
		curr -> unsuccessfulOrder = NULL;
		free(curr);
		curr = temp;
	}
}
