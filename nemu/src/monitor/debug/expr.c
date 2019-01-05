#include "nemu.h"
#include "stdlib.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_plus,TK_sub,num,TK_mul,TK_div,TK_lbr,TK_rbr,hex,reg,TK_NEQ,TK_and,TK_LEQ,DEREF,TK_minus

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces

  {"\\+", TK_plus},         // plus
  {"\\-", TK_sub},         // sub
  {"\\*", TK_mul},         // multiply
  {"\\/", TK_div},         // divide
  {"\\(", TK_lbr},         // left bracket
  {"\\)", TK_rbr},         // right bracket
  {"==", TK_EQ},         // equal
  {"!=", TK_NEQ},        // not equal   
  {"&&", TK_and},        
  {"0[xX][0-9a-fA-F]+", hex},
  {"\\$[a-zA-z]{3}", reg},
  {"[0-9]+", num},       // decimal integer
  {"<=", TK_LEQ}

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

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
  char str[32];
} Token;

Token tokens[32];
int nr_token;

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

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE : break;
          case TK_plus : { tokens[nr_token].type = TK_plus;nr_token++;}break;
          case TK_sub : { tokens[nr_token].type = TK_sub;nr_token++;}break;
          case TK_mul : { tokens[nr_token].type = TK_mul;nr_token++;}break;
          case TK_div : { tokens[nr_token].type = TK_div;nr_token++;}break;
          case TK_lbr : { tokens[nr_token].type = TK_lbr;nr_token++;}break;
          case TK_rbr : { tokens[nr_token].type = TK_rbr;nr_token++;}break;
          case num : { tokens[nr_token].type = num; strncpy(tokens[nr_token].str,substr_start,substr_len);nr_token++;}break;
          case TK_EQ : { tokens[nr_token].type = TK_EQ;nr_token++;}break;
          case TK_NEQ : { tokens[nr_token].type = TK_NEQ;nr_token++;}break;
          case TK_and : { tokens[nr_token].type = TK_and;nr_token++;}break;
          case TK_LEQ : { tokens[nr_token].type = TK_LEQ;nr_token++;}break;
          case hex : { tokens[nr_token].type = hex;strncpy(tokens[nr_token].str,substr_start,substr_len);nr_token++;}break;
          case reg : { tokens[nr_token].type = reg;strncpy(tokens[nr_token].str,substr_start,substr_len);nr_token++;}break;


          default:  break;
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

int lbr = 0;
 bool check_parentheses(int p,int q){

   int i;
   
   if((tokens[p].type==TK_lbr)&&(tokens[q].type==TK_rbr))
       {
	  for(i=p;i<=q;i++)
	    {
		if(tokens[i].type==TK_lbr)
                    lbr++;
  		if(tokens[i].type==TK_rbr)
                    lbr--;
		if(lbr < 0) 
		    { panic("bad expressioon");assert(0);}
        	if((lbr==0)&&(i!=q))
          	    return false;
		    
	    }
         if(lbr==0)
    	     return true;
         else
     	     return false;
         
       }
   else 
	return false;
}


