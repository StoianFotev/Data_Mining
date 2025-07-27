#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <cmath>
#include <chrono>
#include <iomanip>

using namespace std;

const int FOUND = -10;
vector<int> tiles;
vector<string> path;
int sizeOfBoard;
int goalIndexOfZero;
int curIndexOfZero;
unordered_set<string> visitedStates;

int posOfZero() {
    for (int i = 0; i < tiles.size(); i++) {
        if (tiles[i] == 0) return i;
    }
    return -1;
}

bool isSolvable() {
    int inversions = 0;
    for (int i = 0; i < tiles.size(); i++) {
        if (tiles[i] == 0) continue;
        for (int j = i + 1; j < tiles.size(); j++) {
            if (tiles[j] != 0 && tiles[i] > tiles[j]) inversions++;
        }
    }
    int zeroRow = curIndexOfZero / sizeOfBoard;
    if (sizeOfBoard % 2 == 1) {
        return inversions % 2 == 0;
    }
    else {
        return (inversions + zeroRow) % 2 == 1;
    }
}

int manhattan() {
    int sum = 0;
    for (int i = 0; i < tiles.size(); i++) {
        int cur = tiles[i];
        if (cur == 0) continue;
        int goalRow;
        int goalCol;
        int curRow = i / sizeOfBoard;
        int curCol = i % sizeOfBoard;
        if (goalIndexOfZero < cur && goalIndexOfZero != -1) {
            goalRow = (cur) / sizeOfBoard;
            goalCol = (cur) % sizeOfBoard;
        }
        else {
            goalRow = (cur - 1) / sizeOfBoard;
            goalCol = (cur - 1) % sizeOfBoard;
        }
        sum += abs(goalRow - curRow) + abs(goalCol - curCol);
    }
    return sum;
}

bool isGoal() {
    return manhattan() == 0;
}

inline void swapTiles(int posA, int posB) {
    swap(tiles[posA], tiles[posB]);
}

bool moveUp() {
    if (curIndexOfZero / sizeOfBoard == sizeOfBoard - 1) return false;
    swapTiles(curIndexOfZero, curIndexOfZero + sizeOfBoard);
    curIndexOfZero += sizeOfBoard;
    return true;
}

bool moveDown() {
    if (curIndexOfZero / sizeOfBoard == 0) return false;
    swapTiles(curIndexOfZero, curIndexOfZero - sizeOfBoard);
    curIndexOfZero -= sizeOfBoard;
    return true;
}

bool moveRight() {
    if (curIndexOfZero % sizeOfBoard == 0) return false;
    swapTiles(curIndexOfZero, curIndexOfZero - 1);
    curIndexOfZero -= 1;
    return true;
}

bool moveLeft() {
    if (curIndexOfZero % sizeOfBoard == sizeOfBoard - 1) return false;
    swapTiles(curIndexOfZero, curIndexOfZero + 1);
    curIndexOfZero += 1;
    return true;
}

int search(int g, int threshold) {
    int f = g + manhattan();
    if (f > threshold) return f;
    if (isGoal()) {
        cout << path.size() - 1 << endl;
        for (int i = 1; i < path.size(); i++) {
            cout << path[i] << endl;
        }
        return FOUND;
    }

    int min = INT_MAX;

    if (path.back() != "up" && moveDown()) {
        path.push_back("down");
        int temp = search(g + 1, threshold);
        if (temp == FOUND) return FOUND;
        min = (temp < min) ? temp : min;
        path.pop_back();
        moveUp();
    }
    if (path.back() != "down" && moveUp()) {
        path.push_back("up");
        int temp = search(g + 1, threshold);
        if (temp == FOUND) return FOUND;
        min = (temp < min) ? temp : min;
        path.pop_back();
        moveDown();
    }
    if (path.back() != "left" && moveRight()) {
        path.push_back("right");
        int temp = search(g + 1, threshold);
        if (temp == FOUND) return FOUND;
        min = (temp < min) ? temp : min;
        path.pop_back();
        moveLeft();
    }
    if (path.back() != "right" && moveLeft()) {
        path.push_back("left");
        int temp = search(g + 1, threshold);
        if (temp == FOUND) return FOUND;
        min = (temp < min) ? temp : min;
        path.pop_back();
        moveRight();
    }

    return min;
}

void idastar() {
    int threshold = manhattan();
    path.push_back("init");
    while (true) {
        int temp = search(0, threshold);
        if (temp == FOUND) break;
        threshold = temp;
    }
}

int main() {
    int n, index;
    cin >> n >> index;
    int tile;

    for (int i = 0; i <= n; i++) {
        cin >> tile;
        tiles.push_back(tile);
    }

    sizeOfBoard = sqrt(n + 1);
    goalIndexOfZero = (index == -1) ? sizeOfBoard * sizeOfBoard - 1 : index;
    curIndexOfZero = posOfZero();

    if (!isSolvable()) {
        cout << -1 << endl;
        return 0;
    }

    auto start = chrono::steady_clock::now();
    idastar();
    auto end = chrono::steady_clock::now();
    chrono::duration<double> elapsed = end - start;
    //cout << "Time: "<< setprecision(2) << elapsed.count() << "s" << endl;

    return 0;
}
