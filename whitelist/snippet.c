#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "backtrace.h"
#include "backtrace-supported.h"

static void* state;

struct info {
   char* function;
};

struct bdata {
   struct info* all;
   size_t index;
   size_t max;
};

static void error_callback_create(void* data, const char* msg, int errnum) {
   fprintf(stderr, "%s", msg);
   if(errnum > 0) {
      fprintf(stderr, ": %s", strerror(errnum));
   }
   fprintf(stderr, "\n");
   exit(EXIT_FAILURE);
}

static int success_callback(void *vdata, uintptr_t pc,
              const char *filename, int lineno, const char *function) {
  struct bdata *data = (struct bdata *) vdata;
  struct info *p;

  if(data->index >= data->max) {
      fprintf (stderr, "Success callback: callback called too many times\n");
      exit(1);
  }

  p = &data->all[data->index];
  
  if(function == NULL) {
     p->function = NULL;
  } else {
     p->function = strdup(function);
     if(p->function == NULL) {
        fprintf(stderr, "Error. Function null.\n");
        exit(1);
     }
  }
  ++data->index;

  return 0;
}

static void error_callback(void *vdata, const char *msg, int errnum) {
   struct bdata *data = (struct bdata *) vdata;
   fprintf (stderr, "%s", msg);
   if (errnum > 0) {
     fprintf (stderr, ": %s", strerror (errnum));
   }
   fprintf (stderr, "\n");
}

void hash_backtrace(unsigned hashes[], int length) {
        FILE* f = fopen("log.txt", "a");
        if(f == NULL) {
           printf("Error opening file!\n");
           exit(1);
        }  
       
        /*fprintf(f, "Length of possible hashes %d", length);
        for(unsigned i = 0; i < length; i++) {
           fprintf(f, "\npossible hash %d", hashes[i]);
        }
        fprintf(f, "\n");*/
       
        struct info all[20];  
        struct bdata trace_data;
        trace_data.all = &all[0];
        trace_data.index = 0;
        trace_data.max = 20;
        
        state = backtrace_create_state("", BACKTRACE_SUPPORTED, error_callback_create, NULL);
        int collect_result = backtrace_full(state, 0, success_callback, error_callback, &trace_data);
        
        if(collect_result != 0) {
           fprintf(stderr, "Unexpected return value from backtrace_full %d\n. Exiting.", collect_result);
	   exit(1);
        }

        unsigned hash = 0;
        for(unsigned i = 0; i < trace_data.index; i++) {
              if(trace_data.all[i].function != NULL) {
                 for(unsigned j = 0; trace_data.all[i].function[j]; j++) {
                    hash += trace_data.all[i].function[j];
                 } 
              }
        }

	fprintf(f, "Hash of this trace: %d.\n", hash);
	
	int valid = 0;
	for(unsigned i = 0; i < length; i++) {
		if(hash == hashes[i]){
		        fprintf(f, "Trace is OK.\n");
			valid = 1;
		}
	}
	
	if(valid == 0){
		fprintf(f, "Trace is corrupted.\n");
		exit(1);
	}
	fclose(f);
}




