// The playing card implementation.
//
// This is an implementation of the Card ADT, which represents a single
// playing card in the game of Final Card-Down.
//
// By:
//  Elizabeth Willer (z5161594) <z5161594@student.unsw.edu.au>
//  Mitchell Roberts (z5117664) <z5117664@student.unsw.edu.au>
// Lily
// Created on 2017-09-22
// Alex Linker (F11A-Kora)


#include "Card.h"
#include <stdio.h>
#include <stdlib.h>


// The following typedef is included in Card.h:
//         typedef struct _card *Card;
// This means that a struct card has to be implemented *somewhere*, but
// not necessarily in the .h file, as we have done with concrete types.
//
// The way we implement this in an ADT is that you design your own card
// struct, with the fields etc that you need to implement your ADT.

typedef struct _card {
    suit checkSuit;
    color checkColor;
    value checkValue;
} card;



// The interface functions are below. You need to implement these.

// Create a new card.
// These values can only be set at creation time.
// The value should be between 0 and F.
Card newCard(value value, color color, suit suit) {
    Card new = calloc (1, sizeof(card));
    // Check if calloc has succeeded
    if (new == NULL) {
        printf("failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }
    // Puts the value in the card struct if it is valid
    if (value > F) {
        printf ("invalid value\n");
        exit (EXIT_FAILURE);
    } else {
        new->checkValue = value;
    }
    // Puts the suit in the card struct if it is valid
    new->checkSuit = suit;
    // Puts the color in the card struct if it is valid
    new->checkColor = color;

    return new;
}

// Destroy an existing card.
void destroyCard(Card card) {
    free (card);
}

// Get the card suit (HEARTS, DIAMONDS, etc).
suit cardSuit(Card card) {
    return card->checkSuit;
}

// Get the card number (0x0 through 0xF).
value cardValue(Card card) {
    return card->checkValue;
}

// Get the card's color (RED, BLUE, etc).
color cardColor(Card card) {
    return card->checkColor;
}