uint32_t eval(int p, int q) {
  int lbr1=0,flag=0;
  if (p > q) {

    panic ("%d %d",p,q);
    assert(0);
     
    /* Bad expression */
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    if(tokens[p].type==num)
       return atoi(tokens[p].str);
    if(tokens[p].type==hex)
       { 
	  uint32_t a;
	  sscanf(tokens[p].str,"%x",&a);
       return a;
       }
    if(tokens[p].type==reg)
	{
		if(!strcmp(tokens[p].str,"$eax"))  //eax, ecx, edx, ebx, esp, ebp, esi, edi,eip
			return cpu.eax;     
		if(!strcmp(tokens[p].str,"$ebx"))
			return cpu.ebx;
		if(!strcmp(tokens[p].str,"$ecx"))
			return cpu.ecx;
		if(!strcmp(tokens[p].str,"$edx"))
			return cpu.edx;
		if(!strcmp(tokens[p].str,"$esp"))
			return cpu.esp;
		if(!strcmp(tokens[p].str,"$ebp"))
			return cpu.ebp;
		if(!strcmp(tokens[p].str,"$esi"))
			return cpu.esi;
		if(!strcmp(tokens[p].str,"$edi"))
			return cpu.edi;
		if(!strcmp(tokens[p].str,"$eip"))
			return cpu.eip;

         }
}
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    
    return eval(p + 1, q - 1);
  }
  else if (tokens[p].type==TK_minus&&( check_parentheses(p+1, q)==true||( (q-p==1)&&(tokens[p].type==num||tokens[p].type==hex||tokens[p].type==reg) ) ) )
		return -eval(p+1,q-1);
  else if (tokens[p].type==DEREF&&( check_parentheses(p+1, q)==true||( (q-p==1)&&(tokens[p].type==num||tokens[p].type==hex||tokens[p].type==reg) ) ) )
	{		
		uint32_t z;
		z = eval(p+1,q);
		return vaddr_read(z,4);
	}


  else {

    int op=0,i,op_type;
    uint32_t val1;
    uint32_t val2;

    if(lbr!=0)  
	 { 
	        printf("%d",lbr);
		panic("bad expression"); assert(0); 
	 }  

    else 
         {
	     for(i=p;i<=q;i++) //TK_plus,TK_sub,num,TK_mul,TK_div,TK_lbr,TK_rbr
		{
		    if(tokens[i].type==TK_lbr)  
                        lbr1++;
		    if(tokens[i].type==TK_rbr)
			lbr1--;
                    if(lbr1<0)
			{ panic("bad expressioon");assert(0);}
		    if((tokens[i].type==TK_and)&&(lbr1==0)){
			  flag=1;op=i;}
		    else if((tokens[i].type==TK_EQ||tokens[i].type==TK_NEQ)&&(lbr1==0)&&(flag!=1)){
			  flag=2;op=i;}
		    else if((tokens[i].type==TK_LEQ)&&(lbr1==0)&&((flag!=1)&&(flag!=2))){
			  flag=3;op=i;}
		    else if((tokens[i].type==TK_plus||tokens[i].type==TK_sub)&&(lbr1==0)&&((flag!=1)&&(flag!=2)&&(flag!=3)))
			{ flag =4;op=i;
			}
		    else if((tokens[i].type==TK_mul||tokens[i].type==TK_div)&&(flag!=1)&&(lbr1==0)&&((flag!=1)&&(flag!=2)&&(flag!=3)&&(flag!=4)))
			     op=i;
			
		}
	 }

    val1 = eval(p, op - 1);
    val2 = eval(op + 1, q);
    op_type = tokens[op].type;
    
    switch (op_type) {
      case TK_plus: return (val1 + val2);
      case TK_sub: return (val1 - val2);/* ... */
      case TK_mul: return (val1 * val2);/* ... */
      case TK_div: return (val1 / val2);/* ... */
      case TK_EQ: return (val1 == val2);
      case TK_NEQ: return (val1 != val2);
      case TK_and: return (val1 && val2);
      case TK_LEQ: return (val1 <= val2);


      default: assert(0);
    }
  }
return 0;
}



uint32_t expr(char *e, bool *success) {
  int i;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  else 
    { 
      //for (i=0;i<nr_token;i++)
      //printf("%d %s\n",tokens[i].type,tokens[i].str);
	for (i = 0; i < nr_token; i ++) {

  	if (tokens[i].type == '-' && (i == 0 || ( (tokens[i - 1].type != num)&&(tokens[i - 1].type != TK_rbr)&&(tokens[i - 1].type != hex)&&(tokens[i - 1].type != reg) ) ) ) {
   	 tokens[i].type = TK_minus;
 	 }

  	if (tokens[i].type == '*' && (i == 0 || ( (tokens[i - 1].type != num)&&(tokens[i - 1].type != TK_rbr)&&(tokens[i - 1].type != hex)&&(tokens[i - 1].type != reg) ) ) ) {
   	 tokens[i].type = DEREF;
 	 }
	}

     }
  /* TODO: Insert codes to evaluate the expression. */
  return eval(0,nr_token-1);
    
}






//p (1+(3*2) +(($eax-$eax) +(*$eip-1**$eip)+(0x5--5+  *0X100005 - *0x100005) )*4)








