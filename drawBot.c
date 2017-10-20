// An initial A.I. player for Final Card-Down.
//
// Created by:
//  Elizabeth Willer (z5161594) <z5161594@student.unsw.edu.au>
//  Mitchell Roberts (z5117664) <z5117664@student.unsw.edu.au>
// Created on 2017-10-20
// Alex Linker (F11A-Kora)
//

#include "Game.h"
#include "player.h"

#define NOT_FOUND -1

// Find a card in the player's hand that matches the specified color,
// if such a card exists.
// Returns the card index, or NOT_FOUND if no matching card was found.
static int findMatchingCardColor (Game game, color color);

// Do two cards match on either value, color, or suit?
// Returns TRUE if they match any of the above features, and
// FALSE if they don't match on any of the above features.
static int doCardsMatch (Card first, Card second);

// Determine whether the player can currently draw a card.
// If they can't draw a card, they should probably end their turn.
static int canDrawCard (Game game);

// Determine whether the current player should SAY_UNO.
// There are two different situations where it could be a
// valid move to SAY_UNO.
// For now, just deal with the simple situation: "claim card".
// Note: there are several possible ways to determine this.
static int shouldSayUNO (Game game);

// Determine whether the current player should SAY_UNO.
// There are two different situations where it could be a
// valid move to SAY_DUO.
// For now, just deal with the simple situation: "claim card".
// Note: there are several possible ways to determine this.
static int shouldSayDUO (Game game);

// Determine whether the current player should SAY_UNO.
// There are two different situations where it could be a
// valid move to SAY_TRIO.
// For now, just deal with the simple situation: "claim card".
// Note: there are several possible ways to determine this.
static int shouldSayTRIO (Game game);

// Check if player has said (x) after playing a card
static int hasCalled (Game game, action call);

// This function is to be implemented by the A.I.
// It will be called when the player can make an action on their turn.
//
// !!!!!   The function is called repeatedly, until   !!!!!
// !!!!!      they make the action `END_TURN`.        !!!!!
//
// If the player's turn is skipped, this funciton
// is *not* called for that player.
//
// Don't forget about the `isValidMove` function -- it's a handy way
// to work out before you play a move whether or not it will be valid
// (and you should only ever be making valid moves).
//
// You can also use it to help you work out where you are at in the
// game, without needing to look through all of the previous state
// yourself --
//
// Looking at the diagram of valid moves at any given point in the game,
// we can see that at the start of the game, it's valid to:
//   - call somebody out for forgetting to SAY_UNO / SAY_DUO / SAY_TRIO,
//   - draw a card,
//   - play a card, *if* you have a valid card that you can play.
//
// It's not valid to end your turn unless you've done some other
// action/s (again, see the diagram).
//
// We can take advantage of that for our very simple A.I. to determine
// where we are at in our turn, and thus what move we should make.


playerMove decideMove(Game game) {
    // Start out by making a move struct, to say what our move is.
    playerMove move;
    
    // 1. Call someone out if we can (n/a)
    // 2. Say something if we should (just played, has certain card count)
    if (shouldSayUNO(game) == TRUE) {
        move.action = SAY_UNO;
    } else if (shouldSayDUO(game) == TRUE) {
        move.action = SAY_DUO;
    } else if (shouldSayTRIO(game) == TRUE) {
        move.action = SAY_TRIO;
    }
    // else if (cardValue(topDiscard(game)) == DECLARE) {
    //} 
    // 3. Play a card if we can
        // Match with top card, or
        // Match's declared color
    // 4. Pick up a card if valid.
    // 5. End turn

    // Set our move to be END_TURN, and check whether that's
    // a valid move -- if it is, then just end our turn (for now).
    move.action = END_TURN;

    return move;
}

static int canPlayCard(Game game) {
    int cardIndex = NOT_FOUND;
    if (cardValue(topDiscard(game)) == DECLARE) {
        
        //cardIndex = findMatchingCardColor(game, );
    }
    
    return cardIndex;
}

// Find a card in the player's hand that matches the specified color,
// if such a card exists.
// Returns the card index, or NOT_FOUND if no matching card was found.
static int findMatchingCardColor (Game game, color color) {
    int i = 0;
    int match = NOT_FOUND;
    while (i < handCardCount(game)) {
        if (cardColor(handCard(game, i)) == color) {
            match = i;
        }
        i++;
    }
    
    return match;
}

// Do two cards match on either value, color, or suit?
// Returns TRUE if they match any of the above features, and
// FALSE if they don't match on any of the above features.
static int doCardsMatch (Card first, Card second) {
    int match = FALSE;
    if (cardValue(first) == cardValue(second)
        || cardSuit(first) == cardSuit(second)
        || cardColor(first) == cardSuit(second)) {
        match = TRUE;
    }

    return match;
}

// Determine whether the player can currently draw a card.
// If they can't draw a card, they should probably end their turn.
static int canDrawCard (Game game) {
    playerMove move;
    move.action = DRAW_CARD;
    return isValidMove(game, move);
}

// Determine whether the current player should SAY_UNO.
// There are two different situations where it could be a
// valid move to SAY_UNO.
// For now, just deal with the simple situation: "claim card".
// Note: there are several possible ways to determine this.
static int shouldSayUNO (Game game) {
    int sayUno = FALSE;
    if ((handCardCount(game) == 1) && (hasCalled(game, SAY_UNO) == FALSE)) {
        sayUno = TRUE;
    }
    return sayUno;
}

// Determine whether the current player should SAY_DUO.
// There are two different situations where it could be a
// valid move to SAY_DUO.
// For now, just deal with the simple situation: "claim card".
// Note: there are several possible ways to determine this.
static int shouldSayDUO (Game game) {
    int sayDuo = FALSE;
    if ((handCardCount(game) == 2) && (hasCalled(game, SAY_DUO) == FALSE)) {
        sayDuo = TRUE;
    }
    return sayDuo;
}

// Determine whether the current player should SAY_TRIO.
// There are two different situations where it could be a
// valid move to SAY_TRIO.
// For now, just deal with the simple situation: "claim card".
// Note: there are several possible ways to determine this.
static int shouldSayTRIO (Game game) {
    int sayTrio = FALSE;
    if ((handCardCount(game) == 2) && (hasCalled(game, SAY_TRIO) == FALSE)) {
        sayTrio = TRUE;
    }
    return sayTrio;
}

static int hasCalled (Game game, action call) {
    int i = 0;
    int callHasBeenSaid = FALSE;
    int cardHasBeenPlayed = FALSE;
    while (i < turnMoves(game, currentTurn(game))) {
        playerMove move = pastMove(game, currentTurn(game), i);
        if (move.action == PLAY_CARD) {
            cardHasBeenPlayed = TRUE;
        }
        if ((move.action == call) && (cardHasBeenPlayed == TRUE)) {
            callHasBeenSaid = TRUE;
        }
    }
    
    return callHasBeenSaid;
}
