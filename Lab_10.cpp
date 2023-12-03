#include <iostream>
#include <locale.h>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>
#include "Struct.h"
#include <string>
using namespace std;

// рулетка
template<typename T>
T roulette(vector<T> C) {
	random_device rd;
	mt19937 gen(rd());
	discrete_distribution<> distrib{ C.begin(), C.end() };

	int selected_item = distrib(gen);
	return selected_item;
}

// оператор формирования начальной популяции 1 (случайным образом без контроля)
vector<vector<int>> PopulationOne(int N, int populationSize) {

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1);

    // Формируем начальную популяцию
    vector<int> tmpPopulation;
    vector<vector<int>> population;
    for (int i = 0; i < populationSize; i++) {
        for (int j = 0; j < N; j++) {
            tmpPopulation.push_back(dis(gen));
        }
        population.push_back(tmpPopulation);
        tmpPopulation.clear();
    }

    // Выводим начальную популяцию
    for (const auto& genome : population) {
        for (const auto& gene : genome) {
            cout << gene << " ";
        }
        cout << endl;
    }
    return population;
}

// оператор формирования начальной популяции 2 (случайным образом с контролем)
vector<vector<int>> PopulationTwo(int N, int populationSize, vector<int> PricesThings, int Wmax) {

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1);

    // Формируем начальную популяцию
    int sum = 0;
    vector<int> tmpPopulation;
    vector<vector<int>> population;
    for (int i = 0; i < populationSize; i++) {
        sum = 0;
        for (int j = 0; j < N; j++) {
            tmpPopulation.push_back(dis(gen));
            if (tmpPopulation[j] == 1) {
                sum += PricesThings[j];
                if (sum >= Wmax) {
                    for (int k = j + 1; k < N; k++) {
                        tmpPopulation.push_back(0);
                    }
                    tmpPopulation[j] = 0;
                    break;
                }
            }
        }
        population.push_back(tmpPopulation);
        tmpPopulation.clear();
    }

    // Выводим начальную популяцию
    for (const auto& genome : population) {
        for (const auto& gene : genome) {
            cout << gene << " ";
        }
        cout << endl;
    }

    return population;
}

// оператор формирования начальной популяции 3 (Использует эвристику. Жадный алгоритм)
vector<vector<int>> PopulationThree(int N, int populationSize, vector<int> PricesThings, vector<int> weightThings,  int Wmax) {
    vector<vector<int>> population;
    for (int j = 0; j < populationSize; j++) {
        vector <int> S(N, 0); // набор предметов
        vector <int> C; // ценность предмета
        vector <int> W; // вес предметов
        int sumQ = 0; // суммарная ценность всех предметов
        int sumW = 0; // суммарный вес 
        int index = 0;

        for (int i = 0; i < PricesThings.size(); i++) {
            C.push_back(PricesThings[i]);
        }

        for (int i = 0; i < weightThings.size(); i++) {
            W.push_back(PricesThings[i]);
        }

        for (int i = 0; i < N; i++) {

                index = roulette(C);
                if (sumW + W[index] <= Wmax) {
                    sumQ += C[index];
                    sumW += W[index];
                    S[index] = 1;
                }
                else {
                    break;
                }
                C.erase(C.begin() + index);
        }
        population.push_back(S);
    }
   

    // Выводим начальную популяцию
    for (const auto& genome : population) {
        for (const auto& gene : genome) {
            cout << gene << " ";
        }
        cout << endl;
    }

    return population;
}

// выбор родительской пары (случайно (Панмиксия))
void SelectionOfParentOne(int populationSize, vector<vector<int>> population, vector<int>& parentOne, vector<int>& parentTwo) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, populationSize);
    int choiseOne = dis(gen);
    int choiseTwo = 0;
    do {
        choiseTwo = dis(gen);
    } while (choiseTwo == choiseOne);

    parentOne = population[choiseOne];
    parentTwo = population[choiseTwo];
}

// Приспособленность
void Function(int N, vector<int> priceThings, vector<int> weightThings, vector<int> individual, vector<double>& prisb) {
    for (int i = 0; i < N; i++) {
        if (individual[i] == 1) {
            prisb[i] = priceThings[i] / weightThings[i];
        }
    }
}

// выбор родительской пары (по признакам приспособленности)
void SelectionOfParentTwo(vector<int>& parentOne, vector<int>& parentTwo, int populationSize,
    vector<vector<int>> population, int N, vector<int> priceThings, vector<int> weightThings) {

    int parentItemOne = 0;
    int parentItemTwo = 0;
    vector<double> prices;

    /*for (int i = 0; i < populationSize; i++) {
        prices.push_back(Function(N, priceThings, weightThings, population[i])); // исправить 
    }*/

    parentItemOne = roulette(prices);
    prices.erase(prices.begin() + parentItemOne);
    parentItemTwo = roulette(prices);

    parentOne = population[parentItemOne];
    parentTwo = population[parentItemTwo];
}

