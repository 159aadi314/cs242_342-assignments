//Handle multiple socket connections with select and fd_set on Linux  
#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  // gives a variable errno to know whats the error
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close, in_addr_t, in_port_t
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  

#define pi 3.14
#define TRUE  1
#define FALSE 0

struct request{				// request does not specify buy or sell, or the item, coz that depends on the prio queue.
	int trader_id;			// prio q is according to item and buy and sell queues are different
	int price;
	int quantity;
};

struct trade{
	int item_id;
	int seller_id;
	int buyer_id;
	int price;
	int quantity;
};

struct trades{			// simple array, but need to implement lock and cur_size too thats why separate struct
	int locked;				// mutex lock
	int cur_size;			// current size of this list of trades
	struct trade array[201];		// array of matched trades
};

struct prio_queue{
  int locked;		// like a mutex lock, prevents more than one function editing this pq at a time
  int cur_size;	// current size of this pq
  struct request heap[201];		// its a priority queue of requests
};

void initialise_t(struct trades *t){
	t->locked=0;
	t->cur_size=0;
}

void initialise(struct prio_queue *pq){
	pq->locked=0;
	pq->cur_size=0;
}

int isFull_t(struct trades *t){
	if(t->locked==1)
		return -1;
	t->locked=1;
	if(t->cur_size==100){					// atmost 100 trades allowed in total
		t->locked=0;
		return 1;
	}
	else{
		t->locked=0;
		return 0;
	}
}

int isFull(struct prio_queue *pq){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	if(pq->cur_size==100){				// atmost 100 trade requests allowed at a time
		pq->locked=0;
		return 1;
	}
	else{
		pq->locked=0;
		return 0;
	}
}

int getSize(struct trades *t){
	if(t->locked==1)
		return -1;
	t->locked=1;
	int siz = t->cur_size;
	t->locked=0;
	return siz;
}

int getTradeByIndex(struct trades *t, struct trade *current, int index){	// get index 'th element in trades array and put it into current pointer
	if(t->locked==1)
		return -1;
	t->locked=1;
	*current = t->array[index];
	t->locked=0;
	return 0;
}

int isEmpty_t(struct trades *t){	
	if(t->locked==1)
		return -1;
	t->locked=1;
	if(t->cur_size == 0){
		t->locked=0;
		return 1;
	}
	else{
		t->locked=0;
		return 0;
	}
}

int isEmpty(struct prio_queue *pq){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	if(pq->cur_size == 0){
		pq->locked=0;
		return 1;
	}
	else{
		pq->locked=0;
		return 0;
	}
}

int insertIntoArray(struct trade newEntry, struct trades *t){
	if(isFull_t(t))
		return -2;								// full returns -2, locked returns -1, success returns 0

	if(t->locked==1)
		return -1;
	t->locked=1;
	t->cur_size++;						// both in arr and prio queues, nothing stored at index 0, data starts from index 1
	t->array[t->cur_size]=newEntry;
	t->locked=0;
	return 0;
}

int insertIntoMinHeap(struct request newEntry, struct prio_queue *pq){
	if(isFull(pq))
		return -2;

	if(pq->locked==1)
		return -1;
	pq->locked=1;
	pq->cur_size++;
	pq->heap[pq->cur_size]=newEntry;
	int curr=pq->cur_size;
	while(curr>1 && ((pq->heap[curr].price)<(pq->heap[curr/2].price))){
		struct request temp=pq->heap[curr];
		pq->heap[curr]=pq->heap[curr/2];
		pq->heap[curr/2]=temp;
		curr/=2;
	}
	pq->locked=0;
	return 0;
}

int insertIntoMaxHeap(struct request newEntry, struct prio_queue *pq){
	if(isFull(pq))
		return -2;

	if(pq->locked==1)
		return -1;
	pq->locked=1;
	pq->cur_size++;
	pq->heap[pq->cur_size]=newEntry;
	int curr=pq->cur_size;
	while(curr>1 && ((pq->heap[curr].price)>(pq->heap[curr/2].price))){
		struct request temp=pq->heap[curr];
		pq->heap[curr]=pq->heap[curr/2];
		pq->heap[curr/2]=temp;
		curr/=2;
	}
	pq->locked=0;
	return 0;
}


