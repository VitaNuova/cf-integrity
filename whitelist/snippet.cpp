#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#include <cxxabi.h>
#include <string>
#include <iostream>

using namespace std;

int backtrace() {
   void* buffer[256];
   int backtrace_length = backtrace(buffer, 256);
   if(backtrace_length == 0) {
      fprintf(stderr, "Empty stack trace, possibly corrupted stack\n");
      exit(1);
   } else {
      printf("Length is %d\n", backtrace_length);	
   }
   char** symbols_buffer = backtrace_symbols(buffer, backtrace_length);
 
   size_t fnamesize = 256;
   char* fname = (char*)malloc(fnamesize);

   unsigned hash = 0;
   for(unsigned i = 1; i < backtrace_length; i++) {
      char* bname = 0;
      char* boffset = 0;
      char* eoffset = 0;
      for(char* l = symbols_buffer[i]; *l; l++) {
         if(*l == '(') {
	    bname = l;
	 } else if(*l == '+') {
	    boffset = l;
	 } else if(*l == ')' && boffset) {
	    eoffset = l;
         }
      }
      *bname++ = '\0';
      *boffset++ = '\0';
      *eoffset = '\0';
      int status;
      char* ret = abi::__cxa_demangle(bname, fname, &fnamesize, &status);
      if(status == 0) {
	 string name_with_args(ret);
	 unsigned index = name_with_args.find_first_of('(');
	 string name = name_with_args.substr(0, index);
	 cout << "Name without arguments: " << name << endl;
	 for(char letter: name) {
	    hash += letter;  
	 }	
      }
   }  
   cout << "Hash of this trace: " << hash << endl; 
   free(fname);
   free(symbols_buffer);

return hash;
}

void readKey() {
   backtrace();
}

void encrypt() {
        readKey();
}

void decrypt() {
        readKey();
}

void calc() {}
void notify() {}
void display() {
}

void operation1() {
        calc();
        notify();
}

void operation2() {
        encrypt();
        decrypt();
}

void loadUI() {
}

int main(void) {
        operation1();
        operation2();
return 0;
}



