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
    uniform_int_distribution<> dis(0, populationSize-1);
    int choiseOne = dis(gen);
    int choiseTwo = 0;
    do {
        choiseTwo = dis(gen);
    } while (choiseTwo == choiseOne);

    parentOne = population[choiseOne];
    parentTwo = population[choiseTwo];
}

// Приспособленность
double Function(int N, vector<int> priceThings, vector<int> weightThings, vector<int> individual) {
    double prisb = 0;
    for (int i = 0; i < N; i++) {
        if (individual[i] == 1) {
            prisb += priceThings[i] / weightThings[i];
        }
    }
    return prisb;
}

// выбор родительской пары (по признакам приспособленности)
void SelectionOfParentTwo(vector<int>& parentOne, vector<int>& parentTwo, int populationSize,
    vector<vector<int>> population, int N, vector<int> priceThings, vector<int> weightThings) {

    int parentItemOne = 0;
    int parentItemTwo = 0;
    vector<double> prices;

    for (int i = 0; i < populationSize; i++) {
        prices.push_back(Function(N, priceThings, weightThings, population[i]));
    }

    parentItemOne = roulette(prices);
    prices.erase(prices.begin() + parentItemOne);
    parentItemTwo = roulette(prices);

    parentOne = population[parentItemOne];
    parentTwo = population[parentItemTwo];
}


// оператор кроссовера 1 (одноточечный)
void CrossoverOne(int N, vector<int> parentOne, vector<int> parentTwo, vector<int>& childrenOne, vector<int>& childrenTwo) {
    int breakPointOne = 0;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, N - 1);
    breakPointOne = dis(gen);

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

// оператор кроссовера 3 (однородный)
void CrossoverThree(int N, vector<int> parentOne, vector<int> parentTwo, vector<int>& childrenOne, vector<int>& childrenTwo) {

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1);
    int choice = 0;

    for (int i = 0; i < N; ++i) {
        choice = dis(gen);
        if (choice == 0) {
            childrenOne.push_back(parentOne[i]);
            childrenTwo.push_back(parentTwo[i]);
        }
        else {
            childrenOne.push_back(parentTwo[i]);
            childrenTwo.push_back(parentOne[i]);
        }
    }
}

// оператор мутации 1 (генная точечная). Вероятность 10%
void mutationOne(int N, vector<vector<int>>& population) {
    int mutation = 0;
    double probability = 0.1;
    random_device rd;
    mt19937 gen(rd());
  
    for (int i = 0; i < population.size(); i++) {

        uniform_real_distribution<> dis(0.0, 1.0);
        if (dis(gen) <= probability) {
            uniform_int_distribution<> diss(0, population[i].size() - 1);
            mutation = diss(gen);

            if (population[i][mutation] == 0) {
                population[i][mutation] = 1;
            }
            else {
                population[i][mutation] = 0;
            }
        }
    }
}

// оператор мутации 2 (макромутация инверсией). Вероятность 10%
void mutationTwo(int N, vector<vector<int>>& population) {
    int mutationOne = 0;
    int mutationTwo = 0;
    double probability = 0.1;
    random_device rd;
    mt19937 gen(rd());

    for (int i = 0; i < population.size(); i++) {
        uniform_real_distribution<> dis(0.0, 1.0);
        if (dis(gen) <= probability) {
            uniform_int_distribution<> dis(0, population[i].size() - 2);
            mutationOne = dis(gen);
            uniform_int_distribution<> diss(mutationOne, population[i].size() - 1);
            mutationTwo = diss(gen);

            for (int j = mutationOne; j < mutationTwo; j++) {
                if (population[i][j] == 0) {
                    population[i][j] = 1;
                }
                else {
                    population[i][j] = 0;
                }
            }
        }
    }
}

// оператор мутации 3 (Хромосомная). Вероятность от 5% до 15%
void mutationThree(int N, vector<vector<int>>& population) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.05, 0.15);
    double probability = dis(gen);

    for (int i = 0; i < population.size(); i++) {
        uniform_real_distribution<> diss(0.0, 1.0);
        if (diss(gen) <= probability) {
            for (int j = 0; j < population[i].size(); j++) {
                if (population[i][j] == 1) {
                    population[i][j] = 0;
                }
                else {
                    population[i][j] = 1;
                }
            }
        }
    }
}

