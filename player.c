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
#include <stdio.h>
#include <stdlib.h>

#define NOT_FOUND -1

// Determine whether the player can currently draw a card.
// If they can't draw a card, they should probably end their turn.
static int canDrawCard (Game game);

// Returns color most prevelant in current player's hand
static color commonestColor(Game game);

// Go to next player based on direction
static int cyclePlayer(int player, direction gameDirection);

// Returns color that is active (i.e. takes into account declares)
static color declaredColor(Game game);

// Do two cards match on either value, color, or suit?
// Returns TRUE if they match any of the above features, and
// FALSE if they don't match on any of the above features.
static int doCardsMatch (Card first, Card second);

// Find a card in the player's hand that matches the specified color,
// if such a card exists.
// Returns the card index, or NOT_FOUND if no matching card was found.
static int findMatchingCardColor (Game game, color color);

// Find index of card of a particular value in current player's hand.
static int findMatchingCardValue (Game game, value value);

// Check if player has said (x) after playing a card
static int hasCalled (Game game, action call);

// Returns index of playable card from hand
static int playableCard(Game game);

// Check if a anyone discarded on a particular turn
static int playerDiscarded(Game game, int turn);

// Check if it is correct to make a particular call
static int shouldCall (Game game, action call,
    int lastPlayer, int lastAction, int oppCardsPlayed);

// Return the opposite direction
static direction swapDirection(direction toSwap);

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
    // Get data
    int lastPlayer = cyclePlayer(currentPlayer(game),
        swapDirection(playDirection(game)));
    int i = 0;
    int priority = 0;
    Card topCard = topDiscard(game);
    playerMove move;
    playerMove opponentMove;
    playerMove currentPlayerMove;
    playerMove lastPlayerMove;
    int lastAction = -1;
    int playerUsed[6] = {FALSE};
    int numCardsDrawn = 0;
    int oppCardsPlayed = 0;
    int drawTwosPlayed = 0;
    int opponentMoves = 0;
    int j = 1;
    int foundOpponentCard = FALSE;
    
    if (currentTurn(game) > 0) {
        opponentMoves = turnMoves(game, currentTurn(game) - 1);
    }

    // Loop through opponents turn to see what they played
    while (j <= opponentMoves && foundOpponentCard == FALSE) {
        opponentMove = pastMove(game, currentTurn(game) - 1,
            opponentMoves - j);
        if (opponentMove.action == PLAY_CARD) {
            if (cardValue(opponentMove.card) == DRAW_TWO) {
                drawTwosPlayed++;
            }
            foundOpponentCard = TRUE;
            oppCardsPlayed++;
        }
        oppUsed[opponentMove.action] = TRUE;
        j++;
        
    }
   
    /*if (oppPlayedDT == TRUE) {
        printf("Opponent has played DRAW_TWO\n");
    } else if (oppPlayedDT == FALSE) {
        printf("Opponent has not played DRAW_TWO\n");
    }*/

    int playerMoves = turnMoves(game, currentTurn(game));
    int drawTwoPos = findMatchingCardValue(game, DRAW_TWO);

    if (playerMoves != 0) {
        firstMove = FALSE;
    }


    j = 1;
    // Check if the current player has drawn two or more
    while (j <= playerMoves) {
        currentPlayerMove = pastMove(game, currentTurn(game),
            playerMoves - j);
        if (j == 1) {
            // Get last move player took
            lastPlayerMove = currentPlayerMove;
            lastAction = lastPlayerMove.action;
        }
        playerUsed[currentPlayerMove.action] = TRUE;

        if (currentPlayerMove.action == DRAW_CARD) {
            numCardsDrawn++;
        }
        j++;
    }

    // Set default move
    // IF PREVIOUS MOVE WAS DRAW CARD THEN END THE TURN
    // IF THE PREVIOUS MOVE WAS SOMETHING ELSE THEN DRAW A CARD
    move.action = DRAW_CARD;
    if (isValidMove(game, move) == FALSE) {
        move.action = END_TURN;
    }

    // Call out if necessary
    if (shouldCall(game, SAY_UNO, lastPlayer,
        lastAction, oppCardsPlayed) == TRUE) {
        move.action = SAY_UNO;
    } else if (shouldCall(game, SAY_DUO, lastPlayer,
        lastAction, oppCardsPlayed) == TRUE) {
        move.action = SAY_DUO;
    } else if (shouldCall(game, SAY_TRIO, lastPlayer,
        lastAction, oppCardsPlayed) == TRUE) {
        move.action = SAY_TRIO;
    // Check if I just played a card that wasn't a continue,
    // if so end turn
    } else if (playerUsed[PLAY_CARD] == TRUE
        && cardValue(lastPlayerMove.card) != CONTINUE) {
            move.action = END_TURN;
    // Play DRAW_TWO on last DRAW_TWO if possible
    } else if (drawTwosPlayed > 0
        && numCardsDrawn == 0
        && drawTwoPos != NOT_FOUND) {
            move.action = PLAY_CARD;
            move.card = handCard(game, drawTwoPos);
            printf("Playing DRAW_TWO.\n");  
    // Check if I need to draw more cards still
    } else if (drawTwosPlayed > 0) {
        // Leave move as is
    } else if (numCardsDrawn == 1) {
            move.action = END_TURN;
    } else {
        // Loop through all cards, see if any can be played.
        while (i < handCardCount(game)) {
            Card currentCard = handCard(game, i);
            // Check if can play a draw two on a draw two
            if ((cardValue(topCard) == DRAW_TWO
                && cardValue(currentCard) == DRAW_TWO)) {
                move.action = PLAY_CARD;
                move.card = currentCard;
                priority = 3;
            // Check if a wild was played - if so what color
            } else if ((cardValue(topCard) == DECLARE
                && cardColor(currentCard) == declaredColor(game))
                && priority < 3) {
                move.action = PLAY_CARD;
                move.card = currentCard;
                priority = 2;
            // Check if any cards can be played normally
            } else if ((cardSuit(currentCard) == cardSuit(topCard)
                || cardColor(currentCard) == cardColor(topCard)
                || cardValue(currentCard) == cardValue(topCard))
                && priority < 1) {
                move.action = PLAY_CARD;
                move.card = currentCard;
                priority = 1;
            }
            i++;
        }

        // Declare color if necessary
        if (move.action == PLAY_CARD
            && cardValue(move.card) == DECLARE) {
            move.nextColor = commonestColor(game);
        }
    }

    return move;
}

