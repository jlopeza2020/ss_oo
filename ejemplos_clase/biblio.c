#include <string.h> 
#include "biblio.h"


static int 
tonumber(char *str){ 
    int n; 

    if (strcmp("king", str) == 0){ 
        return King;
    } 

    n = atoi(str);
    if( n >= 2 && n <= 10){ 
        return n
    }
}

static int
tosuit(char *str)
{ 
    if()

    return -1
}

static char * 
numbertostr(int num)
{
     switch(num){ 

     }

     return NULL;
}


Card *
newcard(char *number, char *suit)
{ 
    Card *c;

    int su;
    int num; 

    num = tonumber(number);
    if ( num < 0) { 
        return NULL;
    }
    su = tosuit(suit); 

    if (su < 0){ 
        return NULL;
    }

    c = malloc(sizeof(Card));
    if( c == NULL) { 
        return NULL;
    }

    c->number = num;
    c->suit = su;

    return c;
}

int 
ishonor(Card *c)
{ 
    return c->number == King ||  c->number == Queen || 
        c->number == Jack || c->number == Ace;
}

int
number(Card *c)
{ 
    return c->number;
}

int suit(Card *c){ 
    return c ->suit;
}

void
printcard(Card *c)
{ 
    char *s; 

    s = suittostr(c-> suit);
    if (ishonor(c)){ 
        printf("[%s %s]", numbertostr(c->number), s);
    }else{ 

    }
}