// стратегия формирования следующего поколения (?)

// формирование следующего поколения

// оператор кроссовера 1 (одноточечный)
void CrossoverOne(int N, vector<int> parentOne, vector<int> parentTwo, vector<int>& childrenOne, vector<int>& childrenTwo) {
    int breakPointOne = 0;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, N - 1);
    if (breakPointOne == 0) {
        breakPointOne = dis(gen);
    }

    for (int i = 0; i < breakPointOne; i++) {
        childrenOne[i] = parentOne[i];
        childrenTwo[i] = parentTwo[i];
    }
    for (int i = breakPointOne; i < N; i++) {
        childrenOne[i] = parentTwo[i];
        childrenTwo[i] = parentOne[i];
    }
}

// оператор кроссовера 2 (двуточечный)
void CrossoverTwo(int N, vector<int> parentOne, vector<int> parentTwo, vector<int>& childrenOne, vector<int>& childrenTwo) {
    int breakPointOne = 0;
    int breakPointTwo = 0;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, N / 2);
    uniform_int_distribution<> diss(N / 2 + 1, N - 1);
    breakPointOne = dis(gen);
    breakPointTwo = diss(gen);
    for (int i = 0; i < breakPointOne; i++) {
        childrenOne[i] = parentTwo[i];
        childrenTwo[i] = parentOne[i];
    }
    for (int i = breakPointOne; i < breakPointTwo; i++) {
        childrenOne[i] = parentOne[i];
        childrenTwo[i] = parentTwo[i];
    }
    for (int i = breakPointTwo; i < N; i++) {
        childrenOne[i] = parentTwo[i];
        childrenTwo[i] = parentOne[i];
    }
}

// оператор кроссовера 3 (однородный (c вероятностью 50%))
void CrossoverThree(int N, vector<int> parentOne, vector<int> parentTwo, vector<int>& childrenOne, vector<int>& childrenTwo) {
    double p = 0.5;
    vector<double> weights = { p, 1.0 - p };
    for (int i = 0; i < N; ++i) {
        int choice = roulette(weights);
        if (choice == 0) { // Применяем кроссовер с вероятностью p
            childrenOne.push_back(parentOne[i]);
            childrenTwo.push_back(parentTwo[i]);
        }
        else {
            childrenOne.push_back(parentTwo[i]);
            childrenTwo.push_back(parentOne[i]);
        }
    }
}

// оператор мутации 1 (генная точечная)
// добавить вероятность мутации
vector<int> mutationOne(int N, vector<int> genChildren) {
    int mutation = 0;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, genChildren.size()-1);
    mutation = dis(gen);
    if (genChildren[mutation] == 0) {
        genChildren[mutation] = 1;
    }
    else {
        genChildren[mutation] = 0;
    }
    return genChildren;
}

// оператор мутации 2 (макромутация инверсией)
// добавить вероятность мутации
vector<int> mutationTwo(int N, vector<int> genChildren) {
    int mutationOne = 0;
    int mutationTwo = 0;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, genChildren.size()-2);
    mutationOne = dis(gen);
    uniform_int_distribution<> diss(mutationOne, genChildren.size() - 1);
    mutationTwo = diss(gen);
    for (int i = mutationOne; i < mutationTwo; i++) {
        if (genChildren[i] == 0) {
            genChildren[i] = 1;
        }
        else {
            genChildren[i] = 0;
        }
    }
    return genChildren;
}

// оператор селекции 1

// оператор селекции 2

// обработка ограничений (?)


void Weight(int N, vector<int> weightThings, vector<int> individual, vector<double>& weight) {
    for (int i = 0; i < N; i++) {
        if (individual[i] == 1) {
            weight[i] = weightThings[i];
        }
    }
}


