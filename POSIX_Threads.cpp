#include <pthread.h>
#include <vector>
#include <iostream>
#include <string>
#include <string.h>
#include <cmath>
#include <bitset>
using namespace std; 

struct item
{
  char sym;
  double freq; 
  int position; 
  string result;
  vector <double> freqList; //used for calc of fbar
};

double calculation(int p, double f, vector<double> list){   // using location in probabilty vector to calculate fbar
  if(p==0){
    return (f*.5);
  }
  else{
    double temp;
    for(int i =0; i<p; i++){
      temp+= list[i];
    }
    temp += (f*.5);
    return temp; 
  }
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

void * routine(void *ptr){
  struct item *newPtr = (struct item *)ptr; 
  double preCode = calculation(newPtr->position,newPtr->freq,newPtr->freqList);
  int l = lengthFunction(newPtr->freq);
  string code = toBinary(preCode,l);
  newPtr->result = code; 
  return NULL; 
}

int main(){
  vector <char> symbols; 
  vector <double> prob; 
  string temp, temp1, temp2; 
  
  getline(cin,temp1);
  getline(cin,temp2);

  for(int i =0; i<temp1.length(); i++){
    if(!isspace(temp1[i]))
      symbols.push_back(temp1[i]);
  }
  for(int i =0; i<temp2.length();i++ ){
    if(isspace(temp2[i])){
      prob.push_back(stod(temp));
      temp="";
    }
    else{
      temp+=temp2[i];
    }
  }prob.push_back(stod(temp));

  pthread_t tid[symbols.size()];
  struct item a[symbols.size()];
  for(int i =0; i<symbols.size(); i++){
    a[i].sym= symbols[i];
    a[i].freq=prob[i];
    a[i].position=i; 
    a[i].freqList=prob; 
    if(pthread_create(&tid[i],NULL,routine,&a[i])){
      cout<<"Error";
      return 1; 
    }
  }
  for(int i =0; i<symbols.size();i++){
    pthread_join(tid[i],NULL);
  }
  cout << "SHANNON-FANO-ELIAS Codes:"<< endl << endl;
  for(int i =0; i<symbols.size();i++){
    cout << "Symbol " << a[i].sym<< ", Code: "<< a[i].result << endl; 
  }
  return 0; 
}