// STATIC HELPER FUNCTIONS


// Determine whether the player can currently draw a card.
// If they can't draw a card, they should probably end their turn.
static int canDrawCard (Game game) {
    playerMove move;
    move.action = DRAW_CARD;
    return isValidMove(game, move);
}

static color commonestColor(Game game) {
    int i = RED;
    int currentCount = 0;
    int greatestCount = 0;
    int handSize = handCardCount(game);
    color commonest = RED;

    while (i <= PURPLE) {
        int j = 0;
        while (j < handSize) {
            if (cardColor(handCard(game, j)) == i) {
                currentCount++;
            }
        }
        if (currentCount > greatestCount) {
            commonest = i;
            greatestCount = currentCount;
        }
        i++;
    }

    return commonest;
}


// Return the player that should go next
static int cyclePlayer(int player, direction gameDirection) {
    if (gameDirection == CLOCKWISE) {
        player++;
    } else {
        player--;
    }
    player = player % 4;
    if (player < 0) {
        player += 4;
    }

    return player;
}

// Return the active color
static color declaredColor(Game game) {
    color declared = cardColor(topDiscard(game));
    if (cardValue(topDiscard(game)) == DECLARE) {
        int foundDeclared = -1;
        while (foundDeclared == -1) {
            int turn = currentTurn(game) - 1;
            while (turn >= 0) {
                int moveNumber = turnMoves(game, turn) - 1;
                playerMove move = pastMove(game, turn, moveNumber);
                if (move.action == PLAY_CARD
                    && cardValue(move.card) == DECLARE) {
                    declared = move.nextColor;
                }
            }

        }
    }

    return declared;
}

// Do two cards match on either value, color, or suit?
// Returns TRUE if they match any of the above features, and
// FALSE if they don't match on any of the above features.
static int doCardsMatch (Card first, Card second) {
    int match = FALSE;
    if (cardValue(first) == cardValue(second)
        || cardSuit(first) == cardSuit(second)
        || cardColor(first) == cardColor(second)) {
        match = TRUE;
    }

    return match;
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

// Find a card in the player's hand that matches the specified value,
// if such a card exists.
// Returns the card index, or NOT_FOUND if no matching card was found.
static int findMatchingCardValue (Game game, value value) {
    int i = 0;
    int match = NOT_FOUND;
    while (i < handCardCount(game)) {
        if (cardValue(handCard(game, i)) == value) {
            match = i;
        }
        i++;
    }
    
    return match;
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
        i++;
    }
    
    return callHasBeenSaid;
}

// Returns index of a playable card
static int playableCard(Game game) {
    int cardIndex = NOT_FOUND;
    if (cardValue(topDiscard(game)) == DECLARE) {
        cardIndex = findMatchingCardColor(game, declaredColor(game));
    }
    //  else if (cardValue(topDiscard(game)) == TWO) {
    //     cardIndex++
    // }
    
    return cardIndex;
}

// Check if a player discarded at any point during a turn
static int playerDiscarded(Game game, int turn) {
    int i = 0;
    while (i < turnMoves(game, turn)) {
        if (pastMove(game, turn, i).action == PLAY_CARD) {
            return TRUE;
        }
        i++;
    }
    return FALSE;
}

// Determine whether the current player should SAY_UNO.
// There are two different situations where it could be a
// valid move to SAY_UNO.
// For now, just deal with the simple situation: "claim card".
// Note: there are several possible ways to determine this.
static int shouldCall (Game game, action call, int lastPlayer,
    int lastAction, int oppCardsPlayed) {
    int say = FALSE;
    // Assume is valid (that check is elsewhere)
    if (lastAction == PLAY_CARD && (handCardCount(game) == call - 1)) {
        say = TRUE;
    } else if (lastAction == -1) {
        int opponentCards = playerCardCount(game, lastPlayer);
        if (opponentCards <= (call - 1)
            && opponentCards + oppCardsPlayed > (call - 1)) {
            say = TRUE;
        }
    }
    return say;
}


// Return opposite direction
static direction swapDirection(direction toSwap) {
    if (toSwap == CLOCKWISE) {
        return ANTICLOCKWISE;
    } else {
        return CLOCKWISE;
    }
}
