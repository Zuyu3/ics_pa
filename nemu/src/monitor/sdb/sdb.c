/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "watchpoint.h"
#include <memory/vaddr.h>

static int is_batch_mode = false;

void init_regex();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
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

static int cmd_help(char *args);

static int cmd_si(char *args) {
  // TODO: Maybe some bugs here. Need further tests.
  int n = 0;
  if (args) {
    sscanf(args, "%d", &n);
  }
    /*for (int i = 0; args[i] != '\0'; i++)
    {
      if (args[i] < '0' || args[i] > '9')
      {
        printf("Error. Only unsigned integers are allowed. Please input a valid argument.\n");
        
        //Not Sure: argument not integer. Abandon exec here.
        return 0;
      }
      n = n * 10 + args[i] - '0';
    }*/
  n = n ? n : 1;
  cpu_exec(n);
  return 0;
}

static int cmd_info(char *args){ 
  char subcmd;
  sscanf(args, "%c", &subcmd);
  if(subcmd == 'r'){
    isa_reg_display();
  }
  if(subcmd == 'w'){
    print_watchpoints();
  }
  if(subcmd == 'c'){
    isa_csr_display();
  }
  return 0; 
}

static int cmd_x(char *args){
  int n = 0;
  vaddr_t addr;
  if (args) {
    sscanf(args, "%d %x", &n, &addr);
  }
  for(int i = 0; i < n; i++)
    printf("0x%08x:   0x%08x\n", addr + 4 * i, vaddr_read(addr + 4 * i, 4));
  return 0;
}

static int cmd_p(char *args){
  bool is_success;
  word_t res;
  if(!args) {
    printf("No expr\n");
    return 0;
  }
  res = expr(args, &is_success);
  if(is_success)
    printf("%u\n", res);
  return 0;
}

static int cmd_w(char *args){
  new_wp(args);
  return 0;
  //TODO:check it later
}

static int cmd_d(char *args){
  int t;
  sscanf(args, "%d", &t);
  free_wp(t);
  return 0;
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Run the program one step", cmd_si},
  {"info", "Print state of the program", cmd_info},
  {"x", "Scanf the memory and output", cmd_x},
  {"p", "Get the answer of expression and print it out.", cmd_p},
  {"w", "Add a watchpoint.", cmd_w},
  {"d", "delete a watchpoint", cmd_d},

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

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

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
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

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) {
          nemu_state.state = NEMU_QUIT;
          return; 
        }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
  
}
