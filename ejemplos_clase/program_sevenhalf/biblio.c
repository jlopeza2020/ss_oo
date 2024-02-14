#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "biblio.h"

static int
tonumber(char *str)
{
	int n;

	if (strcmp("king", str) == 0) {
		return King;
	}
	if (strcmp("queen", str) == 0) {
		return Queen;
	}
	if (strcmp("jack", str) == 0) {
		return Jack;
	}
	if (strcmp("ace", str) == 0) {
		return Ace;
	}

	n = atoi(str);
	if (n >= 2 && n <= 10) {
		return n;
	}
	return -1;
}

static int
tosuit(char *str)
{
	if (strcmp("clubs", str) == 0) {
		return Clubs;
	}
	if (strcmp("diamonds", str) == 0) {
		return Diamonds;
	}
	if (strcmp("hearts", str) == 0) {
		return Hearts;
	}
	if (strcmp("spades", str) == 0) {
		return Spades;
	}

	return -1;
}

static char *
suittostr(int suit)
{
	switch (suit) {
	case Clubs:
		return "Clubs";
	case Diamonds:
		return "Diamonds";
	case Hearts:
		return "Hearts";
	case Spades:
		return "Spades";
	default:
		return NULL;
	}
}

static char *
numbertostr(int num)
{
	switch (num) {
	case 2:
		return "Two";
	case 3:
		return "Three";
	case 4:
		return "Four";
	case 5:
		return "Five";
	case 6:
		return "Six";
	case 7:
		return "Seven";
	case 8:
		return "Eight";
	case 9:
		return "Nine";
	case 10:
		return "Ten";
	case Jack:
		return "Jack";
	case Queen:
		return "Queen";
	case King:
		return "King";
	case Ace:
		return "Ace";
	default:
		return NULL;
	}
}

Card *
newcard(char *number, char *suit)
{
	Card *c;

	int su;
	int num;

	num = tonumber(number);
	if (num < 0) {
		return NULL;
	}
	su = tosuit(suit);

	if (su < 0) {
		return NULL;
	}

	c = malloc(sizeof(Card));
	if (c == NULL) {
		return NULL;
	}

	c->number = num;
	c->suit = su;

	return c;
}

int
ishonor(Card *c)
{
	return c->number == King || c->number == Queen ||
	    c->number == Jack || c->number == Ace;
}

int
number(Card *c)
{
	return c->number;
}

int
suit(Card *c)
{
	return c->suit;
}

void
printcard(Card *c)
{
	char *s;

	s = suittostr(c->suit);

	if (ishonor(c)) {
		printf("[%s %s]", numbertostr(c->number), s);
	} else {
		printf("[%d %s]", c->number, s);
	}
}
