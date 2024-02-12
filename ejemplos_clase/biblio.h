

// Suits 
enum { 
    Clubs, 
    Diamonds,
    Hearts, 
    Spades,
};

// Numbers 
enum{ 
    Ace = 1, 
    Jack = 11, 
    Queen = 12, 
    King = 13,
};

struct Card { 
    int number; 
    int suit;
};

typedef struct Card Card; 

//declaración: no hace falta ponerle 
Card *newcard(char*, char*); 
int ishonor(Card *c);
int number(Card *); 
int suit(Card *);
void printcard(Card *);

