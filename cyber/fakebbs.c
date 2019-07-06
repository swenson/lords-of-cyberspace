#include "fakebbs.h"
#include <time.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/select.h>
#include <strings.h>

void socket_input(int usernum, char *inp);
void write_to_socket(uint8_t code, uint16_t user, uint16_t data_length, uint8_t *data);
extern void init__muicyber();
extern void cyber_login();
extern void cyber_input();
extern void cyber_hup();
extern void cyber_clean();
char Stroke_Monitor(int chan, int c);


#define LOGIN_CODE 1
#define LOGOUT_CODE 2
#define UPDATE_USER_CODE 3
#define TTY_CODE 4
int user_socket;

/* our global variables */
//int user_sockets[NTERMS];

/* MajorBBS global variables */
struct user *usrptr, user[NTERMS];
char input[INPSIZ], *margv[INPSIZ];
int margc, inplen, status;
char *vdaptr, vdatmp[vdasiz];
int vfyadn;
struct sysvbl sv;
char prfbuf[PRFBUFSIZE];
char *prfptr = prfbuf;
int usrnum;
struct usracc usracc[NTERMS], *usaptr;
int bturno;
int genbb;
int othusn;
struct usracc *othuap;
struct tag_downloader_maybe ftgptr[1];
char tshmsg[81];
struct tag_downloader_maybe ftfscb[1];

int bbs_sendmsg(struct message *msg, char *to) {
  /* TODO */
  return 0;
}

short today() {
  time_t t = time(NULL);
  struct tm *tt = localtime(&t);
  return ((tt->tm_year - 80) * 512) |
         ((tt->tm_mon + 1) * 32) |
         tt->tm_mday;
}

void stzcpy(char *dst, char *src, int max) {
  strncpy(dst, src, max);
}

void stpans(char *ans) {
  /* printf("stpans = %s ??\n", ans); */
  /* ? */
}

void setmem(void *mem, int size, int dat) {
  memset(mem, dat, size);
}

void setmbk(FILE *f) {
  /* not needed */
}

int sameto(char *a, char *b) {
  if (sameas(a, b) == 1) {
    return 1;
  }

  int i;

  for (i = 0; a[i] != 0 && b[i] != 0; i++) {
    if (i > 0 &&
        (a[i] == 13 || a[i] == 10 || a[i] == 0 ||
         b[i] == 13 || b[i] == 10 || b[i] == 0)) {
      return 1;
    }

    if (tolower(a[i]) != tolower(b[i])) {
      return 0;
    }
  }

  return 1;
}

int sameas(char *a, char *b) {
  int i;

  for (i = 0; a[i] != 0 && b[i] != 0; i++) {
    if ((a[i] == 13 || a[i] == 10 || a[i] == 0) &&
        (b[i] == 13 || b[i] == 10 || b[i] == 0)) {
      return 1;
    }

    if (tolower(a[i]) != tolower(b[i])) {
      return 0;
    }
  }

  if (a[i] == 0 && b[i] == 0) {
    return 1;
  }

  return 0;
}

void rstrin() {
  /* gets(input); */
}

struct usracc *uacoff(int usrnum) {
  return &usracc[usrnum];
}

int ynopt(int opt) {
  return 0; /* ? */
}

void readsz(FILE *f, char *buff) {
  char x = '1';
  int i = 0;

  while (x != '\0') {
    fread(&x, 1, 1, f);
    buff[i++] = x;
  }
}

char *all_options;
char **options;

void init_options() {
  options = (char **) malloc(sizeof(char *) * 4096);
  all_options = (char *) malloc(100000);
  FILE *mcv = fopen("../MUICYBER.MCV", "rb");
  int i;
  int z = 0;
  char x;
  int last_start = 0;

  while ((x = fgetc(mcv)) != EOF) {
    if (x == 0xd) {
      x = 0xa;
    }

    all_options[i] = x;

    if (x == 0) {
      options[z++] = &all_options[last_start];
      last_start = i + 1;
    }

    i++;
  }

  fclose(mcv);
}

char *stgopt(int str) {
  return options[str + 1];
}

char *stgoptnl(int str) {
  static char buff[16384];
  return strcat(stgopt(str), "\n");
}

