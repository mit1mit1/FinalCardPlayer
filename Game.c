// The game implementation.
//
// This is an implementation of the Game ADT, which represents a
//  game of Final Card-Down.
//
// By:
//  Elizabeth Willer (z5161594) <z5161594@student.unsw.edu.au>
//  Mitchell Roberts (z5117664) <z5117664@student.unsw.edu.au>
//  Brian Chan (z5165135) <z5165135@student.unsw.edu.au>
//  Lily Huang (z5110502) <z5110502@student.unsw.edu.au>
// Created on 2017-09-22
// Alex Linker (F11A-Kora)

#include "Card.h"
#include "Game.h"
#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>



//_____List of cards_______
typedef struct _cardNode *CardNode;
typedef struct _cardList *CardList;

//_____List of cards_______
typedef struct _cardNode {
    Card card;
    CardNode next;
} cardNode;

typedef struct _cardList {
    CardNode head;
} cardList;



//_____List of moves_______
typedef struct _playerMove *PlayerMove;
typedef struct _moveNode *MoveNode;
typedef struct _turn *Turn;

//_______________TURNLIST___________________
typedef struct _turnNode *TurnNode;
typedef struct _moveNode *MoveNode;

typedef struct _turnNode {
    TurnNode next;
    MoveNode head;
} turnNode;

typedef struct _moveNode {
    MoveNode next;
    playerMove move;
} moveNode;


// The following typedef is included in Game.h:
//         typedef struct _game *Game;
// This means that a struct game has to be implemented *somewhere*, but
// not necessarily in the .h file, as we have done with concrete types.
//
// The way we implement this in an ADT is that you design your own game
// struct, with the fields etc that you need to implement your ADT.

// Deck - a stack of cards (can only access the top)
// Discard pile - another stack (can only see the top)
// Linked list for each player's hand
// Linked list of past turns (each turn is a list of moves)
// int for turn number
// int for current player
// int array for player points
// direction
//
typedef struct _game {
    cardList deckPile;
    cardList discardPile;
    cardList *playerHands;
    TurnNode pastTurns;
    int turnNumber;
    int activePlayer;
    direction direction;
    color activeColor;
    int totalCards;
    int *cardsOfColor;
    int *cardsOfSuit;
    int *cardsOfValue;
} game;

static CardNode newCardNode (Card val);
static MoveNode newMoveNode (playerMove val);
static direction swapDirection(direction toSwap);
static int cyclePlayer(int player, direction gameDirection);
static int hasCard(Game game, Card card);
static int cardsEqual(Card firstCard, Card secondCard);
static int cardsSimilar(Card firstCard, Card secondCard);
static TurnNode newTurnNode (Game game, MoveNode firstMove);
static void linkMoveToPastTurns (Game game, MoveNode playedMove);
static void playHandCard(Game game, playerMove move);
static void giveCards(Game game, int player, int numCards);

// Create a new game engine.
//
// This creates a game with a deck of the given size
// and the value, color, and suit of each card to be taken from
// each of the arrays in order.
//
// Your game will always have 4 players. Each player starts with a hand
// of 7 cards. The first card from the deck is given to player 0, the
// second to player 1, the third to player 2, the fourth to player 3,
// the fifth to player 0, and so on until each player has 7 cards.