int fix_min(struct prio_queue *pq){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	int curr=1;
	while(curr*2<=pq->cur_size){
		if(curr*2+1<=pq->cur_size){
			if((pq->heap[curr].price)<=(pq->heap[curr*2].price)&&(pq->heap[curr].price)<=(pq->heap[curr*2+1].price))
				break;
			else{
				if((pq->heap[curr*2].price)<=(pq->heap[curr*2+1].price)){
					struct request temp=pq->heap[curr*2];
					pq->heap[curr*2]=pq->heap[curr];
					pq->heap[curr]=temp;
					curr*=2;
				} else {
					struct request temp=pq->heap[curr*2+1];
					pq->heap[curr*2+1]=pq->heap[curr];
					pq->heap[curr]=temp;
					curr*=2;
					curr++;
				}
			}
		} else {
			if((pq->heap[curr].price)<=(pq->heap[curr*2].price))
				break;
			else{
				struct request temp=pq->heap[curr*2];
				pq->heap[curr*2]=pq->heap[curr];
				pq->heap[curr]=temp;
				curr*=2;
			}
		}
	}
	pq->locked=0;
	return 0;
}

int fix_max(struct prio_queue *pq){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	int curr=1;
	while(curr*2<=pq->cur_size){
		if(curr*2+1<=pq->cur_size){
			if((pq->heap[curr].price)>=(pq->heap[curr*2].price)&&(pq->heap[curr].price)>=(pq->heap[curr*2+1].price))
				break;
			else{
				if((pq->heap[curr*2].price)>=(pq->heap[curr*2+1].price)){
					struct request temp=pq->heap[curr*2];
					pq->heap[curr*2]=pq->heap[curr];
					pq->heap[curr]=temp;
					curr*=2;
				} else {
					struct request temp=pq->heap[curr*2+1];
					pq->heap[curr*2+1]=pq->heap[curr];
					pq->heap[curr]=temp;
					curr*=2;
					curr++;
				}
			}
		} else {
			if((pq->heap[curr].price)>=(pq->heap[curr*2].price))
				break;
			else{
				struct request temp=pq->heap[curr*2];
				pq->heap[curr*2]=pq->heap[curr];
				pq->heap[curr]=temp;
				curr*=2;
			}
		}
	}
	pq->locked=0;
	return 0;
}

int extractMin(struct prio_queue *pq, struct request *minimum){
	if(isEmpty(pq))
		return -2;
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	*minimum=pq->heap[1];
	if(pq->cur_size==1)
	{
		pq->cur_size=0;
		pq->locked=0;
	}
	else{
		pq->heap[1] = pq->heap[pq->cur_size];
		pq->cur_size--;
		pq->locked=0;

		fix_min(pq);
	}
	return 0;
}

int extractMax(struct prio_queue *pq, struct request *maximum){
	if(isEmpty(pq))
		return -2;
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	*maximum=pq->heap[1];
	if(pq->cur_size==1)
	{
		pq->cur_size=0;
		pq->locked=0;
	}
	else{
		pq->heap[1] = pq->heap[pq->cur_size];
		pq->cur_size--;
		pq->locked=0;

		fix_max(pq);
	}
	return 0;
}

int min_price_min_heap(struct prio_queue *pq){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	int mini=pq->heap[1].price;
	pq->locked=0;
	return mini;
}

int max_price_min_heap(struct prio_queue *pq){			// min heap designed in such a way that left subtree is smaller than right subtree always, similarly max heap
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	int maxi=pq->heap[pq->cur_size].price;
	pq->locked=0;
	return maxi;
}

int max_price_max_heap(struct prio_queue *pq){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	int maxi=pq->heap[1].price;
	pq->locked=0;
	return maxi;
}

int min_price_max_heap(struct prio_queue *pq){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	int mini=pq->heap[pq->cur_size].price;
	pq->locked=0;
	return mini;
}

int quantity_of_min_price_min_heap(struct prio_queue *pq){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	int quant=pq->heap[1].quantity;
	pq->locked=0;
	return quant;
}

int quantity_of_max_price_max_heap(struct prio_queue *pq){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	int quant=pq->heap[1].quantity;
	pq->locked=0;
	return quant;
}

