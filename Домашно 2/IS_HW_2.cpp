#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;

int n;
vector<int> queens;
vector<int> rowConflicts;
vector<int> diag1Conflicts;
vector<int> diag2Conflicts;

void placeInitialQueens() {
    for (int col = 0; col < n; col++) {
        queens[col] = rand() % n;
        rowConflicts[queens[col]]++;
        diag1Conflicts[col - queens[col] + n - 1]++;
        diag2Conflicts[col + queens[col]]++;
    }
}

int findColumnWithMaxConflicts() {
    int maxConflicts = -1;
    vector<int> columnsWithMaxConflicts;
    for (int col = 0; col < n; col++) {
        int queenRow = queens[col];
        int conflicts = rowConflicts[queenRow] + diag1Conflicts[col - queenRow + n - 1] + diag2Conflicts[col + queenRow] - 3;

        if (conflicts > maxConflicts) {
            maxConflicts = conflicts;
            columnsWithMaxConflicts = { col };
        }
        else if (conflicts == maxConflicts) {
            columnsWithMaxConflicts.push_back(col);
        }
    }
    if (maxConflicts == 0) {
        return -1;
    }
    return columnsWithMaxConflicts[rand() % columnsWithMaxConflicts.size()];
}

int findRowWithMinConflicts(int col) {
    int minConflicts = n + 1;
    vector<int> rowsWithMinConflicts;
    for (int row = 0; row < n; row++) {
        int conflicts = rowConflicts[row] + diag1Conflicts[col - row + n - 1] + diag2Conflicts[col + row];

        if (queens[col] == row) {
            conflicts -= 3;
        }

        if (conflicts < minConflicts) {
            minConflicts = conflicts;
            rowsWithMinConflicts = { row };
        }
        else if (conflicts == minConflicts) {
            rowsWithMinConflicts.push_back(row);
        }
    }

    return rowsWithMinConflicts[rand() % rowsWithMinConflicts.size()];
}

void updateState(int col, int newRow) {
    int oldRow = queens[col];
    rowConflicts[oldRow]--;
    diag1Conflicts[col - oldRow + n - 1]--;
    diag2Conflicts[col + oldRow]--;

    queens[col] = newRow;
    rowConflicts[newRow]++;
    diag1Conflicts[col - newRow + n - 1]++;
    diag2Conflicts[col + newRow]++;
}

void printBoard() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (queens[j] == i) {
                cout << "* ";
            }
            else {
                cout << "_ ";
            }
        }
        cout << endl;
    }
}

void printQueens() {
    cout << "[";
    for (int i = 0; i < n; i++) {
        cout << queens[i];
        if (i != n - 1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}

void solve() {
    bool hasConflicts = true;
    int iterations = 0;
    while (hasConflicts && iterations < n * n) {
        int col = findColumnWithMaxConflicts();
        if (col == -1) {
            hasConflicts = false;
            break;
        }

        int row = findRowWithMinConflicts(col);
        updateState(col, row);
        iterations++;
    }

    if (hasConflicts) {
        cout << -1 << endl;
    }
    else {
        printQueens();
    }
}

int main() {
    srand(time(0));

    cin >> n;

    queens.resize(n);
    rowConflicts.resize(n, 0);
    diag1Conflicts.resize(2 * n - 1, 0);
    diag2Conflicts.resize(2 * n - 1, 0);

    placeInitialQueens();

    clock_t start = clock();
    solve();
    clock_t end = clock();
    //cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << " seconds" << endl;
    return 0;
}