char *spr(char *fmt, int x) {
  char *copy = malloc(128);
  snprintf(copy, 128, fmt, x);
  return copy;
}

void shocst(char *dst, char *fmt, char *arg) { /* display text to audit trail     */
  fprintf(stderr, fmt, arg);
  putc(10, stderr);
  fflush(stderr);
}

void randomize() {
}

void rstmbk() {
  /* not needed */
}

void setbtv(int genbb) {
}

struct callback {
  int64_t run_at;
  void (*callback)(void);
  int alive;
};

struct callback callbacks[32];

void rtkick(int seconds, void (*x)(void)) {
  int i;
  time_t t = time(NULL);

  for (i = 0; i < 32; i++) {
    if (!callbacks[i].alive) {
      callbacks[i].alive = 1;
      callbacks[i].run_at = ((int64_t) t) + seconds;
      callbacks[i].callback = x;
      return;
    }
  }

  exit(2);
}

void rstrxf() {
}

void rstbtv() {
}

int register_module(struct module* mod) {
  return 0;
}

void parsin() {
}

void outprf(short user_num) {
  // fprintf(stderr, "Outputting prf to %d\n", user_num);
  // fflush(stderr);
  int i = 0;
  int l = strlen(prfbuf);

  for (i = 0; i < l; i++) {
    if (prfbuf[i] == '\r') {
      prfbuf[i] = '\n';
    }
  }

  write_to_socket(TTY_CODE, user_num, l, (uint8_t *) prfbuf);
  clrprf();
}

void clrprf() {
  prfbuf[0] = 0;
  prfptr = prfbuf;
}

FILE* opnmsg(char *fname) {
  char buff[128];
  snprintf(buff, 128, "../%s", fname);
  return fopen(buff, "r");
}

short numopt(short f, short a, short b) {
  return (short) lngopt(f, a, b);
}

unsigned short now() {
  time_t t = time(NULL);
  struct tm *tt = localtime(&t);
  return ((tt->tm_hour) * 2048) |
         (tt->tm_min * 32) |
         (tt->tm_sec / 2);
}

char *nctime(unsigned short t) {
  static char datebuff[16];
  sprintf(datebuff, "%02d:%02d:%02d", t >> 11, (t >> 5) & 63, (t & 31) * 2);
  return datebuff;
}

char *ncdate(short d) {
  static char datebuff[16];
  sprintf(datebuff, "%02d/%02d/%02d", (d >> 5) & 15, d & 31, ((d >> 9) + 80) % 100);
  return datebuff;
}

long lngopt(long score, int x, long max) {
  char *txt = stgopt(score);
  char *loc = strrchr(txt, ':');

  if (loc == NULL) {
    return atoi(txt);
  }

  long n = atoi(loc + 1);

  if (n < x) {
    return x;
  } else if (n > max) {
    return max;
  }

  return n;
}

static char l2as_buffer[32];
char *l2as(long x) {
  snprintf(l2as_buffer, 32, "%ld", x);
  return l2as_buffer;
}

void invbtv(void *rec, size_t size) {
  /* not needed -- only used for demo */
}

uint64_t global_timer_ns;

void init_global_timer() {
  struct timespec t;

  if (clock_gettime(CLOCK_MONOTONIC, &t) == -1) {
    perror("Error getting time");
    exit(-1);
  }

  global_timer_ns = (t.tv_sec * 1000000000LL) + t.tv_nsec;
}

long hrtval() {
  struct timespec t;

  if (clock_gettime(CLOCK_MONOTONIC, &t) == -1) {
    perror("Error getting time");
    exit(-1);
  }

  uint64_t ns = (t.tv_sec * 1000000000LL) + t.tv_nsec - global_timer_ns;
  return lround(((double) ns) / 15258.7890625);
}

int haskey(char *key) {
  return strncmp("swenson", usracc[usrnum].userid, 10) == 0 ? 1 : 0;
}

char *gmdnam(char *fname) { /* get a module's name from .MDF file   */
  return "Lords of Cyberspace";
}

char *getmsg(int msg) {
  /* ?? */
  return stgopt(msg);
}

