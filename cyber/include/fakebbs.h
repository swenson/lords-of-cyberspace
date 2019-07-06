#if __APPLE__
#include <struct.h>
#else
#define _POSIX_C_SOURCE 200112L
#define __USE_POSIX199309
#define __USE_POSIX200112
#define _XOPEN_SOURCE 600
#include "struct.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
//long int lrand48(void);
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


#define MAXLONG 2147483647
#define MNMSIZ 32
#define EXPORT
#define NTERMS 32

/* volatile per-user info */
struct user {
  int state;  /* module state */
  int substt; /* module sub-state */
  int flags;
  int crdrat; /* credit rate? */
};

#define WSGCSU 1
#define INVISB 2
#define NOGLOB 4
#define INJOIP 8

extern struct user *usrptr,      /* current user */
         user[NTERMS]; /* all users currently logged in */

#define INPSIZ 128          /* user input buffer size */
extern char input[INPSIZ],  /* user input */
       *margv[INPSIZ]; /* input split by spaces */
extern
int margc, /* how many margvs there are */
    inplen, /* input length */
    status; /* btusts? */



#define UIDSIZ 30
#define TPCSIZ 51
#define MTXSIZ   400          /* maximum message-text string size          */

struct message {
  char to[UIDSIZ + 1];
  char from[UIDSIZ + 1];
  char topic[TPCSIZ + 1];
  int flags;
  unsigned crdate;
  unsigned crtime;
  long msgno;
  char userto[UIDSIZ + 1];
  char text[MTXSIZ];
};

#define vdasiz (sizeof(struct message))
extern char *vdaptr, vdatmp[vdasiz];
extern int vfyadn;

/* persistent system variables */
struct sysvbl {
  long msgtot;
};
extern struct sysvbl sv;             /* sys-variables record instance for updates */


#define PRFBUFSIZE 16384
extern char prfbuf[PRFBUFSIZE];
extern int usrnum;

/* module interface block               */
struct module {
  char descrp[MNMSIZ];          /*    description for main menu         */
  int (*lonrou)();              /*    user logon supplemental routine   */
  int (*sttrou)();              /*    input routine if selected         */
  void (*stsrou)();             /*    status-input routine if selected  */
  int (*injrou)();              /*    "injoth" routine for this module  */
  int (*lofrou)();              /*    user logoff supplemental routine  */
  void (*huprou)();             /*    hangup (lost carrier) routine     */
  void (*mcurou)();             /*    midnight cleanup routine          */
  void (*dlarou)();             /*    delete-account routine            */
  void (*finrou)();             /*    finish-up (sys shutdown) routine  */
} **module;


/*  user account info (persisted) */
struct usracc {
  char userid[UIDSIZ];
  int ansifl; /* ansi flag */
  int scnfse; /* full-screen editor length */
};

#define ANSON 1

extern
struct usracc usracc[NTERMS], /* user accounting block array               */
         *usaptr;        /* user accounting block pointer for usrnum  */
extern
int usrnum;                   /* global user-number (channel) in effect    */

extern int bturno;
extern int genbb;
extern int othusn;

extern struct usracc *othuap; /* gen purp other-user accounting data ptr   */

#define CYCLE 0

struct tag_downloader_maybe {
  char tagspc[18];
  char tshmsg[81];
  void *tshndl;
  char fname[1024];
};

extern struct tag_downloader_maybe ftgptr[1];

#define TSLENG 1024

#define TSHDSC 1
#define TSHVIS 2
#define TSHBEG 5
#define TSHFIN 8

extern char tshmsg[81];
extern struct tag_downloader_maybe ftfscb[1];

short today();
void stzcpy(char*, char*, int);
void stpans(char *ans);
void setmem(void *mem, int size, int dat);
void setmbk(FILE *f);
int sameto(char *a, char *b);
int sameas(char *a, char *b);
void rstrin();
struct usracc *uacoff(int usrnum);
int ynopt(int opt);
char *stgopt(int str);
char *stgoptnl(int str);
char *spr(char *fmt, int x);
void shocst(char *dst, char *fmt, char *arg);

void randomize();
void rstmbk();
long LRAND(long n);

void prf(char *fmt, ...);
void prfmsg(int msg, ...);

void setbtv(int);

void rtkick(int, void (*x)(void));
void rstrxf();
void rstbtv();

int register_module(struct module*);
void parsin();
void outprf(short user_num);

void clrprf();
FILE* opnmsg(char *fname);
short numopt(short, short, short);
unsigned short now();

char *nctime(unsigned short);
char *ncdate(short);
long lngopt(long score, int x, long max);
char *l2as(long);

void invbtv(void *, size_t);

long hrtval();
int haskey(char *key);
char *gmdnam(char *);
char *getmsg(int msg);
void ftgsbm(char *s);
int ftgnew();
int fsdroom(int, char *, int);
int fsdrft();
void fsdfxt(int, char *, int);
void fsdego(int a, void (*f)(int));
void fsdbkg(int);
void fsdapr(char*, int, char *);
void dfsthn();
void dclvda(int);
short datofc(short);
short cofdat(short);
void clsmsg(FILE *f);
#define catastro(fmt, ...) { printf(fmt, ##__VA_ARGS__); putchar(10); exit(1); }
#define acqbtv(recp,key,keynum) obtbtv(recp,key, keynum,5)
int obtbtv(void *recptr, void *key, int keynum,
           int obtopt); /* acquire? Btrieve operations utiltiy */
int btuxnf(int chan, int xon, int xoff, ...);
int btuxmn(int chan, char *datstg);
int btumil(int chan, int maxinl);
int btuinj(int chan, int status);
int btuchi(int chan, char (*rouadr)(int chan, int c));
char *alczer(unsigned nbytes);
int addcrd(char *keyuid, char *tckstg, int real); /*add credits to account */


struct flddef {
  char c;
  int index;
  int offset;
  void *v;
};

#define CVTFLD_CHAR 'c'
#define CVTFLD_SHORT 's'
#define CVTFLD_END '\0'
#define CVTFLD_LONG 'l'
#define CVTFLD_STRUCT 't'
#define CVTPACKED 'p'
#define CVTSERVER 'v'
#define CHAN_NUL 'n'

void cvtData(char *data, void *struc, int size, struct flddef *fields, char p, char s, char n);

int bbs_sendmsg(struct message *msg, char *to);