Game newGame(int deckSize, value values[], color colors[], suit suits[]) {

    // Set aside memory for base game struct
    Game new = calloc(1, sizeof(game));

    if (new == NULL) {
        printf("Failed to allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    
    new->pastTurns = calloc(1, sizeof(struct _turnNode));
    //new->pastTurns = NULL;


    // Always 4 player hands - set aside memory for this array
    new->playerHands = calloc(4, sizeof(cardList));

    // Set simple initial values
    new->turnNumber = 0;
    new->activePlayer = 0;
    new->direction = CLOCKWISE;
    new->totalCards = deckSize;

    // Set aside memory for arrays that store constants for
    // counting cards of various types
    new->cardsOfColor = calloc(PURPLE + 1, sizeof(int));
    new->cardsOfSuit = calloc(QUESTIONS + 1, sizeof(int));
    new->cardsOfValue = calloc(F + 1, sizeof(int));

    // Set pointers to use in loop
    CardNode currentCard;
    CardNode prevCard;

    int i = 0;

    // Loop through i, create cards to fill deck.
    while (i < deckSize) {
        // Increment out counters for cards of various types
        new->cardsOfValue[values[i]] += 1;
        new->cardsOfColor[colors[i]] += 1;
        new->cardsOfSuit[suits[i]] += 1;

        // Create the card, keep a pointer to it.
        Card c = newCard(values[i], colors[i], suits[i]);
        // Make a card node which will store this pointer.
        currentCard = newCardNode(c);

        if (i != 0) {
            // Bottom of deck has nothing below it
            // Otherwise, make the last card point 
            // to the card 'below' it.
            prevCard->next = currentCard;
        } else {
            // Finally, we have a stack of cards - make the deck in the
            // game struct point to the top of the stack
            new->deckPile.head = currentCard;
        }

        // Increment counter and temporary pointer
        prevCard = currentCard;
        i++;
    }


    // Reset pointers
    i = 0;
    int j = 0;

    // We'll use this to store the 'top' card in each player's hand
    // so far.
    CardNode lastCard[4];

    // 4 hands, 7 cards each - so loop to 28
    while (i < 28) {

        // Get a card from the top of the deck
        currentCard = new->deckPile.head;
        // Replace the top fo the deck with the card below it.
        new->deckPile.head = currentCard->next;

        // 'Top' of each player's hand
        if (i < 4) {
            new->playerHands[j].head = currentCard;
        // Otherwise, make the 'bottom' card in their
        // hand point to this one
        } else {
            lastCard[j]->next = currentCard;
        }

        // Actual 'bottom' of player's hands should point to null
        if (i > 23) {
            currentCard->next = NULL;
        }

        // Store the pointer to the card we just put in the player's
        // hand as the last card put in that hand. 
        lastCard[j] = currentCard;

        // Increment
        i++;
        j++;

        // Make j the next player (between 0 and 3)
        j = j % 4;
    }

    // Get a card from the top of the deck
    currentCard = new->deckPile.head;
    // Put it on the discard pile
    new->discardPile.head = currentCard;
    // Replace the top fo the deck with the card below it.
    new->deckPile.head = currentCard->next;
    // Make the bottom of the discard pile point to nothing
    currentCard->next = NULL;

    return new;
}

// Destroy an existing game.
//
// This should free all existing memory used in the game including
// allocations for players and cards.
void destroyGame(Game game) {

    // Start with the top of the deck
    CardNode currentCard = game->deckPile.head;
    // Temp variable so we can free the node after finding the next one
    CardNode prevCard;

    // Loop through all of deck
    while (currentCard != NULL) {
        // Free the actual card the node points to
        free(currentCard->card);
        // Keep the pointer to the node, find the next node
        prevCard = currentCard;
        currentCard = currentCard->next;
        // Free the node itself once we know where the next one is
        free(prevCard);
    }

    // Loop through all of discard pile
    currentCard = game->discardPile.head;

    while (currentCard != NULL) {
        free(currentCard->card);
        prevCard = currentCard;
        currentCard = currentCard->next;
        free(prevCard);
    }

    // Loop through each players hand
    int i = 0;
    while (i < 4) {
        currentCard = game->playerHands[i].head;

        while (currentCard != NULL) {
            free(currentCard->card);
            prevCard = currentCard;
            currentCard = currentCard->next;
            free(prevCard);
        }
        i++;
    }

    // Free all the arrays that the struct points to
    free(game->playerHands);
    free(game->cardsOfColor);
    free(game->cardsOfSuit);
    free(game->cardsOfValue);

    // Free past moves

    // Free the struct itself
    free(game);
}

// The following functions can be used by players to discover
// information about the state of the game.

// Get the number of cards that were in the initial deck.
int numCards(Game game) {
    return game->totalCards;
}

// Get the number of cards in the initial deck of a particular
// suit.
// Remember, enums are ints
int numOfSuit(Game game, suit suit) {
    return game->cardsOfSuit[suit];
}

// Get the number of cards in the initial deck of a particular color.
int numOfColor(Game game, color color) {
    return game->cardsOfColor[color];

}

// Get the number of cards in the initial deck of a particular value.
int numOfValue(Game game, value value) {
    return game->cardsOfValue[value];
}

// Get the number of the player whose turn it is.
int currentPlayer(Game game) {
    return game->activePlayer;
}


// Get the current turn number.
//
// The turn number increases after a player ends their turn.
// The turn number should start at 0 once the game has started.
int currentTurn(Game game) {
    return game->turnNumber;
}

// Get the number of points for a given player.
// Player should be between 0 and 3.
//
// This should _not_ be called by a player.
int playerPoints(Game game, int player) {
    return 0;
}

// Get the current direction of play.
direction playDirection(Game game) {
    return game->direction;
}

// _______________________LIZ_START_____________________________________
// This function returns the number of turns that have occurred in the
// game so far including the current turn.
// When using either the turnMoves or pastMove function,
// the turn number should be less than the number of moves that this
// function returns.
// (i.e. on turn 0 of the game, this should return 1, as there has been
// 1 turn so far including the current turn if you called pastMove you
// would need to call it on turn 0, as this is the only possible value
// less than 1.)
// This function returns the number of turns that have occurred in the
// game so far including the current turn.
// When using either the turnMoves or pastMove function,
// the turn number should be less than the number of moves that this
// function returns.
// (i.e. on turn 0 of the game, this should return 1, as there has been
// 1 turn so far including the current turn if you called pastMove you
// would need to call it on turn 0, as this is the only possible value
// less than 1.)
int numTurns(Game game) {
    return currentTurn(game) + 1;
}

// Get the number of moves that happened on a turn.
// A turn may consist of multiple moves such as drawing cards,
// playing cards, and ending the turn.
int turnMoves(Game game, int turn) {
    TurnNode currTurn = game->pastTurns;
    int numMoves = 0;
    int numTurns = 0;
    if (currTurn != NULL) {
        while (currTurn->next != NULL && numTurns < turn) {
            currTurn = currTurn->next;
            numTurns++;
        }
        MoveNode currMove = currTurn->head;
        if (currMove != NULL) {
            // Has already been one move
            numMoves++;
            while (currMove->next != NULL) {
                currMove = currMove->next;
                numMoves++;
            }
        }
    }
    
    return numMoves;
}

// Look at a previous move from a specified turn.
playerMove pastMove(Game game, int turn, int move) {

    TurnNode currTurn = game->pastTurns;
    int numMoves = 0;
    int numTurns = 0;
    while (currTurn->next != NULL && numTurns < turn) {
        if (currTurn->next) currTurn = currTurn->next;
            numTurns++;
        }
    // What happens if no moves have been made. 
    MoveNode currMove = currTurn->head;
    while ((currMove->next != NULL) && (numMoves < move)) {
        currMove = currMove->next;
        numMoves++;
    }
    
    return currMove->move;
}

//______________________LIZ_END_________________________________________

// Get the number of cards in a given player's hand.
int playerCardCount(Game game, int player) {
        
    int cardCount = 0;
    CardNode currentCard = game->playerHands[player].head;

    while (currentCard != NULL) {
        // printf("currentCard address: %p\n", currentCard);
        // printf("next currentCard address: %p\n", currentCard->next);
        cardCount++;
        currentCard = currentCard->next;
    }
    
    return cardCount;
}

// Get the number of cards in the current player's hand.
int handCardCount(Game game) {
    return playerCardCount(game, currentPlayer(game));
}

// View a card from the current player's own hand.
//
// The player should not need to free() this card,
// so you should not allocate or clone an existing card
// but give a reference to an existing card.
Card handCard(Game game, int card) {
    int playerNum = currentPlayer(game);
    CardNode currentCard = game->playerHands[playerNum].head;
    int cardIndex = 0;
    while (currentCard->next != NULL && cardIndex < card) {
        cardIndex++;
        currentCard = currentCard->next;
    }
    return currentCard->card;
}

// Get the card that is on the top of the discard pile.
Card topDiscard(Game game) {
    if (game->discardPile.head == NULL) {
        return NULL;
    }
    return game->discardPile.head->card;
}

// Check if a given move is valid.
//
// If the last player played a 2 (DRAW_TWO),
// the next player must either play a 2
// or draw 2 cards.
// Otherwise, the player must play a card that is either a ZERO
// or that has the same color, value, or suit as the card on the top
// of the discard pile.
//
// If the player plays an ADVANCE, the next player's turn is skipped.
// If the player plays a BACKWARDS, the direction of play is reversed.
// If the player plays a CONTINUE, they may play another card.
// If the player plays a DECLARE, they must also state which color
// the next player's discarded card should be.
//
// A player can only play cards from their hand.
// A player may choose to draw a card instead of discarding a card.
// A player must draw a card if they are unable to discard a card.
//
// This check should verify that:
// * The card being played is in the player's hand
// * The player has played at least one card before finishing their turn,
//   unless a draw-two was played, in which case the player may not
//   play a card, and instead must draw the appropriate number of cards.
int isValidMove(Game game, playerMove move) {
    int isValid = TRUE;

    // Get data
    int lastPlayer = cyclePlayer(currentPlayer(game),
        swapDirection(playDirection(game)));

    // Is this the start of the current players turn?
    int firstMove = TRUE;
    playerMove opponentMove;
    playerMove currentPlayerMove;
    playerMove lastPlayerMove;
    int oppUsed[6] = {FALSE};
    int playerUsed[6] = {FALSE};
    int numCardsDrawn = 0;
    int oppCardsPlayed = 0;
    int opponentValuePlayed = -1;
    int opponentMoves = turnMoves(game, currentTurn(game) - 1);
    int playerMoves = turnMoves(game, currentTurn(game));
    int canCallOut = TRUE;

    if (turnMoves(game, currentTurn(game)) != 0) {
        firstMove = FALSE;
    }

    // Loop through opponents turn to see what they played
    int foundOpponentCard = FALSE;
    int j = 1;
    while (j <= opponentMoves && foundOpponentCard == FALSE) {
        opponentMove = pastMove(game, currentTurn(game) - 1,
            opponentMoves - j);
        if (opponentMove.action == PLAY_CARD) {
            opponentValuePlayed = cardValue(opponentMove.card);
            foundOpponentCard = TRUE;
            oppCardsPlayed++;
        }
        oppUsed[opponentMove.action] = TRUE;
        j++;
    }

    j = 1;
    // Check if the current player has drawn two or more
    while (j <= playerMoves) {
        currentPlayerMove = pastMove(game, currentTurn(game),
            playerMoves - j);
        if (j == 1) {
            // Get last move player took
            lastPlayerMove = currentPlayerMove;
        }
        playerUsed[currentPlayerMove.action] = TRUE;

        if (currentPlayerMove.action == DRAW_CARD) {
            canCallOut = FALSE;
            numCardsDrawn++;
        } else if (currentPlayerMove.action == PLAY_CARD) {
            canCallOut = FALSE;
        }
        j++;
    }

    // Always allowed to draw a card
    if (move.action == DRAW_CARD) {
        if (opponentValuePlayed == DRAW_TWO) {
            if (numCardsDrawn >= 2) {
                isValid = FALSE;
            }
        } else if (numCardsDrawn >= 1) {
            printf("%d\n", opponentValuePlayed);
            isValid = FALSE;
        }
    // Check declaring conditions
    } else if (move.action == SAY_UNO
        || move.action == SAY_DUO
        || move.action == SAY_TRIO) {
        if (canCallOut == TRUE) {
            // Check if already said it
            if (playerUsed[move.action] == TRUE) {
                isValid = FALSE;
            }
            // Check if opponent should have said one of these on
            // their last turn.
            // Start with simple check that they should have ended
            // their turn with it.
            if (currentTurn(game) <= 1) {
                isValid = FALSE;
            } else if (oppUsed[move.action] == TRUE) {
                // Move is incorrect, but valid
                // Four card penalty
                isValid = TRUE;
                giveCards(game, currentPlayer(game), 4);
            } else {
                int opponentCards = playerCardCount(game, lastPlayer);
                if (move.action == SAY_UNO) {
                    if (opponentCards <= 1
                        && opponentCards + oppCardsPlayed > 1) {
                        // Two card penalty for failing to call
                        isValid = TRUE;
                        giveCards(game, lastPlayer, 2);
                    }
                } else if (move.action == SAY_DUO) {
                    if (opponentCards <= 2
                        && opponentCards + oppCardsPlayed > 2) {
                        isValid = TRUE;
                        giveCards(game, lastPlayer, 2);
                    }
                } else if (move.action == SAY_TRIO) {
                    if (opponentCards <= 3
                        && opponentCards + oppCardsPlayed > 3) {
                        isValid = TRUE;
                        giveCards(game, lastPlayer, 2);
                    }
                }
            }
        } else {
            // Check if current player should have said one of these
            if (handCardCount(game) != move.action - 1
                || lastPlayerMove.action != PLAY_CARD) {
                isValid = FALSE;
            }
        }
    // Check if player has failed to draw two
    } else if (currentTurn(game) >= 1
        && opponentValuePlayed == DRAW_TWO
        && numCardsDrawn < 2
        && move.action != PLAY_CARD) {
        printf("Failed to draw two.\n");
        isValid = FALSE;
    // Check if player is trying to skip their turn
    } else if (move.action == END_TURN && firstMove == TRUE) {
        printf("Ending without doing anything.\n");
        isValid = FALSE;
    // Check if player actually has the card they are trying to play
    } else if (move.action == PLAY_CARD
        && hasCard(game, move.card) == FALSE) {
        isValid = FALSE;
    // At long last, check if player is allowed to play the card
    // they just played
    } else if (move.action == PLAY_CARD) {
        // Can only play a draw two if opponent played a draw two
        if (opponentValuePlayed == DRAW_TWO
            && cardValue(move.card) != DRAW_TWO) {
            isValid = FALSE;
        // Otherwise, can always play a wild
        } else if (cardValue(move.card) == ZERO) {
            isValid = TRUE;
        // Otherwise, can always play a wild
        } else if (cardsSimilar(move.card, topDiscard(game)) == FALSE
            && cardColor(move.card) != game->activeColor) {
            isValid = FALSE;
        }
    }

    return isValid;
}

// Play the given action for the current player
//
// If the player makes the END_TURN move, their turn ends,
// and it becomes the turn of the next player.
//
// This should _not_ be called by the player AI.
void playMove(Game game, playerMove move) {

    if (isValidMove(game, move) == FALSE) {
        printf("Invalid!\n");
        //print error or exit function or something

    } else if (isValidMove(game, move) == TRUE) {
        
        MoveNode newMove = newMoveNode(move);

        if (newMove->move.action == SAY_UNO ||
            newMove->move.action == SAY_DUO ||
            newMove->move.action == SAY_TRIO) {

            linkMoveToPastTurns(game, newMove);

        } else if (newMove->move.action == END_TURN) {
            linkMoveToPastTurns(game, newMove);

            TurnNode currTurn = game->pastTurns;

            while (currTurn->next != NULL) {
                currTurn = currTurn->next;
            }

            MoveNode prevMoves = currTurn->head;
            int hasPlayedAdvance = 0;

            while (prevMoves != NULL && prevMoves->next != NULL) {
                if (prevMoves->move.action == PLAY_CARD
                    && cardValue(prevMoves->move.card) == ADVANCE) {
                    hasPlayedAdvance++;
                }
                prevMoves = prevMoves->next;
            }

            //Used Mitch's cyclePlayer function to get new activePlayer.
            //need to consider case where an 'A' card is played in turn.
            if (hasPlayedAdvance == 0) {
                game->activePlayer = cyclePlayer(game->activePlayer,
                game->direction);

            } else if (hasPlayedAdvance > 0) {
                game->activePlayer = cyclePlayer(game->activePlayer,
                game->direction);
                game->activePlayer = cyclePlayer(game->activePlayer,
                game->direction);

            }

            game->turnNumber++;

        } else if (newMove->move.action == DRAW_CARD) {

            int currPlayer = currentPlayer(game);
            giveCards(game, currPlayer, 1);
            linkMoveToPastTurns(game, newMove);

        } else if (newMove->move.action == PLAY_CARD) {
            playHandCard(game, newMove->move);

            if (cardValue(newMove->move.card) == DRAW_TWO) {
                //Do Nothing

            } else if (cardValue(newMove->move.card) == ADVANCE) {
                // Will alter END_TURN

            } else if (cardValue(newMove->move.card) == BACKWARDS) {
                game->direction = swapDirection(game->direction);

            } else if (cardValue(newMove->move.card) == CONTINUE) {
                //Do Nothing

            } else if (cardValue(newMove->move.card) == DECLARE) {
                //Will override/re-initialise the activeColor determined
                //by the playHandCard function.
                game->activeColor = newMove->move.nextColor;
            }

            // End game if player is out of cards

            linkMoveToPastTurns(game, newMove);
        }
    }
}


// Check the game winner.
//
// Returns NOT_FINISHED if the game has not yet finished,
// 0-3, representing which player has won the game, or
// NO_WINNER if the game has ended but there was no winner.
int gameWinner(Game game) {
    int winner = NOT_FINISHED;
    int i = 0;
    while (i < 4) {
        if (playerCardCount(game, i) == 0) {
            winner = i;
        }
        i++;
    }
    return winner;
}


static CardNode newCardNode (Card val) {
    CardNode newCardNode = calloc(1, sizeof(cardNode));

    if (newCardNode == NULL) {
        err(EXIT_FAILURE, "couldn't allocate space for cardNode.");
    }

    newCardNode->next = NULL;
    newCardNode->card = val;

    return newCardNode;
}

static MoveNode newMoveNode (playerMove val) {
    MoveNode newMoveNode = calloc(1, sizeof(moveNode));

    if (newMoveNode == NULL) {
        err(EXIT_FAILURE, "couldn't allocate space for moveNode.");
    }

    newMoveNode->next = NULL;
    newMoveNode->move = val;

    return newMoveNode;
}

// Return opposite direction
static direction swapDirection(direction toSwap) {
    if (toSwap == CLOCKWISE) {
        return ANTICLOCKWISE;
    } else {
        return CLOCKWISE;
    }
}

// Return the player that should go next
static int cyclePlayer(int player, direction gameDirection) {
    if (gameDirection == CLOCKWISE) {
        // printf("Clockwise\n");
        player++;
    } else {
        // printf("Anticlockwise\n");
        player--;
    }
    player = player % 4;
    if (player < 0) {
        player += 4;
    }

    return player;
}

// Check if the current player has a given card in their hand
static int hasCard(Game game, Card card) {
    int hasIt = FALSE;
    int i = 0;

    while (i < handCardCount(game) && hasIt == FALSE) {
        if (cardsEqual(card, handCard(game, i))) {
            hasIt = TRUE;
        }
        i++;
    }

    return TRUE;
}


// Check if two cards are equal (same value, suit, color)
static int cardsEqual(Card firstCard, Card secondCard) {
    int equal = FALSE;

    if (cardValue(firstCard) == cardValue(secondCard)
        && cardColor(firstCard) == cardColor(secondCard)
        && cardSuit(firstCard) == cardSuit(secondCard)) {
        equal = TRUE;
    }

    return equal;
}

// Check if two cards are similar (one of same value, suit or color)
static int cardsSimilar(Card firstCard, Card secondCard) {
    int similar = TRUE;

    if (cardValue(firstCard) != cardValue(secondCard)
        && cardColor(firstCard) != cardColor(secondCard)
        && cardSuit(firstCard) != cardSuit(secondCard)) {
        similar = FALSE;
    }

    return similar;
}




//Brian's static helper functions:

//Function not allocates new memory for another turn, also also links
//the played move to the head of the new turn, forming a new playerMove
//stack in the new turn tht was just created.

static TurnNode newTurnNode (Game game, MoveNode firstMove) {
    TurnNode newTurnNode = calloc(1, sizeof(turnNode));

    if (newTurnNode == NULL) {
        err(EXIT_FAILURE, "couldn't allocate space for moveNode.");
    }

    newTurnNode->next = NULL;
    newTurnNode->head = firstMove;

    return newTurnNode;
}


//Function takes any move and links the move to the appropriate position
//in the pastTurns stack. If a new turn must be added to the pastTurns 
//stack, function will also allocate new memory to do so by calling the
//helper funtion TurnNode newTurnNode.
static void linkMoveToPastTurns (Game game, MoveNode playedMove) {
    //Case for 1st move of 1st turn.
    //Allocates new memory for 1st turn.
    if (game->pastTurns->head == NULL) {
        TurnNode firstTurn = newTurnNode(game, playedMove);
        game->pastTurns = firstTurn;
    //Cases for when it is not 1st turn.
    } else if (game->pastTurns != NULL) {
        
        TurnNode currTurn = game->pastTurns;
        while (currTurn->next != NULL) {
            //Loops unitl most recent turn.
            //Most recent turns are at the tail of the TurnList stack.
            currTurn = currTurn->next;
        }
        
        MoveNode currMove = currTurn->head;
        while (currMove->next != NULL) {
            //Loops unitl most recent move
            //The most recent move is placed at the tail of the
            //playerMove stack of the current turn.
            currMove = currMove->next;
        }

        if (currMove->move.action != END_TURN) {
            //Case where the played move is not 1st move of the
            //current turn.
            //Links playedMove to tail of playerMove stack of the turn.
            currMove->next = playedMove;
            playedMove->next = NULL;

        } else if (currMove->move.action == END_TURN) {
            //Case where the played move is the 1st move of the turn.
            //Adds new turn to the tail of pastTurns stack.
            TurnNode newTurn = newTurnNode(game, playedMove);
            currTurn->next = newTurn;
        }
    }
}

//Function moves card that is to be played inside the current player's
//playerHands stack and places it at the top of discardPile stack.
static void playHandCard(Game game, playerMove move) {

        int currPlayer = currentPlayer(game);
        CardNode currHandCard = game->playerHands[currPlayer].head;
        CardNode prevHandCard = NULL;

        while (currHandCard->card != move.card) {
            prevHandCard = currHandCard;
            currHandCard = currHandCard->next;
        }

        if (currHandCard->next == NULL) {
            //Case where card to be played is the card at the tail of
            //the playerHands stack.

            if (prevHandCard != NULL) {
                prevHandCard->next = NULL;
            } else {
                game->playerHands[currPlayer].head = NULL;
            }

            currHandCard->next = game->discardPile.head;
            game->discardPile.head = currHandCard;


        } else if (currHandCard->next != NULL) {
            if (currHandCard == game->playerHands[currPlayer].head) {
                game->playerHands[currPlayer].head = currHandCard->next;
            } else {
                prevHandCard->next = currHandCard->next;
            }
            //Case where the card to be played is not the last card of
            //playerHands stack.

            // CardNode lastHead = game->discardPile.head;
            currHandCard->next = game->discardPile.head;
            game->discardPile.head = currHandCard;
            // currHandCard->next = playedCard->next;

        
        }
        // printf("Hand card count %d\n", handCardCount(game));
        game->activeColor = cardColor(move.card);
}

// Give a specified player cards
static void giveCards(Game game, int player, int numCards) {
    int i = 0;

    CardNode currHandCard = game->playerHands[player].head;
    while (currHandCard != NULL
        && currHandCard->next != NULL) {
        currHandCard = currHandCard->next;
    }

    CardNode flipping;

    while (i < numCards) {
        // Replenish deck from discard pile if necessary
        if (game->deckPile.head == NULL) {
            if (game->discardPile.head == NULL
                || game->discardPile.head->next == NULL) {
                // Ran out of cards - end the game
                // TODO: what do we actually do here?
            } else {
                while (game->discardPile.head->next != NULL) {
                    // Get the card we're flipping
                    flipping = game->discardPile.head->next;
                    // Make the discard pile skip over it
                    game->discardPile.head->next = flipping->next;
                    // Point the flipped card to the current decktop
                    flipping->next = game->deckPile.head;
                    // Point the decktop to the flipped card
                    game->deckPile.head = flipping;
                }
            }
        }

        CardNode deckCard = game->deckPile.head;// changed to dot
        game->deckPile.head = deckCard->next;// changed to dot
        deckCard->next = NULL;

        currHandCard->next = deckCard;

        currHandCard = deckCard;
        i++;
    }

}