void ftgsbm(char *s) {
}

int ftgnew() {
  return 0;
}

int fsdroom(int x, char *s, int i) {
  return 0;
}

int fsdrft() {
  return 0;
}

void fsdfxt(int a, char *s, int b) {
}

void fsdego(int a, void (*f)(int)) {
}

void fsdbkg(int x) {
}

void fsdapr(char *v, int size, char *tmp) {
}

void dfsthn() {
}

void dclvda(int size) {
}

const int days_per_month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

short datofc(short x) {
  /* inverse of cofdate */
  int year = 1980;

  while (1) {
    if ((year & 3) == 0) {
      if (x >= 366) {
        year++;
        x -= 366;
      } else {
        break;
      }
    } else if (x >= 365) {
      year++;
      x -= 365;
    } else {
      break;
    }
  }

  int month = 1;

  while (1) {
    if ((month == 2) && ((year & 3) == 0)) {
      if (x <= 28) {
        break;
      } else {
        x -= 29;
      }
    } else if (x <= days_per_month[month] - 1) {
      break;
    }

    x -= days_per_month[month];
    month++;
  }

  int day = x + 1;
  return ((year - 1980) << 12) | (month << 5) | day;
}

short cofdat(short x) {
  /* count of days since 1/1/80 */
  /* x = YYYY YYMM MMMD DDDD */
  int years = x >> 12;
  int month = (x >> 5) & 15;
  int dom = x & 31;
  int y = 81;
  int days = years * 365 + (years >> 2);

  if (years) {
    days++; /* 1980 is a leap year */
  }

  int m;

  for (m = 1; m < month; m++) {
    days += days_per_month[m];

    if ((m == 2) && (((years - 1) & 3) == 0)) {
      days++;
    }
  }

  days += dom - 1;
  return days;
}


void clsmsg(FILE *f) {
}

int obtbtv(void *recptr, void *key, int keynum,
           int obtopt) { /* "acquire" Btrieve operations utiltiy */
  /* not needed -- only used for demo */
  return 0;
}

int btuxnf(int chan, int xon, int xoff, ...) {
  /* Set xon/xoff characters */
  return 0;
}

int btuxmn(int chan, char *datstg) {
  /* Non-clearable ASCII transmit */
  return 0;
}

int btumil(int chan, int maxinl) {
  /* set maximum input length / word-wrap on/off */
  return 0;
}

int btuinj(int chan, int status) {
  /* inject status code into channel */
  return 0;
}

int btuchi(int chan, char (*rouadr)(int chan, int c)) {
  /* set character input recepter */
  return 0;
}

char *alczer(unsigned nbytes) {
  return (char *) calloc(nbytes, 1);
}

int addcrd(char *keyuid, char *tckstg, int real) { /* utility for posting credits to an account */
  return 0;
}

void cvtData(char *data, void *struc, int size, struct flddef *fields, char p, char s, char n) {
  memcpy(struc, data, size);
}

short RANDOM(short n) {
  short   val;

  if (n <= 0) {
    return (1);
  }

  val = (lrand48() % n) + 1;
  return (val);
}

long LRAND(long n) {
  short   val;

  if (n <= 0) {
    return (1);
  }

  val = (lrand48() % n) + 1;
  return (val);
}

void do_callbacks() {
  int i;
  time_t t;
  time(&t);

  for (i = 0; i < 32; i++) {
    if (callbacks[i].alive && ((int64_t) t) >= callbacks[i].run_at) {
      callbacks[i].alive = 0;
      callbacks[i].callback();
    }
  }
}

void prf(char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int cnt = vsnprintf(prfptr, PRFBUFSIZE, fmt, args);
  va_end(args);
  prfptr += cnt;
}

void prfmsg(int msg, ...) {
  char *fmt = stgopt(msg);
  int l = strlen(fmt);
  va_list args;
  va_start(args, msg);
  int cnt = vsnprintf(prfptr, PRFBUFSIZE, fmt, args);
  va_end(args);
  prfptr += cnt;
}

