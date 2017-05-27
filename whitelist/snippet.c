#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#include <string.h>

int hash_backtrace() {
   void* buffer[256];
   int backtrace_length = backtrace(buffer, 256);
   if(backtrace_length == 0) {
      fprintf(stderr, "Empty stack trace, possibly corrupted stack\n");
      exit(1);
   } else {
      printf("Length is %d\n", backtrace_length);	
   }
   char** symbols_buffer = backtrace_symbols(buffer, backtrace_length);
 
   unsigned hash = 0;
   for(unsigned i = 1; i < backtrace_length; i++) {
      char* bname = 0;
      char* boffset = 0;
      for(char* l = symbols_buffer[i]; *l; l++) {
         if(*l == '(') {
	    bname = l;
	 } else if(*l == '+') {
	    boffset = l;
	    break;
         }
      }
      *bname++ = '\0';
      *boffset++ = '\0';
      printf("%s\n", bname);
      if(bname[0] != '_') {
         for(unsigned i = 0; bname[i]; i++) {
            hash += bname[i];
         }
      }	   
   }  
   printf("Hash of this trace: %d.\n", hash); 
   free(symbols_buffer);

return hash;
}

