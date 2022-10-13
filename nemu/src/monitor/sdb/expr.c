#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  /* TODO: Add more token types */
  _BOP_START, TK_EQ, TK_NE, TK_AND, // binary operand
  TK_PLUS, TK_MINUS,   
  TK_MUL, TK_DIVIDE, _BOP_END,
  TK_HEX, TK_REG, TK_DEREF,
  TK_LP, TK_RP, TK_NUM,
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_PLUS},         // plus
  {"-", TK_MINUS},         // minus
  {"\\*0x[0-9a-fA-F]+", TK_DEREF},         // dereference 
  {"\\*", TK_MUL},         // multiply
  {"/", TK_DIVIDE},         // divide
  {"==", TK_EQ},        // equal
  {"!=", TK_NE},        // not equal
  {"\\(", TK_LP},         // left parenthesis
  {"\\)", TK_RP},         // right parenthesis
  {"0x[0-9a-fA-F]+", TK_HEX},         // hexadecimal
  {"([1-9][0-9]*)|(0[^0-9]*)", TK_NUM},   // number
  {"\\$[a-z0-9]+", TK_REG},         // register
  {"&&", TK_AND},         // and 
};

word_t isa_reg_str2val(const char *s, bool *success);

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char *str;
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        // if token includes "*0x..."
        if (nr_token != 0 && (tokens[nr_token - 1].type == TK_NUM || tokens[nr_token - 1].type == TK_RP))
          substr_len = 1;
        // if the number is 0
        if (rules[i].token_type == TK_NUM && substr_start[0] == '0')
          substr_len = 1;
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        if (rules[i].token_type != TK_NOTYPE) {
          if (rules[i].token_type == TK_DEREF && substr_len == 1)
            tokens[nr_token].type = TK_MUL;
          else
            tokens[nr_token].type = rules[i].token_type;
          int size = substr_len + 1;
          tokens[nr_token].str = (char *) malloc(size);
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          tokens[nr_token].str[substr_len] = '\0';
          nr_token++;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

void free_tokens_str() {
  for (int i = 0; i < nr_token; i++) {
    free(tokens[i].str);
    tokens[i].str = NULL;
  }
  nr_token = 0;
}

bool check_parentheses(int start, int end) {
  if (tokens[start].type != TK_LP || tokens[end].type != TK_RP) return false;
  int num_left_par = 0;
  for (int i = start + 1; i < end; i++) {
    if (tokens[i].type == TK_LP) {
      num_left_par++;
    } else if (tokens[i].type == TK_RP) {
      if (num_left_par == 0) return false;
      else num_left_par--;
    }
  }
  if (num_left_par == 0) return true;
  return false;
}

word_t vaddr_read(vaddr_t addr, int len);

word_t expr_substr(int start, int end/*, bool *success*/) {
  word_t val = 0;
  if (start > end) {
    // *success = false;
  } else if (start == end) {
    switch (tokens[start].type) {
      case TK_NUM:
        val = atoi(tokens[start].str);
        break;
      case TK_HEX:
        sscanf(tokens[start].str, "%x", &val);
        break;
      case TK_REG:
          val = isa_reg_str2val(tokens[start].str + 1, NULL);
          break;
      case TK_DEREF:
        uint32_t addr;
        sscanf(tokens[start].str + 1, "%x", &addr);
        val = vaddr_read(addr, 4);
        break;
    }
  } else if (check_parentheses(start, end) == true) {
    val = expr_substr(start + 1, end - 1);
  } else {
    bool in_parentheses = false;
    int op = start;
    int master_op = start;
    while (op <= end) {
      int op_type = tokens[op].type;
      int master_op_type = tokens[master_op].type;
      if (op_type == TK_LP) in_parentheses = true;
      else if (op_type == TK_RP) in_parentheses = false;
      else if (!in_parentheses){
        if (op_type > _BOP_START && op_type < _BOP_END && master_op_type > op_type)
          master_op = op;
      }
      if (op_type == TK_LP) in_parentheses = true;
      op += 1;
    }
    word_t val_left = expr_substr(start, master_op - 1);
    word_t val_right = expr_substr(master_op + 1, end);
    switch (tokens[master_op].type) {
      case TK_PLUS:
        val = val_left + val_right; break;
      case TK_MINUS:
        val = val_left - val_right; break;
      case TK_MUL:
        val = val_left * val_right; break;
      case TK_DIVIDE:
        if (val_right != 0)
          val = val_left / val_right; 
        else
          printf("Zero Division!\n");
        break;
      case TK_EQ:
       val = val_left == val_right; break;
      case TK_NE:
       val = val_left != val_right; break;
      case TK_AND:
       val = val_left && val_right; break;
      default: printf("Wrong operator type!\n");
    }
  }
  return val;
}

void print_expr() {
  for (int i = 0; i < nr_token; i++) {
    printf("%s ", tokens[i].str);
  }
  printf("\n");
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  word_t val = expr_substr(0, nr_token - 1/*, success*/);
  free_tokens_str();
  return val;
}

void test_token(char *e) {
  word_t val = expr(e, NULL);
  printf("%d\n", val);
}