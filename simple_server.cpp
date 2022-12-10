#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "http_server.hh"
#include <condition_variable>
#include <mutex>
#include <netinet/in.h>
#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

queue<int> q1;
pthread_mutex_t lock_1=PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t cond_full=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_empty=PTHREAD_COND_INITIALIZER;

socklen_t clilen;
struct sockaddr_in serv_addr, cli_addr;
void error(char *msg) {
  perror(msg);
  exit(1);
}
 void * master_thread(void * arg)
 {
    cout<<"i am in master thread\n";
    int my_arg = *((int *) arg);
    while(1)
    {    
          //cout<<*lock_1;
          //cout<<"abhijeet sing mandal====$$$$$$$$$$$$$$$$$$4\n";
           int fd=accept(my_arg, (struct sockaddr *)&cli_addr, &clilen);
        pthread_mutex_lock(&lock_1);
          //cout<<"*************aabhadijfja*******************\n";
        //int fd=accept(my_arg, (struct sockaddr *)&cli_addr, &clilen);
          q1.push(fd);
        //pthread_mutex_unlock(&lock_1);
        pthread_cond_signal(&cond_empty);
        pthread_mutex_unlock(&lock_1);
       
    }


 }
 void * worker_thread_handler(void *arg)
 {
    
    int my_arg = *((int *) arg);
    cout<<"worker thread is created of id = "<<my_arg<<endl;
    while(1)
    { 
        
       
        pthread_mutex_lock(&lock_1);
         if(q1.empty())//if queue is empty
            pthread_cond_wait(&cond_empty,&lock_1);
          
        //cout<<"i am in worker thread after wait\n";
      
         int x=q1.front();
         
         q1.pop();
         
        //cout<<"AASJFAJFAJF////////********value of x before unlock="<<x<<endl;
       
        pthread_mutex_unlock(&lock_1);
        
        char buffer[2048];
        int n;
         //printf("i am in thread now at start of handler function\n");
   
        bzero(buffer, 2048);
     

        //cout<<"abhijeet is nice guy\n";
        //cout<<"value of x is ="<<x<<endl;
        n = read(x, buffer, 2048);
        cout<<n<<endl;
        cout<<"i am done\n";
        if(n==0)
         {
           printf("nothing is passed from client\n");
           pthread_exit(0);
         }
         if (n < 0)
           error("ERROR reading from socket");
      printf("Here is the message: %s", buffer);
      
      HTTP_Response *response =handle_request(buffer);
      string s=response->get_string();
      char buffer1[2048];
      int k;
      for(k=0;k<s.length();k++)
      {
        buffer1[k]=s[k];
      }
      buffer1[k]='\0';
     // cout<<buffer1;
 
        n = write(x, buffer1, k+2);
         if (n < 0)
           error("ERROR writing to socket");
       delete response;  
       
      close(x);
      // while()
      
     	int a=0;
    }
    
    }
 
 

int main(int argc, char *argv[]) {
  
  int i;
 
  
  int sockfd, newsockfd, portno;
  //socklen_t clilen;
  char buffer[256];
  //struct sockaddr_in serv_addr, cli_addr;
  int n;

  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* listen for incoming connection requests */

     listen(sockfd, 10);
     clilen = sizeof(cli_addr);
   
   //int thread_arg[10];


   pthread_t thid;
     int master_thread_arg=sockfd;
    pthread_create(&thid,NULL,master_thread,&master_thread_arg);// creating master thread
     
    pthread_t wthid[5];
    int worker_thread_arg[5];
    //int i;
    for(i=0;i<3;i++)  // creating worker threads
    {
       worker_thread_arg[i]=i;
       pthread_create(&wthid[i],NULL,worker_thread_handler,&worker_thread_arg[i]);
    } 

    while(1)
    {    

       
      
       
    }
    
  

 

  return 0;
}
