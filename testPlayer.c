// Test the final card down ADT
// Created by:
//  Elizabeth Willer (z5161594) <z5161594@student.unsw.edu.au>
//  Mitchell Roberts (z5117664) <z5117664@student.unsw.edu.au>
// Created on 2017-09-22
// Alex Linker (F11A-Kora)
//Checking which version I am changing

#include "player.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    printf("Welcome to the tests!\n");
    color cs[800];
    suit ss[800];
    value vs[800];
    int numCards = 800;

    // Fill arrays appropriately for a deck with same number of each card
    // For now, assume numCards is multiple of possible combinations
    int i = 0;
    int j = 0;
    // Note: we need to count the 0th suit, value, color.
    int duplicates = numCards / ((QUESTIONS + 1) * (F + 1) * (PURPLE + 1));

    // Loop through ss (note enums are essentially ints)
    int nextSuit = 0;
    // Loop through cs
    int nextColor = 0;
    // Loop through vs
    int nextValue = 0;

    // TODO Add shuffling
    while (nextSuit <= QUESTIONS) {
        nextColor = 0;
        while (nextColor <= PURPLE) {
            nextValue = 0;
            while (nextValue <= F) {
                j = 0;
                while (j < duplicates) {
                    vs[i] = nextValue;
                    cs[i] = nextColor;
                    ss[i] = nextSuit;
                    i++;
                    j++;
                }
                nextValue++;
            }
            nextColor++;
        }
        nextSuit++;
    }

    Game game = newGame(800, vs, cs, ss);
    playerMove move = decideMove(game);
    playMove(game, move);
    printf("All tests passed.!\n");
    return EXIT_SUCCESS;
}

