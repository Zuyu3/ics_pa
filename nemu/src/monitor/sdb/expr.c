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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <memory/vaddr.h>

enum
{
  TK_NOTYPE = 256,
  TK_EQ,
  TK_NEQ,
  TK_LAND,
  TK_DECNUM,
  TK_DECNUMU,
  TK_UNARY_MINUS,
  TK_HEXNUM,
  TK_HEXNUMU,
  TK_REGISTER,
  TK_REGISTER_NOSUCH, //no such register
  TK_UNARY_MULT,
  /* TODO: Add more token types */

};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE}, // spaces
    {"\\+", '+'},      // plus
    {"==", TK_EQ},     // equal
    {"!=", TK_NEQ},
    {"&&", TK_LAND},
    {"\\-", '-'},
    {"\\*", '*'},
    {"\\/", '/'},
    {"\\(", '('},
    {"\\)", ')'},
    {"0x[0123456789abcdefABCDEF]+u", TK_HEXNUMU},
    {"0x[0123456789abcdefABCDEF]+", TK_HEXNUM},
    {"[0-9]+u", TK_DECNUMU},
    {"[0-9]+", TK_DECNUM},
    //$0, ra, sp, gp, tp, t0, t1, t2, s0, s1, a0, a1, a2, a3, a4, a5, a6, a7, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, t3, t4, t5, t6
    {"\\$(0|ra|[sgt]p|t[0-6]|a[0-7]|s[0-9]|s10|s11)", TK_REGISTER},
    //No such register
    {"\\$.{1,2}", TK_REGISTER_NOSUCH},
    
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[60000] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

bool is_unary_operator(int x) {
  int certain_type[] = {')', TK_DECNUM, TK_HEXNUM, TK_REGISTER};
  if(x == 0)
    return true;
  for(int i = 0; i < 4; i++){
    if(tokens[x - 1].type == certain_type[i])
      return false;
  }
  return true;
}

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0' && e[position] != '\n')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        if (substr_len > 32)
        {
          printf("Substr is too long.\n");
          assert(0);
        }

        switch (rules[i].token_type)
        {
        case TK_NOTYPE:
          break;
        case TK_HEXNUMU:
          //unsigned int
          tokens[nr_token].type = TK_HEXNUM;
          for (int j = 0; j < substr_len; j++)
            tokens[nr_token].str[j] = substr_start[j];
          tokens[nr_token].str[substr_len - 1] = 0;
          nr_token++;
          break;
        case TK_DECNUMU:
          //unsigned hex int
          tokens[nr_token].type = TK_DECNUM;
          for (int j = 0; j < substr_len; j++)
            tokens[nr_token].str[j] = substr_start[j];
          tokens[nr_token].str[substr_len - 1] = 0;
          nr_token++;
          break; 
        case '-':
          if(is_unary_operator(nr_token)) {
            tokens[nr_token].type = TK_UNARY_MINUS;
            tokens[nr_token].str[0] = '-';
            nr_token++;
            break;
          }
        case '*':
          if(is_unary_operator(nr_token)) {
            tokens[nr_token].type = TK_UNARY_MULT;
            tokens[nr_token].str[0] = '*';
            nr_token++;
            break;
          }
        case TK_REGISTER_NOSUCH:
        printf("register name wrong at position %d\n%s\n%*.s^\n", position, e, position, "");
        return false;
        default:
          tokens[nr_token].type = rules[i].token_type;
          for (int j = 0; j < substr_len; j++)
            tokens[nr_token].str[j] = substr_start[j];
          //printf("%d    %s\n", tokens[nr_token].type, tokens[nr_token].str);
          nr_token++;
        }
        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q) {
  /*check if the expression is surrounded by a matched pair of parentheses.
   *return true if it is.
   */
  // TODO: check it later.
  if (tokens[p].type != '(' || tokens[q].type != ')')
    return false;
  int parenthesesCounter = 0;
  for(int i = p + 1; i < q; i++){
    switch (tokens[i].type)
    {
    case '(':
      parenthesesCounter++;
      break;
    case ')':
    if(parenthesesCounter > 0)
      parenthesesCounter--;
    else{
      //printf("check parentheses at %d and %d, result is %d\n", p, q, parenthesesCounter);
      return false;
    }
    default:
      break;
    }
  }
  //printf("check parentheses at %d and %d, result is %d\n", p, q, parenthesesCounter);
  return parenthesesCounter == 0;
}

const char *decode_register(int x){
  printf("decode: %s\n", tokens[x].str);
  char ori[][6] = {
  {"$0"}, {"$ra"}, {"$sp"}, {"$gp"}, {"$tp"}, {"$t0"}, {"$t1"}, {"$t2"},
  {"$s0"}, {"$s1"}, {"$a0"}, {"$a1"}, {"$a2"}, {"$a3"}, {"$a4"}, {"$a5"},
  {"$a6"}, {"$a7"}, {"$s2"}, {"$s3"}, {"$s4"}, {"$s5"}, {"$s6"}, {"$s7"},
  {"$s8"}, {"$s9"}, {"$s10"}, {"$s11"}, {"$t3"}, {"$t4"}, {"$t5"}, {"$t6"}
  };
  const char *result[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
  };
  
  for(int i = 0; i < 32; i++){
    if(tokens[x].str == ori[i]){
      printf("match at %d   %s", i, result[i]);
      return result[i];
    }
  }
  return 0;
}

