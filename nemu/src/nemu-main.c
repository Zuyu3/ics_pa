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

#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();
word_t expr(char *e, bool *success);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  /*Test Module
  FILE *fp = fopen("tools/gen-expr/input", "r");
  char toEvalExpr[50000];
  unsigned int stdAnswer, myAnswer;
  bool s = true;

  for(int i = 0; i < 10000; i++){
    if(!fscanf(fp, "%d", &stdAnswer)){
      break;
    }
    if(!fgets(toEvalExpr, 50000, fp)){
      break;
    }
    Log("%d\n%s", stdAnswer, toEvalExpr);
    myAnswer = expr(toEvalExpr, &s);
    if(myAnswer != stdAnswer){
      printf("WA!\ninput expression is: %s\n Standard is %d, but my answer is %d\n",toEvalExpr, stdAnswer, myAnswer);
      assert(0);
    }
    else{
      printf("Pass expression %d\n\n", i + 1);
    }
  }
  */


 
  /* Start engine. */
  engine_start();
  nemu_state.state = NEMU_QUIT;
  
  return is_exit_status_bad();
}