int changeMinQuantity_min_heap(struct prio_queue *pq, int new_value){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	pq->heap[1].quantity = new_value;
	pq->locked=0;
	return 0;
}

int changeMaxQuantity_max_heap(struct prio_queue *pq, int new_value){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	pq->heap[1].quantity = new_value;
	pq->locked=0;
	return 0;
}

int showMin_min_heap(struct prio_queue *pq, struct request *minimum){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	*minimum=pq->heap[1];
	pq->locked=0;
	return 0;
}

int showMax_max_heap(struct prio_queue *pq, struct request *maximum){
	if(pq->locked==1)
		return -1;
	pq->locked=1;
	*maximum=pq->heap[1];
	pq->locked=0;
	return 0;
}
void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

struct prio_queue sell_pq[11], buy_pq[11];		// array of buy and sell priority queues
int logged_in[6];															// array of logged in users
struct trades trade_array[6];									// array of matched trades of each user

void init(){				// initialises all sell q's, buy q's, trade arrays and logged in users ki array
	int num1=10;
	while(num1>0) {
		
		num1--;
	}
	initialise(&sell_pq[1]);
	initialise(&sell_pq[2]);
	initialise(&sell_pq[3]);			
	initialise(&sell_pq[4]);
	initialise(&sell_pq[5]);
	initialise(&sell_pq[6]);
	initialise(&sell_pq[7]);
	initialise(&sell_pq[8]);
	initialise(&sell_pq[9]);
	initialise(&sell_pq[10]);
	int num2=10;
	int num3=5;
	int num4=5;
	while(num2>0){

		num2--;
	}
	initialise(&buy_pq[1]);
	initialise(&buy_pq[2]);
	initialise(&buy_pq[3]);
	initialise(&buy_pq[4]);
	initialise(&buy_pq[5]);
	initialise(&buy_pq[6]);
	initialise(&buy_pq[7]);
	initialise(&buy_pq[8]);
	initialise(&buy_pq[9]);
	initialise(&buy_pq[10]);
	logged_in[1]=-1;
	logged_in[2]=-1;
	logged_in[3]=-1;
	logged_in[4]=-1;
	logged_in[5]=-1;
	initialise_t(&trade_array[1]);
	initialise_t(&trade_array[2]);
	initialise_t(&trade_array[3]);
	initialise_t(&trade_array[4]);
	initialise_t(&trade_array[5]);
	while(num3--) num3=num3;
	while(num4--) num4=num4;
}

//-1 indicates no such user exists
//-2 indicates wrong password
//-3 indicates already logged in from somewhere else
//non negative value indicates authentication successful
int authenticate(char username[], char password[], int username_len, int password_len){			// goes thru trader_auth.txt and sees if a username with those credentials exists, error codes listed above. same username not allowed
	FILE *fptr;
	fptr = fopen("traders_auth.txt","r");
	int i=1;
	while(i<=5){
		char uname[2049], pwd[2049];
		bzero(uname,2049);
		bzero(pwd,2049);
		char c;
		int uname_len=0, pwd_len=0;
		while((c = fgetc(fptr))!=' '){
			uname[uname_len]=c;
			uname_len++;
		}
		while((c = fgetc(fptr))!='\n'){
			pwd[pwd_len]=c;
			pwd_len++;
		}
		if(uname_len!=username_len)
			continue;
		else{
			int umatch=1;
			for(int j=0;j<2049;j++){
				if(username[j]!=uname[j]){
					umatch=0;
					break;
				}
			}
			if(umatch==0){
				continue;
			}
			else{
				if(password_len!=pwd_len){
					fclose(fptr);
					return -2;
				}
				else{
					int pmatch=1;
					for(int j=0;j<2049;j++){
						if(password[j]!=pwd[j]){
							pmatch=0;
							break;
						}
					}
					if(pmatch==1){
						fclose(fptr);
						if(logged_in[i-1]==-1){
							return i;
						}
						else{
							return -3;
						}
					}
					else{
						fclose(fptr);
						return -2;
					}
				}
			}
		}
		i++;
	}
	fclose(fptr);
	return -1;
}

