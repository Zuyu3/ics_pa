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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
    "#include <stdio.h>\n"
    "int main() { "
    "  unsigned result = %s; "
    "  printf(\"%%u\", result); "
    "  return 0; "
    "}";
int buf_index;

void gen_space(){
  int res = rand() % 4;
  for(int i = 0; i < res; i++){
    buf[buf_index] = ' ';
    buf_index++;
  }
}

void gen_num()
{
  int l = buf_index;
  int res = rand();
  char temp;
  while (res)
  {
    buf[l] = res % 10 + '0';
    l++;
    res /= 10;
  }
  for (int i = buf_index, j = l - 1; i < j; i++, j--)
  {
    temp = buf[i];
    buf[i] = buf[j];
    buf[j] = temp;
  }
  buf[l++] = 'u';
  buf_index = l;
  gen_space();
}

void gen_rand_op(){
  int res = rand() % 3;
  switch (res)
  {
  case 0:
    buf[buf_index] = '+';
    buf_index++;
    break;
  case 1:
    buf[buf_index] = '-';
    buf_index++;
    break;
  case 2:
    buf[buf_index] = '*';
    buf_index++;
    break;
  default:
    break;
  }
  gen_space();
}

void gen_rand_expr(int gen_level) {
  if(gen_level == 0){
    gen_num();
    gen_space();
    return;
  }
  int gen_level_moder = gen_level / 5 * 3 + 1;
  int gen_level_adder = gen_level / 5;
  switch (rand() % 10) {
  case 0:
  case 1:
    gen_num(buf_index);
    break;
  case 2:
  case 3:
  case 4:
  case 5:
    buf[buf_index++] = '(';
    gen_space();
    gen_rand_expr(rand() % gen_level_moder + gen_level_adder);
    buf[buf_index++] = ')';
    gen_space();
    break;
  default:
    gen_rand_expr(rand() % gen_level_moder + gen_level_adder);
    gen_rand_op();
    gen_rand_expr(rand() % gen_level_moder + gen_level_adder);
    break;
  }
}

int main(int argc, char *argv[])
{
  int seed = time(0);
  srand(seed);
  int loop = 1000;
  if (argc > 1)
  {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i++)
  {
    buf_index = 0;
    gen_rand_expr(3000);
    buf[buf_index++] = '\0';

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);
    
    int ret = system("gcc -w /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) {
      printf("%d\n", i);
      continue;
    }

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    int temp = fscanf(fp, "%d", &result);
    assert(temp);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
