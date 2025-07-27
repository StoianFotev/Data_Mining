#include <iostream>
#include <vector>

using namespace std;

char board[3][3] = {
    {'-', '-', '-'},
    {'-', '-', '-'},
    {'-', '-', '-'}
};

const char emptyCell = '-';
const int maxScore = 10;
const int minScore = -10;
bool isPersonTurn;
char computerSymbol;
char personSymbol;
const int invalidPosition = -1;
int movesAvailable = 9;

void printBoard() {
    cout << "-------------\n";
    for (int i = 0; i < 3; ++i) {
        cout << "| ";
        for (int j = 0; j < 3; ++j) {
            cout << board[i][j] << " | ";
        }
        cout << "\n-------------\n";
    }
}

bool areThereMovesLeft() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == emptyCell) {
                return true;
            }
        }
    }
    return false;
}

int evaluateBoard(int depth) {
    for (int row = 0; row < 3; ++row) {
        if (board[row][0] == board[row][1] && board[row][1] == board[row][2]) {
            if (board[row][0] == computerSymbol) {
                return maxScore - depth;
            }
            else if (board[row][0] == personSymbol) {
                return minScore + depth;
            }
        }
    }

    for (int col = 0; col < 3; ++col) {
        if (board[0][col] == board[1][col] && board[1][col] == board[2][col]) {
            if (board[0][col] == computerSymbol) {
                return maxScore - depth;
            }
            else if (board[0][col] == personSymbol) {
                return minScore + depth;
            }
        }
    }

    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        if (board[0][0] == computerSymbol) {
            return maxScore - depth;
        }
        else if (board[0][0] == personSymbol) {
            return minScore + depth;
        }
    }

    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        if (board[0][2] == computerSymbol) {
            return maxScore - depth;
        }
        else if (board[0][2] == personSymbol) {
            return minScore + depth;
        }
    }

    return 0;
}

int minimizer(int alpha, int beta, int depth);

int maximizer(int alpha, int beta, int depth) {
    int curScore = evaluateBoard(depth);

    if (curScore != 0) {
        return curScore;
    }

    if (!areThereMovesLeft()) {
        return 0;
    }

    int bestScore = numeric_limits<int>::min();

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == emptyCell) {
                board[i][j] = computerSymbol;

                bestScore = max(bestScore, minimizer(alpha, beta, depth + 1));

                board[i][j] = emptyCell;

                if (bestScore >= beta) {
                    return bestScore;
                }
                alpha = max(alpha, bestScore);
            }
        }
    }
    return bestScore;
}

int minimizer(int alpha, int beta, int depth) {
    int curScore = evaluateBoard(depth);

    if (curScore != 0) {
        return curScore;
    }

    if (!areThereMovesLeft()) {
        return 0;
    }

    int bestScore = numeric_limits<int>::max();

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == emptyCell) {
                board[i][j] = personSymbol;

                bestScore = min(bestScore, maximizer(alpha, beta, depth + 1));

                board[i][j] = emptyCell;

                if (bestScore <= alpha) {
                    return bestScore;
                }
                beta = min(beta, bestScore);
            }
        }
    }
    return bestScore;
}

pair<int, int> findBestTurnForComputer() {
    int bestValue = numeric_limits<int>::min();
    pair<int, int> bestNextTurn = { invalidPosition, invalidPosition };

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == emptyCell) {
                board[i][j] = computerSymbol;

                int curTurnValue = minimizer(numeric_limits<int>::min(), numeric_limits<int>::max(), 0);

                board[i][j] = emptyCell;

                if (curTurnValue > bestValue) {
                    bestNextTurn = { i, j };
                    bestValue = curTurnValue;
                }
            }
        }
    }
    return bestNextTurn;
}

bool isThereWinner() {
    return evaluateBoard(0) != 0;
}

bool makeTurn(int i, int j, bool isPersonTurn) {
    if (i < 0 || j < 0 || i >= 3 || j >= 3) {
        cout << "There are no such coordinates\n";
        printBoard();
        return false;
    }
    if (board[i][j] == emptyCell) {
        board[i][j] = isPersonTurn ? personSymbol : computerSymbol;
        movesAvailable--;

        cout << "==========\n";
        printBoard();

        if (isThereWinner()) {
            if (isPersonTurn) {
                cout << "Congratulations, you are the winner!\n";
            }
            else {
                cout << "Sorry, you lost!\n";
            }
        }
        return true;
    }
    cout << "This cell is not empty! Choose another.\n";
    return false;
}

int main() {
    printBoard();

    cout << "Do you want to start the game? Enter 1 for first and 0 for second: ";
    cin >> isPersonTurn;

    if (isPersonTurn) {
        computerSymbol = 'o';
        personSymbol = 'x';
    }
    else {
        computerSymbol = 'x';
        personSymbol = 'o';
    }

    while (movesAvailable > 0 && !isThereWinner()) {
        if (isPersonTurn) {
            int i, j;
            do {
                cout << "Row: ";
                cin >> i;
                cout << "Column: ";
                cin >> j;
            } while (!makeTurn(i - 1, j - 1, isPersonTurn));
            isPersonTurn = !isPersonTurn;
        }
        else {
            auto bestTurn = findBestTurnForComputer();
            makeTurn(bestTurn.first, bestTurn.second, isPersonTurn);
            isPersonTurn = !isPersonTurn;
        }
    }

    if (!isThereWinner() && movesAvailable == 0) {
        cout << "The game ends in a draw!\n";
    }

    return 0;
}