void buy(int trader_id, int item_no, int quantity, int price){
	while(quantity>0&&((!isEmpty(&sell_pq[item_no]))&&min_price_min_heap(&sell_pq[item_no])<=price)){
		int seller_has = quantity_of_min_price_min_heap(&sell_pq[item_no]);
		if(seller_has>quantity){
			struct request mini;
			showMin_min_heap(&sell_pq[item_no], &mini);
			struct trade tr;
			tr.item_id = item_no;
			tr.seller_id = mini.trader_id;
			tr.buyer_id = trader_id;
			tr.price = mini.price;
			tr.quantity = quantity;

			insertIntoArray(tr, &trade_array[tr.seller_id]);
			insertIntoArray(tr, &trade_array[tr.buyer_id]);

			quantity -= tr.quantity;
			changeMinQuantity_min_heap(&sell_pq[item_no], mini.quantity - tr.quantity);
		}
		else{
			struct request mini;
			extractMin(&sell_pq[item_no], &mini);
			struct trade tr;
			tr.item_id = item_no;
			tr.seller_id = mini.trader_id;
			tr.buyer_id = trader_id;
			tr.price = mini.price;
			tr.quantity = mini.quantity;

			insertIntoArray(tr, &trade_array[tr.seller_id]);
			insertIntoArray(tr, &trade_array[tr.buyer_id]);

			quantity -= tr.quantity;
		}
	}
	if(quantity){
		struct request temp;
		temp.trader_id = trader_id;
		temp.price = price;
		temp.quantity = quantity;
		insertIntoMaxHeap(temp, &buy_pq[item_no]);
	}
	return;
}

void sell(int trader_id, int item_no, int quantity, int price){
	while(quantity>0&&((!isEmpty(&buy_pq[item_no]))&&max_price_max_heap(&buy_pq[item_no])>=price)){
		int buyer_has = quantity_of_max_price_max_heap(&buy_pq[item_no]);
		if(buyer_has>quantity){
			struct request maxi;
			showMax_max_heap(&buy_pq[item_no], &maxi);
			struct trade tr;
			tr.item_id = item_no;
			tr.seller_id = trader_id;
			tr.buyer_id = maxi.trader_id;
			tr.price = maxi.price;
			tr.quantity = quantity;

			insertIntoArray(tr, &trade_array[tr.seller_id]);
			insertIntoArray(tr, &trade_array[tr.buyer_id]);

			quantity -= tr.quantity;
			changeMaxQuantity_max_heap(&buy_pq[item_no], maxi.quantity - tr.quantity);
		}
		else{
			struct request maxi;
			extractMax(&buy_pq[item_no], &maxi);
			struct trade tr;
			tr.item_id = item_no;
			tr.seller_id = trader_id;
			tr.buyer_id = maxi.trader_id;
			tr.price = maxi.price;
			tr.quantity = maxi.quantity;

			insertIntoArray(tr, &trade_array[tr.seller_id]);
			insertIntoArray(tr, &trade_array[tr.buyer_id]);

			quantity -= tr.quantity;
		}
	}
	if(quantity){
		struct request temp;
		temp.trader_id = trader_id;
		temp.price = price;
		temp.quantity = quantity;
		insertIntoMinHeap(temp, &sell_pq[item_no]);
	}
	return;
}

void show_order_status(char message[]){
int i=1;
	while(i<=10){												// go thru each item
		char header[50] = "Item No. ";
		char num[50];
		sprintf(num, "%d", i);
		char newl[50] = "\n";
		char bsheader[50] = "Best Sell = ";
		char quantityheader[50] = "\tQuantity = ";
		char bbheader[50] = "Best Buy = ";
		char bb[50] = "No Record", bs[50] = "No Record";
		char bbquant[50] = "No Record", bsquant[50] = "No Record";
		if(!isEmpty(&sell_pq[i])){																		// print this item's best sell value by treating its sell queue as a min heap and finding the min value
			int bsnum = min_price_min_heap(&sell_pq[i]);
			int bsquant_num = quantity_of_min_price_min_heap(&sell_pq[i]);
			sprintf(bs, "%d", bsnum);
			sprintf(bsquant, "%d", bsquant_num);
		}
		if(!isEmpty(&buy_pq[i])){
			int bbnum = max_price_max_heap(&buy_pq[i]);
			int bbquant_num = quantity_of_max_price_max_heap(&buy_pq[i]);
			sprintf(bb, "%d", bbnum);
			sprintf(bbquant, "%d", bbquant_num);
		}
		strcat(message, header);
		strcat(message, num);
		strcat(message, newl);
		strcat(message, bbheader);
		strcat(message, bb);
		strcat(message, quantityheader);
		strcat(message, bbquant);
		strcat(message, newl);
		strcat(message, bsheader);
		strcat(message, bs);
		strcat(message, quantityheader);
		strcat(message, bsquant);
		strcat(message, newl);
		strcat(message, newl);
		i++;
	}
}

