#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32



static WP wp_pool[NR_WP];
static WP *head, *free_ , *phead, *pfree_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
  phead = head;
  
  
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){

  if(free_==NULL)
	{
		assert(0);
	}	
  else
	{
		pfree_ = free_;
		phead->next = free_;
		free_->next = NULL;
		phead = free_; 
		free_ =pfree_->next;
                return phead;

	}  

}

void free_wp(WP* wp){
  WP* b;
  if(wp==NULL)
    printf("no used watchpoint\n");
  else
    {
         for(b=head->next;b!=NULL;b=b->next)
		{
			if(b->next==wp)
 			  {
   	 		        b->next = b->next->next;
			  }
		}

         pfree_ = free_;
  	 free_ = wp;
  	 free_ ->next = pfree_;
    }
}

bool checkwp(){
  int a;
  WP* b;
  bool  success1 = true;
  bool* success  = &success1;
  for(b=head->next;b!=NULL;b=b->next)
    {
	a = b->result;
        b->result = expr(b->s,success) ;
        if(a!=b->result)
           return true;

    }

  return false;

}

void print_wp(){
  WP* c;
  for(c=head->next;c!=NULL;c=c->next)
    {
	printf("num %d   string %s   result %d",c->NO,c->s,c->result);
    }
  return ;
}

WP* loc_wp(int a){
  WP* c;
  for(c=head->next;c!=NULL;c=c->next)
     {
	if(c->NO==a)
	   return c;
	 
     }
	   return NULL;
}     










