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

void gen_space(int *index){
  int res = rand() % 4;
  for(int i = 0; i < res; i++){
    buf[*index] = ' ';
    (*index)++;
  }
}

void gen_num(int *index)
{
  int l = *index;
  int res = rand();
  char temp;
  while (res)
  {
    buf[l] = res % 10 + '0';
    l++;
    res /= 10;
  }
  for (int i = *index, j = l - 1; i < j; i++, j--)
  {
    temp = buf[i];
    buf[i] = buf[j];
    buf[j] = temp;
  }
  *index = l;
}

void gen_rand_op(int *index){
  int res = rand() % 4;
  switch (res)
  {
  case 0:
    buf[*index] = '+';
    (*index)++;
    break;
  case 1:
    buf[*index] = '-';
    (*index)++;
    break;
  case 2:
    buf[*index] = '*';
    (*index)++;
    break;
  case 3:
    buf[*index] = '/';
    (*index)++;
    break;
  default:
    break;
  }
}

static void gen_rand_expr(int *index, int max_loop_number)
{
  //TODO:filter expressions including " / 0"
  int loop_number = rand() % max_loop_number;
  for (int i = 0; i < loop_number; i++)
    switch (rand() % 3)
    {
    case 0:
      gen_num(index);
      gen_space(index);
      break;
    case 1:
      buf[*index] = '(';
      (*index)++;
      gen_space(index);
      max_loop_number = max_loop_number * 10 / 12;
      gen_rand_expr(index, max_loop_number);
      buf[*index] = ')';
      (*index)++;
      gen_space(index);
      break;
    default:
      max_loop_number = max_loop_number * 10 / 12;
      gen_rand_expr(index, max_loop_number);
      gen_space(index);

      gen_rand_op(index);
      gen_space(index);

      max_loop_number = max_loop_number * 10 / 12;
      gen_rand_expr(index, max_loop_number);
      gen_space(index);
      break;
    }
}

int main(int argc, char *argv[])
{
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1)
  {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  int index = 0;
  for (i = 0; i < loop; i++)
  {
    gen_rand_expr(&index, 6000);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0)
      continue;

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