void show_trade_status(int trader_id, char message[]){
	int sz = getSize(&trade_array[trader_id]);
	int i=1;
	while(i<=sz){
		struct trade current;
		getTradeByIndex(&trade_array[trader_id], &current, i);
		char iidheader[50] = "Item Id = ";
		char sidheader[50] = "Seller id = ";
		char bidheader[50] = "Buyer id = ";
		char pheader[50] = "Price = ";
		char qheader[50] = "Quantity = ";
		char iid[50], sid[50], bid[50], p[50], q[50];
		sprintf(iid, "%d", current.item_id);
		sprintf(sid, "%d", current.seller_id);
		sprintf(bid, "%d", current.buyer_id);
		sprintf(p, "%d", current.price);
		sprintf(q, "%d", current.quantity);
		char newl[50] = "\n";
		strcat(message, iidheader);
		strcat(message, iid);
		strcat(message, newl);
		strcat(message, sidheader);
		strcat(message, sid);
		strcat(message, newl);
		strcat(message, bidheader);
		strcat(message, bid);
		strcat(message, newl);
		strcat(message, pheader);
		strcat(message, p);
		strcat(message, newl);
		strcat(message, qheader);
		strcat(message, q);
		strcat(message, newl);
		strcat(message, newl);
		i++;
	}
}

