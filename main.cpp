#include <iostream>
#include <random>
#include <algorithm>
#include <limits>
using std::cout, std::cin, std::endl;

static int* newDeck() {
    //make deck
    int* deck = new int[52];
    for (int i = 0; i < 52; i++) {
        if (i >= 40) {
            deck[i] = 10;
        }
        else {
            deck[i] = i / 4 + 1;
        }
    }
    //shuffle
    std::random_device rand;
    std::mt19937 rng(rand());
    std::shuffle(deck, deck + 52, rng);

    return deck;
}
static void printDeck(int* deck) {
    for (int i = 0; i < 52; i++) {
        cout << deck[i] << " ";
    }
    cout << endl;
}
static int getPlayerNum() {
    int numPlayers;
    while (true) {
        cout << "Enter number of Players (1-5 - excluding dealer): ";
        cin >> numPlayers;

        if (cin.fail()) { //not number
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a numeric value." << endl;
            continue;
        }
        else if (numPlayers < 1 || numPlayers > 5) { //out of range
            cout << "Value out of range. Please enter a value between 1 and 5." << endl;
        }
        else {
            return numPlayers + 1; //add one for dealer
        }
    }
}
static int** dealCards(int* deck, int numPlayers) {

    int** playerHands = new int* [numPlayers];

    for (int i = 0; i < numPlayers; i++) {
        playerHands[i] = new int[5];
        playerHands[i][0] = deck[i * 2];
        playerHands[i][1] = deck[i * 2 + 1];
        playerHands[i][2] = 0;
        playerHands[i][3] = 0;
        playerHands[i][4] = 0;
    }
    deck += numPlayers * 2; //point to next card in deck
    return playerHands;
}
static void viewCards(int** playerHands, int currentPlayer, int numPlayers, bool viewAll) {
    for (int i = 0; i < numPlayers; i++) {
        if (i == (numPlayers - 1)) {
            cout << "Dealer: ";
        }
        else if (i == currentPlayer) {
            cout << "You: ";
        }
        else {
            cout << "Player" << i+1 << ": ";
        }
        for (int j = 0; j < 5; j++) {
            if (playerHands[i][j] == 0) {
                break;
            }
            if (j == 0 && i != currentPlayer && !viewAll) {
                cout << "? ";
            }
            else {
                cout << playerHands[i][j] << " ";
            }
        }
        cout << endl;
    }
}
static bool getMove() { //true for hit and false for stand
    char move;

    while (true) {
        cout << "Would you like to hit (h) or stand (s)? ";
        cin >> move;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input. Try again." << endl;
            continue;
        }
        if (move == 'h' || move == 'H') {
            return true;
        }
        else if (move == 's' || move == 'S') {
            return false;
        }
        else {
            cout << "Enter \"h\" to hit or \"s\" to stand." << endl;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}
static bool manageMove(int* myCards, int* deck) { //false for bust or stand, true otherwise
    if (!getMove()) return false; //for stand

    int sum = 0;
    for (int i = 0; i < 5; i++) {
        if (myCards[i] == 0) { //first empty slot in hand
            myCards[i] = deck[0]; //hit
            deck++; //increment to next in deck
            sum += myCards[i];
            break;
        }
        sum += myCards[i];
    }
    return !(sum > 21 || myCards[4] != 0);
}
static bool manageMoveDealer(int* myCards, int* deck) {
    int sum = 0;
    bool hasAce = false;
    for (int i = 0; i < 5; i++) {
        if (myCards[i] == 1) hasAce = true;
        else if (myCards[i] == 0) { //first empty slot in hand
            if ((!hasAce && sum < 17) //normal: hit on 16 and under
            || (hasAce && sum < 7) //ace as 11: hit on under 7
            || (hasAce && sum > 11 && sum < 17)){ //ace as 1: hit on over 11 but under 17
                myCards[i] = deck[0]; //hit
                deck++; //increment to next in deck
            }
            sum += myCards[i];
            break;
        }
        sum += myCards[i];
    }
    return !(sum > 21 || myCards[4] != 0);
}
static int getWinner(int** playerHands, int numPlayers) {
    int sum;
    bool hasAce;
    int bestScore = 0;
    int bestPlayer = -1;
    for (int i = 0; i < numPlayers; i++) {
        sum = 0;
        hasAce = false;
        for (int j = 0; j < 5; j++) {
            if (playerHands[i][j] == 1) {
                hasAce = true;
            }
            sum += playerHands[i][j];
        }
        if (hasAce && sum < 12) {
            sum += 10;
        }
        if (sum < 22 && sum >= bestScore) {
            bestScore = sum;
            bestPlayer = i;
        }
    }
    return bestPlayer;
}
static bool playAgain() {
    char choice;
    while (true) {
        cout << "Would you like to play again (y/n)? ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input. Try again." << endl;
            continue;
        }
        if (choice == 'y' || choice == 'Y') {
            return true;
        }
        else if (choice == 'n' || choice == 'N') {
            return false;
        }
    }
}
int main() {
    //init
    int numPlayers = getPlayerNum(); //ask user for num of players
    bool* canPlay = new bool[numPlayers];
    for (int i = 0; i < numPlayers; i++) {
        canPlay[i] = true;
    }
    int* deck = newDeck(); //generate shuffled deck
    int** playerHands = dealCards(deck, numPlayers); //give each player 2 cards
    //gameloop
    for (int _ = 0; _ < 5; _++) {
        for (int i = 0; i < numPlayers; i++) {
            if (!canPlay[i]) {
                continue;
            }
            if (i == (numPlayers - 1)) { //dealer turn
                cout << "Dealer turn." << endl;
                viewCards(playerHands, -1, numPlayers, false);
                canPlay[i] = manageMoveDealer(playerHands[i], deck);
            }
            else { //player turn
                cout << "Player" << i+1 << "'s turn." << endl;
                viewCards(playerHands, i, numPlayers, false);
                canPlay[i] = manageMove(playerHands[i], deck);
            }
        }
        cout << endl;
    }
    //display game over
    cout << "\n" << "Game Over!" << "----------" << endl;
    viewCards(playerHands, -1, numPlayers, true);
    int winner = getWinner(playerHands, numPlayers);
    if (winner == numPlayers - 1) {
        cout << "Dealer Wins!" << endl;
    }
    else if (winner == -1) {
        cout << "Everybody busted!" << endl;
    }
    else {
        cout << "Player" << winner+1 << " Wins!" << endl;
    }
    //clean up
    for (int i = 0; i < numPlayers; i++) {
        delete[] playerHands[i];
    }
    delete[] playerHands;
    delete[] deck;
    delete[] canPlay;
    //restart game
    if (playAgain()) main();

    return 0;
}