double Weight(int N, vector<int> weightThings, vector<int> individual) {
    double weight = 0;
    for (int i = 0; i < N; i++) {
        if (individual[i] == 1) {
            weight += weightThings[i];
        }
    }
    return weight;
}

// оператор селекции 1(Бета-турнир)
vector<vector<int>> SelectionOne(int newPopulationSize, vector<vector<int>> newPopulation, double g, int N, vector<int> weightThings) {
    vector<vector<int>> toNewPopulation;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<>dis(2, newPopulationSize / 2);
    vector<vector<int>> CopyNewPopulation;
    CopyNewPopulation = newPopulation;
    vector<vector<int>> tournament;
    int count = 0;
    for (int i = 0; i < g; i++) {
        uniform_int_distribution<> diss(0, CopyNewPopulation.size() - 1);
        int beta = dis(gen);
        do {
            int index = diss(gen);
            tournament.push_back(CopyNewPopulation[index]);
            count++;
        } while (count != beta);

        int maxPrisb = 0;
        vector<int> maxPerson = tournament[0];
        for (int j = 0; j < tournament.size(); j++) {
            double prisb = Weight(N, weightThings, tournament[j]);
            if (prisb > maxPrisb) {
                maxPrisb = prisb;
                maxPerson = tournament[j];
            }
        }
        toNewPopulation.push_back(maxPerson);
    }
    return toNewPopulation;
}

// оператор селекции 2(Пропорциональная)
vector<vector<int>> SelectionTwo(int newPopulationSize, vector<vector<int>> newPopulation, double g, int N,
    vector<int> priceThings, vector<int> weightThings) {
    vector<vector<int>> toNewPopulation;
    vector<int> prices;

    for (int i = 0; i < newPopulationSize; i++) {
        prices.push_back(Function(N,priceThings,weightThings,newPopulation[i]));
    }

    for (int i = 0; i < g; i++) {
        int index = roulette(prices);
        toNewPopulation.push_back(newPopulation[index]);
    }

    return toNewPopulation;
}

// стратегия формирования следующего поколения (?)

// формирование следующего поколения