void write_to_socket(uint8_t code, uint16_t user, uint16_t data_length, uint8_t *data) {
  char header[5];
  header[0] = code;
  header[1] = user & 0xff;
  header[2] = user >> 8;
  header[3] = data_length & 0xff;
  header[4] = data_length >> 8;
  int written = write(user_socket, header, 5);

  if (written != 5) {
    perror("Error writing to socket");
    exit(-1);
  }

  int left = data_length;

  while (1) {
    written = write(user_socket, &data[data_length - left], left);

    if (written < 0) {
      perror("error writing to socket");
      clrprf();
      return;
    }

    if (written >= left) {
      break;
    }

    left -= written;
  }
}

typedef struct socket_message_s {
  uint8_t code;
  uint16_t user;
  uint16_t data_length;
  uint8_t *data;
} socket_message_t;

socket_message_t *read_socket_message() {
  static uint8_t *data;
  static uint8_t tmp[5];

  if (data == NULL) {
    data = malloc(65536);
  }

  static socket_message_t msg;
  int n = read(user_socket, tmp, 5);

  if (n == 0) {
    return NULL;
  } else if (n != 5) {
    perror("Error reading from socket");
    exit(-1);
  }

  msg.code = tmp[0];
  msg.user = (tmp[2] << 8) | tmp[1];
  msg.data_length = (tmp[4] << 8) | tmp[3];
  msg.data = data;
  uint16_t red = 0;

  while (red != msg.data_length) {
    int m = read(user_socket, &data[red], msg.data_length - red);

    if (m == 0) {
      perror("Error reading from socket");
      exit(-1);
    }

    red += m;
  }

  return &msg;
}

void switch_active_user(int usernum) {
  usrnum = usernum;
  usrptr = &user[usernum];
  usaptr = &usracc[usernum];
  // fprintf(stderr, "Switching to port %d user %s\n", usernum, usracc[usernum].userid);
  // fflush(stderr);
}

void handle_user_login(socket_message_t *msg) {
  int i = msg->user;
  usrptr = &user[i];
  usaptr = &usracc[i];
  usracc[i].ansifl |= ANSON;
  strncpy(usracc[i].userid, (const char *) msg->data, msg->data_length);

  if ((usracc[i].userid[msg->data_length - 1]) != 0) {
    usracc[i].userid[msg->data_length] = 0;
  }

  fprintf(stderr, "Logging in user port %d username %s\n", msg->user, usracc[i].userid);
  fflush(stderr);
  switch_active_user(i);
  cyber_login();
}

int Setup_Signoff(void);
void Sign_Me_Off(void);

char *user_buffers[256];
int user_buffer_idx[256];

void handle_user_logout(socket_message_t *msg) {
  // TODO: write out user file
  switch_active_user(msg->user);
  fprintf(stderr, "Logging out user port %d username %s\n", msg->user, usracc[msg->user].userid);
  fflush(stderr);
  
  int u = Setup_Signoff();
  if (u < 0) {
    fprintf(stderr, "Could not sign them off. Oh well.\n");
    fflush(stderr);
    bzero(&user[msg->user], sizeof(struct user));
    bzero(&usracc[msg->user], sizeof(struct usracc));
    usrptr = NULL;
    usaptr = NULL;
    usrnum = -1;
    user_buffer_idx[msg->user] = 0;
    return;
  }
  Sign_Me_Off();
  fprintf(stderr, "User port %d username %s signed off\n", msg->user, usracc[msg->user].userid);
  fflush(stderr);
  //cyber_hup();
  // fprintf(stderr, "User port %d username %s hung up\n", msg->user, usracc[msg->user].userid);
  // fflush(stderr);
  // bzero(&user[msg->user], sizeof(struct user));
  // bzero(&usracc[msg->user], sizeof(struct usracc));
  usrptr = NULL;
  usaptr = NULL;
  usrnum = -1;
  user_buffer_idx[msg->user] = 0;
  fprintf(stderr, "User port %d successfully logged out\n", msg->user);
  fflush(stderr);
}


