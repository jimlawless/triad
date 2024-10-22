#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

char *tri_next;
int tri_end;
int tri_error;

#define TRI_MAX (17576)
typedef void(*TRI_PROC)(void);
TRI_PROC tri_procs[TRI_MAX];
char *tri_user_procs[TRI_MAX];



int tri_interpret(void);
int tri_three2int(char *);
void tri_init(void);

   // The run-time code for the built-in procs
void do_unknown_proc(void);
void do_invoke(void);
void do_block_comment(void);
void do_rem(void);
void do_print(void);
void do_newline(void);
void do_def(void);

char *tri_skipwhite(char *);

int main(int argc,char **argv) {
   char *buff;
   FILE *fp;   
  
   tri_init();

   buff=(char *)malloc(30000);
   if(buff==NULL) {
      fprintf(stderr,"Cannot allocate buffer\n");
      return 1;
   }
   fp=fopen("triad.txt","rb");
   if(fp==NULL) {
      fprintf(stderr,"Cannot open input file triad.txt\n");
      return 1;
   }
   
   fread(buff,1,30000,fp);
   fclose(fp);
   
   tri_next=buff;
   tri_interpret();   
   
}

void tri_init(void) {
   int i;
   tri_end=tri_three2int("end");
   tri_error=0;
   for(i=0;i<TRI_MAX;i++) {
      tri_procs[i]=do_unknown_proc;
      tri_user_procs[i]=NULL;
   }
      // Add our builtin procs to the runtime   
   tri_procs[tri_three2int("inv")]=do_invoke;
   tri_procs[tri_three2int("rem")]=do_rem;
   tri_procs[tri_three2int("com")]=do_block_comment;
   tri_procs[tri_three2int("prt")]=do_print;
   tri_procs[tri_three2int("nwl")]=do_newline;
   tri_procs[tri_three2int("def")]=do_def;
}

   // Default builtin proc.  If we execute a three-letter word that
   // Isn't yet defined, this function will be invoked.
void do_unknown_proc(void) {
   char *p;
   p=tri_next-3;
   fprintf(stderr,"Error: Unknown procedure %c%c%c.\n",*p,*(p+1),*(p+2));
   tri_error=1;
}

   // Invoke a user-defined procedure
void do_invoke(void) {
   int proc;
   char *targ;
   char *p=tri_next;
   p=tri_skipwhite(p);
   proc=tri_three2int(p);
   if(proc==-1) {
      fprintf(stderr,"In INV expecting user-defined proc name. Found %c%c%c\n",*p,*(p+1),*(p+2));
      tri_error=1;
      return;
   }
   targ=tri_user_procs[proc];
   if(targ!=NULL) {
      tri_next=targ;
      if(tri_interpret()==-1) {
         return;
      }
      tri_next=p+3;
   }
   else {
      fprintf(stderr,"In INV user-defined proc %c%c%c is empty.\n",*p,*(p+1),*(p+2));
      tri_error=1;
   }
}

void do_block_comment(void) {
   char *p,sentinel;
   p=tri_next;
   p=tri_skipwhite(p);
   if(!*p) {
      fprintf(stderr,"Error: COM end of input before initial sentinel.");
      tri_error=1;
      return;
   }
   sentinel=*p;
   p++;
   while( (*p)&&( (*p)!=sentinel)) {
      p++;
   }
   if(!*p) {
      fprintf(stderr,"Error: COM end of input before closing sentinel character.");
      tri_error=1;
      return;
   }
   tri_next=p+1;
}

   // Consume input text until '\n' is encountered.
void do_rem(void) {
   char *p=tri_next;
   while((*p)&&(*p!='\n'))
      p++;
   if(*p) {
      tri_next=p+1;
   }
   else {
      tri_next=p;
   }
}

   // Display everything between sentinel characters.
void do_print(void) {
   char *p,sentinel;
   p=tri_next;
   p=tri_skipwhite(p);
   sentinel=*p;
   p++;
   while((*p)&&(*p!=sentinel)) {
      printf("%c",*p);
      p++;
   }
   if(*p) {
      tri_next=p+1;
   }
   else {
      tri_next=p;
   }
}

   // Print a newline.
void do_newline(void) {
   printf("\n");
}

   // Create a user-defined proc
void do_def(void) {
   int proc;
   char sentinel;
   tri_next=tri_skipwhite(tri_next);
   proc=tri_three2int(tri_next);
   if(proc==-1) {
      fprintf(stderr,"Bad format in user-defined proc name %c%c%c\n",*tri_next,*(tri_next+1),*(tri_next+2));
      tri_error=1;
      return;
   }
   tri_next=tri_skipwhite(tri_next+3);
   if(!*tri_next) {
      fprintf(stderr,"In DEF.  End of input before initial sentinel character.\n");
      tri_error=1;
      return;
   }
   sentinel=*tri_next;
   tri_next++;
   tri_user_procs[proc]=tri_next;
   while(*tri_next) {
      if(*tri_next==sentinel)
         break;      
      tri_next++;
   }   
   if(*tri_next!=sentinel) {
      fprintf(stderr,"In DEF.  End of input before final sentinel character.\n");
      tri_error=1;
      return;
   }
   tri_next++;
}


   // Utility function.  Skip over whitespace characters.
char *tri_skipwhite(char *s) {
   char c;
   while((c=*s)) {
      if((c!=' ')&&(c!='\t')&&(c!='\r')&&(c!='\n'))
         break;
      s++;
   }
   return s;
}

int tri_interpret(void) {
   int proc;
   for(;;) {
      if(tri_error)
         return -1;
      tri_next=tri_skipwhite(tri_next);      
      proc=tri_three2int(tri_next);
      if(proc==-1) {
         fprintf(stderr,"Unknown proc %c%c%c\n",*tri_next,*(tri_next+1),*(tri_next+2));
         return -1;
      }            
      if(proc==tri_end)
         return tri_end;      
      tri_next+=3;
      (tri_procs[proc])();
   }
}


   // Convert a three-letter word ( floored to
   // lower-case ) to an int equivalent.  The
   // conversion treats the three-letter word as
   // a base-26 number with the letter 'a' representing
   // zero and the letter 'z' representing 25.

int tri_three2int(char *s) {
   int a;
   int i;
   a=0;
   for(i=0;i<3;i++) {
      if(!isalpha(*s)) {
         return -1;
      }
      a=(a*26)+(tolower(*s)-'a');
      s++;
   }
   return a;   
}

