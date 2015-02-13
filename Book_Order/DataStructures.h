#include <pthread.h>

typedef struct BookOrders BookOrder; 

/** Book order */
struct BookOrders {

	char *title;			/** Book title */
	char *category;			/** Book category */
	int ID;				/** Customer ID */
	float price;			/** Book price */
	BookOrder *nextBookOrder;	/** Next Book order */
};

typedef struct Queue BookQueue;

/** Book queue */
struct Queue {

	BookOrder *bookOrders;		/** Book order list */
	BookOrder *tail;		/** Tail of queue */
};

typedef struct Books Book; 

struct Books {

	char *title;			/** Book title */
	char *category;			/** Book category */
	float price;			/** Book price */
	float balance;			/** Customer balance after order */
	int ID;				/** Customer ID */
	Book *nextBook;			/** Next book */
};

typedef struct Customers Customer;

struct Customers {

	char *name;			/** Customer name */
	char *address;			/** Customer address */
	char *state;			/** Customer state */
	char *zip;			/** Customer zip code */
	int ID;				/** Customer ID */
	float creditLimit;		/** Customer credit limit */
	float balance;			/** Customrr current balance */
	Book *successfulOrder;		/** List of successful book orders */
	Book *unsuccessfulOrder;	/** List of unsuccessful book orders */
	Book *S_OTail;			/** Successful book order tail */
	Book *U_OTail;			/** Unsuccessful book order tail */
	Customer *nextCustomer;		/** Next customer in hash map */
	Customer *nextCustomerL;	/** Next customer in customer list */
};

typedef struct Customer_List CustomerList;

/** Customer list */
struct Customer_List {

	Customer *head;
};

typedef struct Categories Category;

/** Category */
struct Categories {

	char *category;
	Category *nextCategory;
};

void createBook(Book **book, BookOrder **bookOrder);
void addBookToCustomer(Book **book, int index);
void createBookOrder(BookOrder **bookOrder, char *bookInfo[]);
void addToBookQueueList(BookOrder **bookOrder);
void createQueue();
void createCustomer(Customer **customer, char *info[]);
void addCustomer(Customer **customer, int index) ;
void addToCategoriesList(Category **cat);
void createCategoryNode(Category **cat, char *category);
void initializeBookQueue();
void initializeCustomerList();
void initializeCustomerHashMap();
void initializeCategoriesList();
void destroyQueue();
void destroyCategoriesList();
void destroyBookOrder(BookOrder **bookOrder);
void destroyBook(Book **book);
void destroyC_List() ;
