#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

void hash_backtrace(unsigned hashes[], int length) {
	void* buffer[256];
	int backtrace_length = backtrace(buffer, 256);
	if(backtrace_length == 0) {
		fprintf(stderr, "Empty stack trace, possibly corrupted stack\n");
		exit(1);
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
		if(bname[0] != '_') {
			for(unsigned i = 0; bname[i]; i++) {
				hash += bname[i];
			}
		}	   
	}  
	
	printf("Hash of this trace: %d.\n", hash);
	
	int valid = 0;
	for(unsigned i = 0; i < length; i++){
		if(hash == hashes[0]){
			fprintf(stderr, "Trace is OK.\n");
			valid = 1;
		}
	}
	
	free(symbols_buffer);
	
	if(valid = 0){
		fprintf(stderr, "Trace is corrupted.\n");
		exit(1);
	}
	
}

void xhash_backtrace(int expected_hash) {
	void* buffer[256];
	int backtrace_length = backtrace(buffer, 256);
	if(backtrace_length == 0) {
		fprintf(stderr, "Empty stack trace, possibly corrupted stack\n");
		exit(1);
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
		if(bname[0] != '_') {
			for(unsigned i = 0; bname[i]; i++) {
				hash += bname[i];
			}
		}	   
	}  
	printf("Hash of this trace: %d.\n", hash); 
	if(hash != expected_hash) {
		fprintf(stderr, "Trace is corrupted.\n");
		exit(1);
	} else {
		fprintf(stderr, "Trace is OK.\n");
	}
	free(symbols_buffer);

}