int main() {
	setlocale(LC_ALL, "rus");
	int N = 0;
	int Wmax = 0;
    int populationSize = 0;

	cout << "Введите N: ";
	cin >> N;
	cout << endl;

    cout << "Введите ценность вещей: " << endl;
    vector<int> PricesThings(N);
    for (int i = 0; i < N; i++) {
        int tmp = 0;
        cin >> PricesThings[i];
    }
    cout << endl;

    cout << "Введите вес вещей: " << endl;
    vector<int> weightThings(N);
    for (int i = 0; i < N; i++) {
        int tmp = 0;
        cin >> weightThings[i];
    }
    cout << endl;

	cout << "Введите весовое ограничение: ";
    cin >> Wmax;
    cout << endl;

    cout << "Введите количество особей в популяции: ";
    cin >> populationSize;
    cout << endl;


    int operatorPopulation = 0;
    int operatorСrossover = 0;
    int operatorMutation = 0;
    int operatorSelection = 0;
    int operatorSelectedParents = 0;
    cout << "Выберите оператор формирования начальной популяции: " << endl;
    cout << "1. Случайный" << endl << "2. Случайный с контролем" << endl << "3. Жадный алгоритм" << endl;
    do {
        cin >> operatorPopulation;
    } while (operatorPopulation != 1 && operatorPopulation != 2 && operatorPopulation != 3);
    cout << endl;

    cout << "Выберите оператор выбора родительской пары: " << endl;
    cout << "1. Случайный" << endl << "2. По признаку приспособленности" << endl;
    do {
        cin >> operatorSelectedParents;
    } while (operatorSelectedParents != 1 && operatorSelectedParents != 2);
    cout << endl;

    cout << "Выберите оператор кроссовер: " << endl;
    cout << "1. Одноточечный" << endl << "2. Двуточечный" << endl << "3. Однородный" << endl;
    do {
        cin >> operatorСrossover;
    } while (operatorСrossover != 1 && operatorСrossover != 2 && operatorСrossover != 3);
    cout << endl;

    cout << "Выберите оператор мутации: " << endl;
    cout << "1. Генная точечная" << endl << "2. макромутация инверсией" << endl;
    do {
        cin >> operatorMutation;
    } while (operatorMutation != 1 && operatorMutation != 2);
    cout << endl;

    cout << "Выберите оператор селекции: " << endl;
    cout << "1. Бета турнир" << endl << "2. Пропорциональная" << endl;
    do {
        cin >> operatorSelection;
    } while (operatorSelection != 1 && operatorSelection != 2);
    cout << endl;

    vector<vector<int>> population;
    if (operatorPopulation == 1) {
        population = PopulationOne(N, populationSize);
    }
    else if (operatorPopulation == 2) {
        population = PopulationTwo(N, populationSize, PricesThings, Wmax);
    }
    else if (operatorPopulation == 3) {
        population = PopulationThree(N, populationSize, PricesThings, weightThings,  Wmax);
    }

    int step = 1;
    vector<int> maxIndividual(N);
    int index = 0;
    double maxPrisb = 0.0f;
    double maxWeight = 0.0f;

    cout << endl;
    cout << "Поколение " << step << "." << endl;
    cout << "Особи этого поколения: " << endl;
    vector<double> prisb(N);
    vector<double> weight(N);

    for (int i = 0; i < population.size(); i++)
    {
        Function(N, PricesThings, weightThings, population[i], prisb);
        Weight(N, weightThings, population[i], weight);
    }

    for (int i = 0; i < population.size(); i++) {

        if (prisb[i] > maxPrisb) {
            index = i + 1;
            maxPrisb = prisb[i];
            maxWeight = weight[i];
            maxIndividual = population[i];
        }
        string number = "";

        for (int j = 0; j < population[i].size(); j++) {
            number += to_string(population[i][j]) + " ";
        }

        cout << i + 1 << ". " << number << " с приспособленностью " << prisb[i] << " и весом " << weight[i] << endl;
    }

    string number = "";

    for (int j = 0; j < maxIndividual.size(); j++) {
        number += to_string(maxIndividual[j]) + " ";
    }
    cout << endl << "Лучшая особь из поколения: " << endl;
    cout << index << ". " << number << " с приспособленностью " << maxPrisb << " и весом " << maxWeight << endl;

    vector<int> parentOne(N);
    vector<int> parentTwo(N);
    if (operatorSelectedParents == 1) {
        SelectionOfParentOne(populationSize, population, parentOne, parentTwo);
    }
    else if (operatorSelectedParents == 2) {
        SelectionOfParentTwo(parentOne, parentTwo, populationSize, population, N,PricesThings, weightThings);
    }
    cout << "Родители выбраны" << endl;

    vector<int> childOne(N);
    vector<int> childTwo(N);
    if(operatorСrossover == 1) {
        CrossoverOne(N, parentOne, parentTwo, childOne, childTwo);
    }
    else if(operatorСrossover == 2) {
        CrossoverTwo(N, parentOne, parentTwo, childOne, childTwo);
    }
    else if (operatorСrossover == 3) {
        CrossoverThree(N, parentOne, parentTwo, childOne, childTwo);
    }
    cout << "Дети выбраны" << endl;
	return 0;
}