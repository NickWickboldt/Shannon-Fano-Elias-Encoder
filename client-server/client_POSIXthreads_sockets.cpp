#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <string.h>
#include <pthread.h>
#include <vector>


using namespace std;

struct item{
  char sym;
  double freq;
  double cFreq; 
  char *server; 
  char *portno; 
  string result;  
};

void * routine(void *ptr){
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  struct item *newPtr = (struct item *)ptr; 
  portno = atoi(newPtr->portno);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    cerr << "ERROR opening socket";
  server = gethostbyname(newPtr->server);
  if (server == NULL)
  {
    cerr << "ERROR, no such host\n";
    exit(0);
  }
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    cerr << "ERROR connecting";
    exit(1);
  }
  int sSym=1; 
  char symbol = newPtr->sym;
  n = write(sockfd, &sSym, sizeof(int));//writing size of symbol
  if (n < 0)
  {
    cerr << "ERROR writing to socket";
    exit(1);
  }
  n = write(sockfd, &symbol, 1);//writing symbol
  if (n < 0)
  {
    cerr << "ERROR writing to socket";
    exit(1);
  }
  double frequency = newPtr->freq; 
  int sFreq = sizeof(frequency); 
  n = write(sockfd, &sFreq, sizeof(int));//writing size of frequency
  if (n < 0)
  {
    cerr << "ERROR writing to socket";
    exit(1);
  }
  n = write(sockfd, &frequency, sFreq);//writing frequency
  if (n < 0)
  {
    cerr << "ERROR writing to socket";
    exit(1);
  }
  double cFrequency = newPtr->cFreq;
  int sCFrequency = sizeof(cFrequency);
  n = write(sockfd, &sCFrequency, sizeof(int));//writing size of cumulative frequency
  if (n < 0)
  {
    cerr << "ERROR writing to socket";
    exit(1);
  }
  n = write(sockfd, &cFrequency, sCFrequency);//writing cumulative frequency
  if (n < 0)
  {
    cerr << "ERROR writing to socket";
    exit(1);
  }
  int size;
  n = read(sockfd, &size, sizeof(int));//reading size of buffer
  if (n < 0)
  {
    cerr << "ERROR reading from socket in client";
    exit(1);
  }

  char *buffer = new char[size + 1];
  bzero(buffer, size + 1);
  n = read(sockfd, buffer, size);
  string s(buffer); 
  newPtr->result = s; 
  delete [] buffer;
   close(sockfd);
   return NULL; 
}

int main(int argc, char *argv[]){
  string str,temp,t; 
  getline(cin,str);
  sort(str.begin(), str.end());
  for(int i =0; i<str.size(); i++){
      if(temp.find(str[i])==string::npos){
          temp+=str[i];
      }
  }
  //cout << str << " " << temp.size(); 
  int counter =0;
  double freqT=0,cFreqT=0; 
  struct item a[temp.size()];
  if (argc < 3)
  {
    cerr << "usage " << argv[0] << "hostname port" << endl;
    exit(0);
  }
  for(int i =0; i<str.size();i++){
    freqT++;
    if(str[i]!=str[i+1]){
      //freqT++; 
      a[counter].freq= ((freqT)/str.size()); 
      a[counter].sym= str[i];
      a[counter].cFreq = (cFreqT/str.size()); 
      cFreqT+=freqT;
      freqT=0; 
      counter++; 
    }
  }
  
  pthread_t tid[temp.size()];
  
  for(int i =0; i<temp.size(); i++){
    a[i].server = argv[1]; 
    a[i].portno = argv[2];
    if(pthread_create(&tid[i],NULL,routine,&a[i])){
      cout<<"Error";
      return 1; 
    }
  }
  for(int i =0; i<counter;i++){
    pthread_join(tid[i],NULL);
  }
  cout<< "SHANNON-FANO-ELIAS Codes:"<<endl<<endl;
  
  for(int i=0; i<temp.size(); i++){
      cout << "Symbol "<< a[i].result[0] << ", Code: "<< a[i].result.substr(1)<< endl; 
  }
  
}
