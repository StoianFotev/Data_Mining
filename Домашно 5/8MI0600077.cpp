#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <random>

using namespace std;

vector<string> dataset;
vector<string> trainSet, testSet;
int democrats[3][16] = { 0 };
int republicans[3][16] = { 0 };
double numDem = 0, numRep = 0;
int testsize;
const int initPosRep = 11;
const int initPosDem = 9;
double lambda = 1.0;

void updateTables(string line, int diff) {
    bool isRep = line[0] == 'r';
    isRep ? numRep += diff : numDem += diff;
    int initPos = isRep ? initPosRep : initPosDem;
    int counter = 0;
    for (int i = initPos; i < line.size(); i += 2) {
        switch (line[i]) {
        case 'y':
            isRep ? republicans[0][counter] += diff : democrats[0][counter] += diff;
            break;
        case 'n':
            isRep ? republicans[1][counter] += diff : democrats[1][counter] += diff;
            break;
        case '?':
            isRep ? republicans[2][counter] += diff : democrats[2][counter] += diff;
            break;
        }
        counter++;
    }
}

void fillMissingValues() {
    for (auto& line : dataset) {
        for (int i = initPosDem; i < line.size(); i += 2) {
            if (line[i] == '?') {
                int counter = (i - initPosDem) / 2;
                int demMax = max({ democrats[0][counter], democrats[1][counter], democrats[2][counter] });
                int repMax = max({ republicans[0][counter], republicans[1][counter], republicans[2][counter] });
                if (demMax >= repMax) {
                    if (democrats[0][counter] == demMax) line[i] = 'y';
                    else if (democrats[1][counter] == demMax) line[i] = 'n';
                    else line[i] = '?';
                }
                else {
                    if (republicans[0][counter] == repMax) line[i] = 'y';
                    else if (republicans[1][counter] == repMax) line[i] = 'n';
                    else line[i] = '?';
                }
            }
        }
    }
}

int processTestLine(string line) {
    bool isRep = line[0] == 'r';
    int initPos = isRep ? initPosRep : initPosDem;
    double probRepLog = 0;
    double probDemLog = 0;

    int indexAttribute = 0;
    for (int i = initPos; i < line.size(); i += 2) {
        switch (line[i]) {
        case 'y':
            probRepLog += log((republicans[0][indexAttribute] + lambda) / (numRep + 3 * lambda));
            probDemLog += log((democrats[0][indexAttribute] + lambda) / (numDem + 3 * lambda));
            break;
        case 'n':
            probRepLog += log((republicans[1][indexAttribute] + lambda) / (numRep + 3 * lambda));
            probDemLog += log((democrats[1][indexAttribute] + lambda) / (numDem + 3 * lambda));
            break;
        case '?':
            probRepLog += log((republicans[2][indexAttribute] + lambda) / (numRep + 3 * lambda));
            probDemLog += log((democrats[2][indexAttribute] + lambda) / (numDem + 3 * lambda));
            break;
        }
        indexAttribute++;
    }
    probRepLog += log((numRep + lambda) / (numRep + numDem + 2 * lambda));
    probDemLog += log((numDem + lambda) / (numRep + numDem + 2 * lambda));
    return isRep ? (probRepLog >= probDemLog) : (probRepLog <= probDemLog);
}

double calculateAccuracy(vector<string>& testSet) {
    double rightPredictions = 0;
    for (auto& line : testSet) {
        rightPredictions += processTestLine(line);
    }
    return (rightPredictions / testSet.size()) * 100;
}

void splitData() {
    random_device rd;
    mt19937 g(rd());
    shuffle(dataset.begin(), dataset.end(), g);
    int trainSize = dataset.size() * 0.8;
    trainSet.assign(dataset.begin(), dataset.begin() + trainSize);
    testSet.assign(dataset.begin() + trainSize, dataset.end());
    for (auto& line : trainSet) {
        updateTables(line, 1);
    }
}

int main() {
    ifstream filein("house-votes-84.data", ios::in);
    if (!filein) {
        cerr << "Error: Could not open file!" << endl;
        return 1;
    }
    string line;
    while (getline(filein, line)) {
        dataset.push_back(line);
    }
    filein.close();
    cout << "Enter option (0 - '?' as abstain, 1 - fill missing values): ";
    int option;
    cin >> option;
    if (option == 1) {
        fillMissingValues();
    }
    splitData();
    double trainAccuracy = calculateAccuracy(trainSet);
    cout << "Train Set Accuracy: " << trainAccuracy << "%" << endl;
    int foldSize = trainSet.size() / 10;
    double avgAccuracy = 0.0, stdDeviation = 0.0;
    vector<double> accuracies;
    for (int i = 0; i < 10; ++i) {
        vector<string> validationSet(trainSet.begin() + i * foldSize, trainSet.begin() + (i + 1) * foldSize);
        vector<string> trainingSet = trainSet;
        trainingSet.erase(trainingSet.begin() + i * foldSize, trainingSet.begin() + (i + 1) * foldSize);
        fill(democrats[0], democrats[0] + 48, 0);
        fill(republicans[0], republicans[0] + 48, 0);
        numDem = numRep = 0;
        for (auto& line : trainingSet) {
            updateTables(line, 1);
        }
        double accuracy = calculateAccuracy(validationSet);
        accuracies.push_back(accuracy);
        avgAccuracy += accuracy;
    }
    avgAccuracy /= 10;
    for (double acc : accuracies) {
        stdDeviation += pow(acc - avgAccuracy, 2);
    }
    stdDeviation = sqrt(stdDeviation / 10);
    cout << "10-Fold Cross-Validation Results:" << endl;
    for (int i = 0; i < 10; i++) {
        cout << "Accuracy Fold " << i + 1 << ": " << accuracies[i] << "%" << endl;
    }
    cout << "Average Accuracy: " << avgAccuracy << "%" << endl;
    cout << "Standard Deviation: " << stdDeviation << "%" << endl;
    double testAccuracy = calculateAccuracy(testSet);
    cout << "Test Set Accuracy: " << testAccuracy << "%" << endl;
    return 0;
}