int main(int argc , char *argv[])   
{
	if (argc < 2) {				// wrong execution format
		fprintf(stderr,"ERROR, no port provided\n");
		exit(EXIT_FAILURE);
	}
	  int PORT = atoi(argv[1]);			// port number
	  init();			// initialises all sell and buy queues, logged in users ki list, trades array
	  char *message;
    int opt = TRUE;   
    int master_socket, addrlen, new_socket, client_socket[5],  max_clients = 5, activity, i, valread, sd;  // max clients allowed is 5, master_socket is used for listening and establishing connections, 
		// new socket for accepting a new connection, client_socket stores currently active connections
		// activity gives number of active sockets
		// sd just for going thru sockets
    int max_sd;   
    struct sockaddr_in address;   // socket address structure for master socket
         
    char buffer[2049];  //data buffer of 2K  
         
    //set of socket descriptors  
    fd_set readfds;   // sockfds of various connections, only needed for select function, its like bitset
       
     
    //initialise all client_socket[] to 0 so not checked
    i=0;  
    while(i<max_clients)   
    {   
        client_socket[i] = 0;
        i++;   
    }   
         
    //create a master socket  
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    //set master socket to allow multiple connections ,  
    //this is just a good habit, it will work without this  
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }  // setsockopt used to change settings of a socket. SOL_SOCKET says that we are changing socket options at the socket level (?), so_reuseaddr says that if connection closes and a delayed packet arrives, connection can be reopened temporarily to allow receiving that pkt. not the case usually though
     
    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   // bind to all local interfaces, so that it can listen later on any free interface
    address.sin_port = htons( PORT );   
         
    //bind the socket to port  
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("Listener on port %d \n", PORT);   
         
    //try to specify maximum of 5 pending connections while the master socket  
    if (listen(master_socket, 5) < 0)   // set master_socket into a passive mode where it listens for client requests
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    puts("Waiting for connections ...");   
    int runloop=0; 
    for(runloop=0;;runloop++)
    {   
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        //add child sockets to set  
        int i=0;
        while(i<max_clients)  
        {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;
            i++;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
				// checks for ready sockets
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket,
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d  \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   // inet_ntoa converts network byte type address to string
           
            //send connection successful message
       		  message = "Connection successful\nEnter username\n";
            
       		  int usiz, psiz;

            write(new_socket, message, strlen(message));		// write welcome msg and ask for username
                 
            char username[2049], password[2049];

            bzero(username,2049);
            usiz = read( new_socket , username, 2048);		// read username
            
            printf("Username = %s\n",username);

            message = "Enter password\n";										// ask for password
            write(new_socket , message , strlen(message));
            
            bzero(password,2049);
            psiz = read( new_socket , password, 2048);		// read pwd

            printf("Password = %s\n",password);
                 
            int ret = authenticate(username, password, usiz, psiz);		// authenticate this user, print error if any

            if(ret==-3){
							printf("Already logged in from somewhere else\nConnection closed\n");
							message = "Already logged in from somewhere else\nConnection closed\n";
							write(new_socket, message, strlen(message));			// write error message

							getpeername(new_socket , (struct sockaddr*)&address , (socklen_t*)&addrlen); // put client ip address in new_socket into address  
							printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  

							close( new_socket );   
            }
            else if(ret==-2){
            	printf("Wrong password\nConnection closed\n");
            	message = "Wrong password\nConnection closed\n";
            	write(new_socket, message, strlen(message));

				      getpeername(new_socket , (struct sockaddr*)&address , (socklen_t*)&addrlen);   
        		  printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  

        		  close( new_socket );   
            }
            else if(ret==-1){
            	printf("No such user exists\nConnection closed\n");
            	message = "No such user exists\nConnection closed\n";
            	write(new_socket, message, strlen(message));

				      getpeername(new_socket , (struct sockaddr*)&address , (socklen_t*)&addrlen);   
        		  printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  

        		  close( new_socket );   
            }
            else{
            	printf("Authentication successful\n");
            	message = "Authentication successful\nAvailable commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
            	write(new_socket, message, strlen(message));
	            //add new socket to array of sockets
	            i=0;  
	            while(i<max_clients)
	            {   
	                //if position is empty  
	                if( client_socket[i] == 0 )   
	                {   
	                    client_socket[i] = new_socket;   // put sockfd into client_socket array
	                    logged_in[ret] = i;								// set this user as logged in	
	                    printf("Adding to list of sockets as %d\n" , i);   
	                         
	                    break;   
	                }   
	                i++;
	            }
            }   
        }   
        i=0;
        //else its some IO operation on some other socket (read, write, client side close request)
        while(i<max_clients)
        {   
            sd = client_socket[i];   
            if (FD_ISSET( sd , &readfds))   	// if this socket had one of the above operations
            {   
                //Check if it was for closing , and also read the  
                //incoming message
                bzero(buffer,2049);  
                if ((valread = read( sd , buffer, 2048)) == 0)  // 0 meaning 0 bytes read 
                {   
                    //Somebody disconnected, get his details and print  
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );
                    int j=1;
                    while(j<=5){
                    	if(logged_in[j]==i){
                    		logged_in[j]=-1;
                    		break;
                    	}
                    	j++;
                    }
                    client_socket[i] = 0;   
                }   
                
                else 
                {
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';
                    
                    if((valread==4&&((!(strncmp("buy\n" , buffer , 4)))))||(valread==2&&((!(strncmp("1\n" , buffer , 2)))))){
												message="Enter item number (between 1 to 10 inclusive)\n";
												send(sd , message , strlen(message) , 0 );	// send vs write: same just can specify some flags in send, flag = 0 in send is same as write
												
												bzero(buffer,2049);
												read(sd,buffer,2048);
												int itemno = atoi(buffer);
												if(itemno>=1&&itemno<=10)
												{
													message="Enter quantity\n";
													send(sd , message , strlen(message) , 0 );

													bzero(buffer,2049);
													read(sd,buffer,2048);
													int quant = atoi(buffer);
													if(quant>0){
														message="Enter unit price\n";
															send(sd , message , strlen(message) , 0 );

															bzero(buffer,2049);
															read(sd,buffer,2048);
															int price = atoi(buffer);
															if(price>0){
																message="Operation Successful\nAvailable commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
																send(sd , message , strlen(message) , 0 );
																int tid;
																int j=1;
																while(j<=5){
																	if(logged_in[j]==i){
																		tid=j;
																		break;
																	}
																	j++;
																}
																buy(tid, itemno, quant, price);
															}
															else{
																message="Invalid price\nAvailable commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
															send(sd , message , strlen(message) , 0 );
															}
													}	
													else{
														message="Invalid quantity\nAvailable commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
														send(sd , message , strlen(message) , 0 );
													}
												}
												else{
													message="Invalid item number\nAvailable commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
													send(sd , message , strlen(message) , 0 );
												}
                    }

                    else if((valread==5&&(!(strncmp("sell\n" , buffer , 5))))||(valread==2&&((!(strncmp("2\n" , buffer , 2)))))){
                    message="Enter item number (between 1 to 10 inclusive)\n";
                		send(sd , message , strlen(message) , 0 );
                		
                		bzero(buffer,2049);
					          read(sd,buffer,2048);
                		int itemno = atoi(buffer);
                		if(itemno>=1&&itemno<=10)
                		{
                			message="Enter quantity\n";
                			send(sd , message , strlen(message) , 0 );

                			bzero(buffer,2049);
					    	      read(sd,buffer,2048);
                			int quant = atoi(buffer);
                			if(quant>0){
                				message="Enter unit price\n";
                    			send(sd , message , strlen(message) , 0 );

                    			bzero(buffer,2049);
						    	        read(sd,buffer,2048);
                    			int price = atoi(buffer);
                    			if(price>0){
                    				message="Operation Successful\nAvailable commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
                					  send(sd , message , strlen(message) , 0 );
                    				int tid;
                    				int j=1;
                    				while(j<=5){
                    					if(logged_in[j]==i){
                    						tid=j;
                    						break;
                    					}
                    					j++;
                    				}
                    				sell(tid, itemno, quant, price);
                    			}
                    			else{
                    				message="Invalid price\nAvailable commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
                					  send(sd , message , strlen(message) , 0 );
                    			}
                			}	
                			else{
                				message="Invalid quantity\nAvailable commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
                				send(sd , message , strlen(message) , 0 );
                			}
                		}
                		else{
                			message="Invalid item number\nAvailable commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
                			send(sd , message , strlen(message) , 0 );
                		}
                    }

                    else if((valread==13&&(!(strncmp("order_status\n" , buffer , 13))))||(valread==2&&((!(strncmp("3\n" , buffer , 2)))))){
                    	bzero(buffer,2049);
                    	show_order_status(buffer);
                    	char success[1000] = "Available commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
                    	strcat(buffer, success);
                    	send(sd , buffer , strlen(buffer) , 0 );
                    }

                    else if((valread==13&&(!(strncmp("trade_status\n" , buffer , 13))))||(valread==2&&((!(strncmp("4\n" , buffer , 2)))))){
												int tid;
												int j=1;
												while(j<=5){
													if(logged_in[j]==i){
														tid=j;
														break;
													}
													j++;
												}
												bzero(buffer,2049);
												if(isEmpty_t(&trade_array[tid]))
													strcpy(buffer, "No Record\n");
												else
													show_trade_status(tid, buffer);
												char success[1000] = "Available commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
												strcat(buffer, success);

												send(sd , buffer , strlen(buffer) , 0 );

                    }

                    else if((valread==7&&(!(strncmp("logout\n" , buffer , 7))))||(valread==2&&((!(strncmp("5\n" , buffer , 2)))))){
												message="Connection closed\n";
												send(sd , message , strlen(message) , 0 );

												getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);   
												printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  

												close( sd );
												int jj=1;
												while(jj<=5){
														if(logged_in[jj]==i){
															logged_in[jj]=-1;
															break;
														}
														jj++;
													}   
												client_socket[i] = 0;
                    }
										
                    else{
                    	message="Command not recognized\nAvailable commands:\n1. buy\n2. sell\n3. order_status\n4. trade_status\n5. logout\nPlease input the number or the command name\n";
                		  send(sd , message , strlen(message) , 0 );
                    }
                }   
            }   
            i++;
        }  
    }   
         
    return 0;   
} 