void handle_user_tty(socket_message_t *msg) {
  if (user_buffers[msg->user] == NULL) {
    user_buffers[msg->user] = calloc(1024, 1);
    user_buffer_idx[msg->user] = 0;
  }

  switch_active_user(msg->user);
  memcpy(user_buffers[msg->user], msg->data, msg->data_length);
  user_buffer_idx[msg->user] += msg->data_length;

  // if (n > 0) {
  //   for (int i = bufi - n; i < bufi; i++) {
  //     sock_buff[i] = Stroke_Monitor(0, sock_buff[i]);
  //   }
  // }
  while (1) {
    int nl = 0;
    int j;

    for (j = 0; j < user_buffer_idx[msg->user]; j++) {
      // printf("sock_buff[%d] = %d\n", j, (int) sock_buff[j]);
      if (user_buffers[msg->user][j] == 10 || user_buffers[msg->user][j] == 13) {
        user_buffers[msg->user][j] = 0;
        nl = 1;
        break;
      }
    }

    if (nl) {
      socket_input(msg->user, user_buffers[msg->user]);
      j += 1;

      if (j < user_buffer_idx[msg->user]) {
        memmove(user_buffers[msg->user], &user_buffers[msg->user][j], user_buffer_idx[msg->user] - j);
      }

      user_buffer_idx[msg->user] -= j;
    } else {
      break;
    }
  }
}

void check_for_cleanup() {
  static time_t next_check = -1;
  time_t t = time(NULL);

  if (t >= next_check) {
    next_check = t + 60;
    int do_cleanup = 0;
    FILE *f = fopen("lastcleanup.txt", "r");
    if (f == NULL) {
      do_cleanup = 1;
    } else {
      short last_ran;
      fscanf(f, "%hd", &last_ran);
      fclose(f);
      if (today() != last_ran) {
        do_cleanup = 1;
      }
    }
    
    if (do_cleanup) {
      f = fopen("lastcleanup.txt", "w");
      fprintf(f, "%hd\n", today());
      fclose(f);
      cyber_clean();
    }
  }
}

/* void catastro(char *f) {} */
int main(int argc, char **argv) {
  bzero(user_buffers, sizeof(user_buffers));
  init_global_timer();
  init_options();
  bzero(callbacks, sizeof(struct callback) * 32);
  init__muicyber();
  /* setup user and usrptr */
  fprintf(stderr, "LoC: Waiting on connection\n");
  fflush(stderr);
  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, "muisock", sizeof(addr.sun_path) - 1);
  unlink(addr.sun_path);

  if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("socket bind error");
    exit(-1);
  }

  if (listen(sock, 5) == -1) {
    perror("listen error");
    exit(-1);
  }

  int cl = accept(sock, NULL, NULL);

  if (cl == -1) {
    perror("accept error");
    exit(-1);
  }

  user_socket = cl;
  char inp[1024];

  for (int i = 0; i < INPSIZ / 2; i++) {
    margv[i] = malloc(INPSIZ);
  }

  sleep(2);
  do_callbacks();
  /* 10 ms */
  struct timespec pselect_timeout;
  pselect_timeout.tv_sec = 0;
  pselect_timeout.tv_nsec = 10000000;

  while (1) {
    check_for_cleanup();
    do_callbacks();
    fd_set fd, error_fd;
    bzero(&fd, sizeof(fd));
    bzero(&error_fd, sizeof(fd));
    FD_SET(cl, &fd);
    FD_SET(cl, &error_fd);
    // TODO: catch these errors
    pselect(cl + 1, &fd, NULL, NULL, &pselect_timeout, NULL);

    if (FD_ISSET(cl, &fd)) {
      socket_message_t *msg = read_socket_message();

      if (msg == NULL) {
        continue;
      }

      switch (msg->code) {
      case LOGIN_CODE:
        handle_user_login(msg);
        break;

      case LOGOUT_CODE:
        handle_user_logout(msg);
        break;

      case TTY_CODE:
        handle_user_tty(msg);
        break;

      default:
        printf("Unknown code %d\n", msg->code);
      }
    }
  }

  return 0;
}

void socket_input(int usernum, char *inp) {
  char *tok = inp;
  margc = 0;
  tok = strtok(inp, " ");

  while (tok != NULL) {
    strcpy(margv[margc++], tok);
    tok = strtok(NULL, " ");
  }

  switch_active_user(usernum);
  cyber_input();
}