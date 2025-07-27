#include <iostream>
#include <math.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;

vector<string> cityNames;
vector<double> x, y;
int n;

double distance(int x1, int y1, int x2, int y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

struct Individual
{
    vector<int> genes;
    double result = 0;

    void findResult()
    {
        result = 0;
        for (int i = 0; i < genes.size() - 1; i++)
        {
            int a = genes[i];
            int b = genes[i + 1];
            result += distance(x[a], y[a], x[b], y[b]);
        }
    }

    friend bool operator>(Individual& i1, Individual& i2)
    {
        return i1.result > i2.result;
    }
};

struct Compare
{
    bool operator()(Individual& lhs, Individual& rhs)
    {
        return lhs > rhs;
    }
};

priority_queue<Individual, vector<Individual>, Compare> q;
priority_queue<Individual, vector<Individual>, Compare> nextQ;

Individual findBest()
{
    return q.top();
}

void printIndividual(Individual individual) {
    cout << "Current best result: " << individual.result << endl;
    cout << "Path: ";
    for (int i = 0; i < individual.genes.size(); i++) {
        cout << cityNames[individual.genes[i]];
        if (i < individual.genes.size() - 1) {
            cout << " -> ";
        }
    }
    cout << endl;
}

void mutate(Individual& i)
{
    int randGene1 = rand() % n;
    int randGene2 = rand() % n;

    swap(i.genes[randGene1], i.genes[randGene2]);
}

void fillRestOfChildWithParent(Individual& child, Individual& parent, int stopper)
{
    bool found;
    int iter = 0;
    int nextGen;

    for (int i = stopper + 1; i < n; i++) {
        found = true;
        while (found)
        {
            nextGen = parent.genes[iter];
            found = false;
            for (int j = 0; j < child.genes.size(); j++)
            {
                if (child.genes[j] == nextGen)
                {
                    found = true;
                    iter++;
                    break;
                }
            }
        }
        child.genes.push_back(nextGen);
    }
}

void cross(Individual p1, Individual p2) {
    Individual c1, c2;

    int stopper = rand() % n;

    for (int i = 0; i <= stopper; i++) {
        c1.genes.push_back(p1.genes[i]);
        c2.genes.push_back(p2.genes[i]);
    }

    fillRestOfChildWithParent(c1, p2, stopper);
    fillRestOfChildWithParent(c2, p1, stopper);

    mutate(c1);
    mutate(c2);

    c1.findResult();
    c2.findResult();

    nextQ.push(c1);
    nextQ.push(c2);
}

void reproduce()
{
    Individual i1, i2;
    int initSize = q.size();

    while (q.size() > initSize / 2)
    {
        i1 = q.top();
        q.pop();
        i2 = q.top();
        q.pop();
        cross(i1, i2);

        nextQ.push(i1);
        nextQ.push(i2);
    }

    while (!q.empty()) {
        q.pop();
    }
}

void readCityData()
{
    ifstream nameFile("uk12_name.csv");
    ifstream coordFile("uk12_xy.csv");

    string line;
    string city;
    double xCoord, yCoord;


    while (getline(nameFile, line)) {
        stringstream ss(line);
        while (getline(ss, city, ',')) {
            cityNames.push_back(city);
        }
    }

    while (getline(coordFile, line)) {
        stringstream ss(line);
        ss >> xCoord;
        ss.ignore(1, ',');
        ss >> yCoord; 
        x.push_back(xCoord);
        y.push_back(yCoord);
    }

    n = cityNames.size();
}

void initPopulation(vector<Individual>& parents, int numberPop)
{
    for (int i = 0; i < numberPop; i++)
    {
        Individual individ;
        for (int j = 0; j < n; j++)
        {
            individ.genes.push_back(j);
        }
        random_shuffle(individ.genes.begin(), individ.genes.end());
        individ.findResult();
        q.push(individ);
    }
}


void initNextGen() {
    q = nextQ;

    while (!nextQ.empty())
    {
        nextQ.pop();
    }
}

int main()
{
    readCityData();

    vector<Individual> parents;
    int numberPop = 10;
    initPopulation(parents, numberPop);

    int iter = 0;
    Individual best;

    while (iter <= 5000) {
        if (iter == 10 || iter == 100 || iter == 300 || iter == 600 || iter == 800 || iter == 1000 || iter == 5000)
        {
            cout << iter << ": ";
            best = findBest();
            printIndividual(best);
        }

        reproduce();
        initNextGen();
        iter++;
    }

    cout << "\nBest path after " << iter << " iterations: ";
    best = findBest();
    printIndividual(best);

    return 0;
}