word_t eval(int p, int q, bool *success)
{
  word_t res;
  if (p > q)
  {
    // Bad expression.
    // TODO:maybe need further check here.
    *success = false;
    printf("Error at %d %d\n", p, q);
    return 0;
  }
  else if (p == q) {
    // Single token.
    // For now this token should be a number.
    // Return the value of the number.
    if(tokens[p].type == TK_DECNUM)
      *success = sscanf(tokens[p].str, "%d", &res);
    else if (tokens[p].type == TK_DECNUM)
      *success = sscanf(tokens[p].str, "%x", &res);
    else if (tokens[p].type == TK_REGISTER) {
      decode_register(p);
    }
    return *success? res: 0;
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, success);
  }
  else if ((tokens[p].type != '(' && (tokens[p].type < TK_EQ || tokens[p].type > TK_UNARY_MULT)) || (tokens[q].type != ')' && (tokens[q].type < TK_EQ || tokens[q].type > TK_UNARY_MULT))) {
    printf("Ban before operators\n");
    *success = false;
    return 0;
  }
  else {
    // TODO: implement it here.
    
    // scan for "&&"
    for (int i = q, parenthesesCounter = 0; i > p; i--)
    {
      switch (tokens[i].type)
      {
      case TK_LAND:
        if (!parenthesesCounter)
        {
          res = eval(p, i - 1, success) && eval(i + 1, q, success);
          return *success ? res : 0;
        }
        break;
      case ')':
        parenthesesCounter++;
        break;
      case '(':
        if (parenthesesCounter > 0)
          parenthesesCounter--;
        else {
          *success = false;
          printf("Error. parentheses not match at position %d. %d '(' remains\n", i, parenthesesCounter);
          return 0;
        }
        break;
      default:
        break;
      }
    }
    
    // scan for '==' or '!='
    for (int i = q, parenthesesCounter = 0; i > p; i--)
    {
      switch (tokens[i].type)
      {
      case TK_EQ:
        if (!parenthesesCounter)
        {
          res = eval(p, i - 1, success) == eval(i + 1, q, success);
          return *success ? res : 0;
        }
        break;
      case TK_NEQ:
        if (!parenthesesCounter)
        {
          res = eval(p, i - 1, success) != eval(i + 1, q, success);
          return *success ? res : 0;
        }
        break;
      case ')':
        parenthesesCounter++;
        break;
      case '(':
        if (parenthesesCounter > 0)
          parenthesesCounter--;
        else {
          *success = false;
          printf("Error. parentheses not match at position %d. %d '(' remains\n", i, parenthesesCounter);
          return 0;
        }
        break;
      default:
        break;
      }
    }
   
    // scan for '+' or '-'
    for (int i = q, parenthesesCounter = 0; i > p; i--)
    {
      switch (tokens[i].type)
      {
      case '+':
        if (!parenthesesCounter)
        {
          res = eval(p, i - 1, success) + eval(i + 1, q, success);
          return *success ? res : 0;
        }
        break;
      case '-':
        if (!parenthesesCounter)
        {
          res = eval(p, i - 1, success) - eval(i + 1, q, success);
          return *success ? res : 0;
        }
        break;
      case ')':
        parenthesesCounter++;
        break;
      case '(':
        if (parenthesesCounter > 0)
          parenthesesCounter--;
        else {
          *success = false;
          printf("Error. parentheses not match at position %d. %d '(' remains\n", i, parenthesesCounter);
          return 0;
        }
        break;
      default:
        break;
      }
    }

    // second scan for '*' or '/'
    for (int i = q, parenthesesCounter = 0; i > p; i--)
    {
      switch (tokens[i].type)
      {
      case '*':
        if (!parenthesesCounter)
        {
          res = eval(p, i - 1, success) * eval(i + 1, q, success);
          return *success ? res : 0;
        }
        break;
      case '/':
        if (!parenthesesCounter)
        {
          res = eval(i + 1, q, success);
          /*if(res == 0){
            printf("Divide 0.\n");
            *success = false;
          }
          else*/
            res = eval(p, i - 1, success) / res;
          return *success ? res : 0;
        }
        break;
      case ')':
        parenthesesCounter++;
        break;
      case '(':
        if (parenthesesCounter > 0)
          parenthesesCounter--;
        else
        {
          *success = false;
          printf("Error. parentheses not match at position %d. %d '(' remains\n", i, parenthesesCounter);
          return 0;
        }
        break;
      default:
        break;
      }
    }

    // third scan for unary '-' or '*'
    // associate with right side
    for (int i = p, parenthesesCounter = 0; i < q; i++)
    {
      switch (tokens[i].type)
      {
      case TK_UNARY_MINUS:
        if (!parenthesesCounter)
        {
          res = 0u - eval(i + 1, q, success);
          return *success ? res : 0;
        }
        break;
      case TK_UNARY_MULT:
        if (!parenthesesCounter)
        {
          res = eval(i + 1, q, success);
          if(*success == false)
            return 0;
          res = vaddr_read((vaddr_t)res , 4);
          return *success ? res : 0;
        }
        break;
      case '(':
        parenthesesCounter++;
        break;
      case ')':
        if (parenthesesCounter > 0)
          parenthesesCounter--;
        else
        {
          *success = false;
          printf("Error. parentheses not match at position %d. %d '(' remains\n", i, parenthesesCounter);
          return 0;
        }
        break;
      default:
        break;
      }
    }
    *success = false;
    printf("No arithmetic operators\n");
    return 0;
  }
}

word_t expr(char *e, bool *success)
{
  memset(tokens, 0, sizeof(tokens));
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }
  /*for (int i = 0; i < nr_token; i++)
  {
    if (tokens[i].type == TK_DECNUM)
      printf("__");

    else
      printf("%s", tokens[i].str);
  }
  printf("\n");
  for (int i = 0; i < nr_token; i++)
  {
    if (tokens[i].type == TK_DECNUM)
      printf("%d   %s\n", i, tokens[i].str);
  }*/
  *success = true;
  // TODO: maybe need to check success here.
  return eval(0, nr_token - 1, success);
}