// обработка ограничений (модификация генотипа (случайно))
vector<vector<int>> Genotype(int N, vector<vector<int>> population, int Wmax, vector<int> weightThings) {
    for (int i = 0; i < population.size(); i++) {
        while (Weight(N, weightThings, population[i]) > Wmax){
            int min = 9999;
            int index = 0;
            for (int j = 0; j < population[i].size(); j++) {
                if (population[i][j] == 1 && weightThings[j] < min) {
                    min = weightThings[j];
                    index = j;
                }
            }
            population[i][index] = 0;
        }
    }
    return population;
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
    cout << "1. Генная точечная (10%)" << endl << "2. макромутация инверсией (10%)" << endl << "3. Хромосомная (от 5% до 15%)" << endl;
    do {
        cin >> operatorMutation;
    } while (operatorMutation != 1 && operatorMutation != 2 && operatorMutation != 3);
    cout << endl;

    cout << "Выберите оператор селекции: " << endl;
    cout << "1. Бета турнир" << endl << "2. Пропорциональный" << endl;
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

    int solution = 0;
    int step = 1;
    int count = 0;
    while (true) {
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
            prisb[i] = Function(N, PricesThings, weightThings, population[i]);
            weight[i] = Weight(N, weightThings, population[i]);
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


        // родители - дети
        vector <vector<int>> reproductPopulation; // репродуктивное множество
        for (int i = 0; i < populationSize; i++) {
            vector<int> childOne(N);
            vector<int> childTwo(N);
            vector<int> parentOne(N);
            vector<int> parentTwo(N);

            if (operatorSelectedParents == 1) {
                SelectionOfParentOne(populationSize, population, parentOne, parentTwo);
            }
            else if (operatorSelectedParents == 2) {
                SelectionOfParentTwo(parentOne, parentTwo, populationSize, population, N, PricesThings, weightThings);
            }

            if (operatorСrossover == 1) {
                CrossoverOne(N, parentOne, parentTwo, childOne, childTwo);
            }
            else if (operatorСrossover == 2) {
                CrossoverTwo(N, parentOne, parentTwo, childOne, childTwo);
            }
            else if (operatorСrossover == 3) {
                CrossoverThree(N, parentOne, parentTwo, childOne, childTwo);
            }
            reproductPopulation.push_back(childOne);
            reproductPopulation.push_back(childTwo);
        }

        // мутация
        vector<vector<int>> populationCopy;
        populationCopy = reproductPopulation;

        if (operatorMutation == 1) {
            mutationOne(N, populationCopy);
        }
        else if (operatorMutation == 2) {
            mutationTwo(N, populationCopy);
        }
        else if (operatorMutation == 3) {
            mutationThree(N, populationCopy);
        }

        for (const auto& genome : populationCopy) {
            reproductPopulation.push_back(genome);;
        }

        // обработка исключений
        reproductPopulation = Genotype(N, reproductPopulation, Wmax, weightThings);

        // G - коэффициент перекрытия поколений, G = (0, 1]
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.01, 1);
        double G = dis(gen);
        double g = G * populationSize;

        vector<vector<int>> nextPopulation;

        // Элитарная стратегии формирования популяции
        maxPrisb = 0;
        maxIndividual.clear();
        maxIndividual = population[0];
        vector<double> prisbTwo(reproductPopulation.size());

        for (int i = 0; i < population.size(); i++) {
            prisbTwo[i] = Function(N, PricesThings, weightThings, population[i]);
            if (prisbTwo[i] > maxPrisb) {
                maxPrisb = prisbTwo[i];
                maxIndividual = population[i];
            }
        }

        for (int i = 0; i < reproductPopulation.size(); i++) {
            prisbTwo[i] = Function(N, PricesThings, weightThings, reproductPopulation[i]);
            if (prisbTwo[i] > maxPrisb) {
                maxPrisb = prisbTwo[i];
                maxIndividual = reproductPopulation[i];
            }
        }
        nextPopulation.push_back(maxIndividual);

        vector<vector<int>> gForPopulation;
        index =0;
        random_device rdTwo;
        mt19937 genTwo(rdTwo());
        for (int i = 0; i < g; i++) {
            uniform_int_distribution<> disTwo(0, population.size() - 1);
            index = disTwo(gen);
            gForPopulation.push_back(population[index]);
            population.erase(population.begin() + index);
        }

        for (int i = 0; i < population.size(); i++) {
            nextPopulation.push_back(population[i]);
        }

        vector<vector<int>> tmpNewPopulation;
        if (operatorSelection == 1) {
            tmpNewPopulation = SelectionOne(reproductPopulation.size(), reproductPopulation, g, N, weightThings);
        }
        else if (operatorSelection == 2) {
            tmpNewPopulation = SelectionTwo(reproductPopulation.size(), reproductPopulation, g, N, PricesThings, weightThings);
        }

        for (int i = 0; i < g; i++) {
            nextPopulation.push_back(tmpNewPopulation[i]);
        }

        if (nextPopulation.size() > populationSize) {
            double minPrisb = 0;
            int indexMinIndividual = -1;
            for (int i = 0; i < nextPopulation.size(); i++) {
                double prisb = Function(N, PricesThings, weightThings, nextPopulation[i]);
                if (prisb < minPrisb) {
                    minPrisb = prisb;
                    indexMinIndividual = i;
                }
            }
            if (indexMinIndividual == -1) {
                nextPopulation.erase(nextPopulation.begin());
            }
            else {
                nextPopulation.erase(nextPopulation.begin() + indexMinIndividual);
            }

        }

        population = nextPopulation;

        maxPrisb = 0;
        maxIndividual.clear();
        for (int i = 0; i < population.size(); i++) {
            double prisb = Function(N, PricesThings, weightThings, nextPopulation[i]);
            if (prisb > maxPrisb) {
                maxPrisb = prisb;
                maxIndividual = population[i];
            }
        }

        if (maxPrisb != solution) {
            solution = maxPrisb;
            count = 0;
        }
        else {
            count += 1;
        }

        if (count >= 10) {
            cout << "Эволюция закончена" << endl;
            cout << "Итоги эволюции:" << endl;
            cout << "Лучшая особь: ";
            for (int i = 0; i < maxIndividual.size(); i++) {
                cout << maxIndividual[i] << " ";
            }
            cout << " c приспособленностью " << maxPrisb << " и весом " << Weight(N, weightThings, maxIndividual);
            break;
        }
        step++;
        cout << endl;
    }
	return 0;
}