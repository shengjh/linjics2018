#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
WP* new_wp();
void free_wp(WP *wp);
void print_wp();
WP* loc_wp(int a);
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
  uint64_t num = 1;
  if (args!=NULL)
       num = strtoull(args,NULL,10 );
   cpu_exec(num); 
   return 0;
}

static int cmd_info(char *args) {
  int i;
  if ((args[0] == 'r')||(args[0] == 'R'))
     {
        for(i=0;i<8;i++)
           {
             printf("%d, %#010x\n",i,cpu.gpr[i]._32);
           }
        return 0;
      }
  if((args[0] == 'w')||(args[0] == 'W'))
     {
        print_wp();       
        return 0;
     }
     return 0;
}

static int cmd_w(char *args) {
  bool  success2 = true;
  bool* success  = &success2;
  WP* p;
  p = new_wp();
  strcpy(p->s,args);
  p->result = expr(p->s,success);
  
  return 0;
}

static int cmd_d(char *args) {
  int a;
  WP* b;
  a = atoi(&args[0]);
  b = loc_wp(a);
  free_wp(b);
  return 0;
}
static int cmd_x(char *args) {
  uint32_t a,b,i;
  a = strtoul(&args[0],NULL,10);
  b = strtoul(&args[1],NULL,10);
  for (i=0;i<a;i++)
    {  
        printf("%#08x\n",paddr_read(b,4));
        b=b+4;
    }
  return 0;
}
       
static int cmd_p(char *args){
  bool  success1 = true;
  bool* success  = &success1;
  printf("%d\n",expr(args,success));
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step one instruction exactly",cmd_si },
  { "info", "List of integer registers and their contents",cmd_info},
  { "x", "Examine memory: x/FMT ADDRESS",cmd_x},
  { "p", "expression evaluation",cmd_p},
  { "w", "set watchpoints",cmd_w},
  { "d","free uesd watchpoints",cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

char *str;
  for (; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

//p (1+(3*2) +(($eax-$eax) +(*$eip-1**$eip)+(0x5--5+  *0X100005 - *0x100005) )*4)
