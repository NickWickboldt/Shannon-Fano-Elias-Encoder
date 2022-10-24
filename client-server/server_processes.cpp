#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <sys/wait.h>
#include <cmath>
#include <string>
#include <string.h>

using namespace std; 
void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0);
}
double calculation(double f, double cf){   // using location in probabilty vector to calculate fbar
  double temp; 
  temp = (f*.5)+cf;
  return temp; 
}
int lengthFunction(double f){   // returns length of desired code using log and ceil
  f = ceil(log2((1/f))+1);
  return f; 
}
string toBinary(double x, int l){   //converts decimal to binary using l most significant digits
  int temp; 
  string result; 
  for(int i =0; i<l; i++){
    if(x>=1){
      x-=1; 
      x*=2; 
    } 
    else
      x *=2;
    temp = int(x); 
    result+=to_string(temp); 
  }
  return result; 
}
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    signal(SIGCHLD, fireman); 
    if (argc < 2)
    {
        std::cerr << "ERROR, no port provided\n";
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket";
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR on binding";
        exit(1);
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (true)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0)
        {
            if (newsockfd < 0)
            {
                std::cerr << "ERROR on accept";
                exit(1);
            }
            int symbolSize;
            n = read(newsockfd, &symbolSize, sizeof(int));//reading size of symbol
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket in server 1-a";
                exit(1);
            }
            char *symbolBuffer = new char[symbolSize+1];
            bzero(symbolBuffer, symbolSize+1);
            n = read(newsockfd, symbolBuffer, symbolSize);//reading symbol
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket in server 1-b";
                exit(1);
            }
            int frequencySize; 
            n = read(newsockfd, &frequencySize, sizeof(int));//reading size of frequency
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket in server 2-a";
                exit(1);
            }
            double frequencyBuffer; 
            n = read(newsockfd, &frequencyBuffer, frequencySize);//reading frequency
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket in server 2-b";
                exit(1);
            }
            int cumulativeFrequencySize; 
            n = read(newsockfd, &cumulativeFrequencySize, sizeof(int));//reading cumulative frequency size
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket in server 3-a";
                exit(1);
            }
            double cumulativeFrequencyBuffer; 
            n = read(newsockfd, &cumulativeFrequencyBuffer, cumulativeFrequencySize);//reading cumulative frequency
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket in server 3-b";
                exit(1);
            }
            //std::cout << symbolBuffer << " " << frequencyBuffer << " " << cumulativeFrequencyBuffer << std::endl; 
            double preCode = calculation(frequencyBuffer, cumulativeFrequencyBuffer);
            int l = lengthFunction(frequencyBuffer);
            string code = toBinary(preCode,l);
            
            //std::cout << preCode << " " << l << " " << code << " " << symbolBuffer << std::endl; 
            string messageT = symbolBuffer+code; 
            //std:: cout<< message <<endl;
            int stringSize = messageT.size();
            char message[stringSize+1]; 
            strcpy(message,messageT.c_str());
            
            n = write(newsockfd, &stringSize, sizeof(int));//writing size of message
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            n = write(newsockfd, message, stringSize);//writing message
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            close(newsockfd);
            delete[] symbolBuffer;
            _exit(0);
        }
    }
    close(sockfd);
    return 0;
}
