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

int roulette(vector<double> C, vector<int> select) {
	random_device rd;
	mt19937 gen(rd());
	int selected_item = 0;
	do {
		discrete_distribution<> distrib{ C.begin(), C.end() };
		selected_item = distrib(gen);
	} while (select[selected_item] == 1);
	return selected_item;
}

// оператор формирования начальной популяции 1 (случайным образом без контроля)
vector<vector<int>> PopulationOne(int N, int populationSize) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, 1);

	vector<int> tmpPopulation;
	vector<vector<int>> population;

	for (int i = 0; i < populationSize; i++) {
		for (int j = 0; j < N; j++) {
			tmpPopulation.push_back(dis(gen));
		}
		population.push_back(tmpPopulation);
		tmpPopulation.clear();
	}
	return population;
}

// оператор формирования начальной популяции 2 (случайным образом с контролем)
vector<vector<int>> PopulationTwo(int N, int populationSize, vector<int> weighThings, int Wmax) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, 1);

	int sum = 0;
	vector<int> tmpPopulation;
	vector<vector<int>> population;
	for (int i = 0; i < populationSize; i++) {
		sum = 0;
		for (int j = 0; j < N; j++) {
			tmpPopulation.push_back(dis(gen));
			if (tmpPopulation[j] == 1) {
				sum += weighThings[j];
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
	return population;
}

// оператор формирования начальной популяции 3 (Использует эвристику. Жадный алгоритм)
vector<vector<int>> PopulationThree(int N, int populationSize, vector<int> PricesThings, vector<int> weightThings, int Wmax) {
	vector<vector<int>> population;
	for (int j = 0; j < populationSize; j++) {
		vector <int> S(N, 0); // набор предметов
		vector <double> C; // ценность предмета
		vector <int> W; // вес предметов
		int sumQ = 0; // суммарная ценность всех предметов
		int sumW = 0; // суммарный вес 
		int index = 0;

		for (int i = 0; i < PricesThings.size(); i++) {
			C.push_back(PricesThings[i]);
		}

		for (int i = 0; i < weightThings.size(); i++) {
			W.push_back(weightThings[i]);
		}

		for (int i = 0; i < N; i++) {

			index = roulette(C);
			int sum = sumW + W[index];
			if (sum <= Wmax) {
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
	return population;
}

// выбор родительской пары (случайно (Панмиксия))
void SelectionOfParentOne(int populationSize, vector<vector<int>> population, vector<int>& parentOne, vector<int>& parentTwo) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, populationSize - 1);
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
			prisb += (double)priceThings[i] / (double)weightThings[i];
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
	vector<int> selected(populationSize, 0);

	for (int i = 0; i < populationSize; i++) {
		prices.push_back(Function(N, priceThings, weightThings, population[i]));
	}

	parentItemOne = roulette(prices, selected);
	selected[parentItemOne] = 1;
	parentItemTwo = roulette(prices, selected);

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
	uniform_int_distribution<> dis(1, 2);
	int choiseForChildrenOne = 0;
	int choiseForChildrenTwo = 0;
	for (int i = 0; i < N; i++) {
		choiseForChildrenOne = dis(gen);
		choiseForChildrenTwo = dis(gen);
		if (choiseForChildrenOne == 1) {
			childrenOne[i] = parentOne[i];
		}
		else {
			childrenOne[i] = parentTwo[i];
		}

		if (choiseForChildrenTwo == 1) {
			childrenTwo[i] = parentOne[i];
		}
		else {
			childrenTwo[i] = parentTwo[i];
		}
	}
}

// оператор мутации 1 (генная точечная). Вероятность 10%
void mutationOne(int N, vector<vector<int>>& mutantPopulation, vector<vector<int>> reproductPopulation) {
	int mutation = 0;
	double probability = 0.1;
	random_device rd;
	mt19937 gen(rd());
	for (int i = 0; i < reproductPopulation.size(); i++) {
		uniform_real_distribution<> dis(0.0, 1.0);
		if (dis(gen) <= probability) {
			uniform_int_distribution<> diss(0, reproductPopulation[i].size() - 1);
			mutation = diss(gen);

			if (reproductPopulation[i][mutation] == 0) {
				reproductPopulation[i][mutation] = 1;
			}
			else {
				reproductPopulation[i][mutation] = 0;
			}
			mutantPopulation.push_back(reproductPopulation[i]);
		}
	}
}

// оператор мутации 2 (макромутация инверсией). Вероятность 10%
void mutationTwo(int N, vector<vector<int>>& mutationPopulation, vector<vector<int>> reproductPopulation) {
	int mutationOne = 0;
	int mutationTwo = 0;
	double probability = 0.1;
	random_device rd;
	mt19937 gen(rd());

	for (int i = 0; i < reproductPopulation.size(); i++) {
		uniform_real_distribution<> dis(0.0, 1.0);
		if (dis(gen) <= probability) {
			uniform_int_distribution<> dis(0, reproductPopulation[i].size() - 2);
			mutationOne = dis(gen);
			uniform_int_distribution<> diss(mutationOne, reproductPopulation[i].size() - 1);
			mutationTwo = diss(gen);

			for (int j = mutationOne; j < mutationTwo; j++) {
				if (reproductPopulation[i][j] == 0) {
					reproductPopulation[i][j] = 1;
				}
				else {
					reproductPopulation[i][j] = 0;
				}
			}
			mutationPopulation.push_back(reproductPopulation[i]);
		}
	}
}

// оператор мутации 3 (Хромосомная). Вероятность от 5% до 15%
void mutationThree(int N, vector<vector<int>>& mutationPopulation, vector<vector<int>> reproductPopulation) {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0.05, 0.15);
	double probability = dis(gen);

	for (int i = 0; i < reproductPopulation.size(); i++) {
		uniform_real_distribution<> diss(0.0, 1.0);
		if (diss(gen) <= probability) {
			for (int j = 0; j < reproductPopulation[i].size(); j++) {
				if (reproductPopulation[i][j] == 1) {
					reproductPopulation[i][j] = 0;
				}
				else {
					reproductPopulation[i][j] = 1;
				}
			}
			mutationPopulation.push_back(reproductPopulation[i]);
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
vector<vector<int>> SelectionOne(int newPopulationSize, vector<vector<int>> newPopulation, double g, int N, vector<int> weightThings, vector<int> priceThings) {
	vector<vector<int>> gToNewPopulation;
	random_device rd;
	mt19937 gen(rd());
	int beta = 0;
	vector<vector<int>> tournament;
	for (int i = 0; i < g; i++) {
		uniform_int_distribution<>dis(2, newPopulationSize / 2);
		beta = dis(gen);

		tournament.clear();
		vector<int> selected(newPopulation.size(), 0);
		int k = 0;
		int index = 0;
		do {
			uniform_int_distribution<> diss(0, newPopulation.size() - 1);
			int index = diss(gen);
			if (selected[index] != 1) {
				tournament.push_back(newPopulation[index]);
				selected[index] = 1;
				k++;
			}
		} while (k < beta);

		double maxPrisb = 0;
		vector<int> maxPerson = tournament[0];
		for (int j = 0; j < tournament.size(); j++) {
			double prisb = Function(N, priceThings, weightThings, tournament[j]);
			if (prisb > maxPrisb) {
				maxPrisb = prisb;
				maxPerson = tournament[j];
			}
		}
		gToNewPopulation.push_back(maxPerson);
	}
	return gToNewPopulation;
}

// оператор селекции 2(Пропорциональная)
vector<vector<int>> SelectionTwo(int newPopulationSize, vector<vector<int>> newPopulation, double g, int N,
	vector<int> priceThings, vector<int> weightThings) {

	vector<vector<int>> toNewPopulation;
	vector<double> prices(newPopulationSize, 0);

	for (int i = 0; i < newPopulationSize; i++) {
		prices[i] = Function(N, priceThings, weightThings, newPopulation[i]);
	}

	for (int i = 0; i < g; i++) {
		vector<int> selected(newPopulationSize, 0);
		int index = roulette(prices, selected);
		selected[index] = 1;
		toNewPopulation.push_back(newPopulation[index]);
	}

	return toNewPopulation;
}

// обработка ограничений (модификация генотипа (случайно))
vector<vector<int>> Genotype(int N, vector<vector<int>> population, int Wmax, vector<int> weightThings, vector<int> priceThings) {
	vector<vector<int>> populationCopy;
	populationCopy = population;

	for (int i = 0; i < populationCopy.size(); i++) {
		while (Weight(N, weightThings, populationCopy[i]) > Wmax) {
			int min = 9999;
			int index = 1000;
			for (int j = 0; j < N; j++) {
				if (populationCopy[i][j] == 1 && priceThings[j] < min) {
					min = priceThings[j];
					index = j;
				}
			}
			populationCopy[i][index] = 0;
		}
	}
	return populationCopy;
}


int main() {

	/*
	   цена:
	  15

	  11
	  18
	  23
	  21
	  23
	  30
	  23
	   3
	   4
	   6
	  24
	   2
	  10
	  15
	   6

	   1
	   25
	   29
	   24
	   27
	   21
	   24
	   13
	   16
	   15
	   23
	   14
	   15
	   3
	   16

	   106

	   30
	*/

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
		cin >> PricesThings[i];
	}
	cout << endl;
	cout << "Введите вес вещей: " << endl;
	vector<int> weightThings(N);
	for (int i = 0; i < N; i++) {
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
		population = PopulationTwo(N, populationSize, weightThings, Wmax);
	}
	else if (operatorPopulation == 3) {
		population = PopulationThree(N, populationSize, PricesThings, weightThings, Wmax);
	}

	double solution = 0;
	int step = 1;
	int count = 0;
	while (true) {
		vector<int> maxIndividual(N);
		int index = 0;
		double maxPrisb = 0.0;
		double maxWeight = 0.0;

		cout << endl;
		cout << "Поколение " << step << "." << endl;
		cout << "Особи этого поколения: " << endl;

		for (int i = 0; i < population.size(); i++) {
			double prisbb = Function(N, PricesThings, weightThings, population[i]);
			int weight = Weight(N, weightThings, population[i]);
			if (prisbb > maxPrisb) {
				index = i + 1;
				maxPrisb = prisbb;
				maxWeight = weight;
				maxIndividual = population[i];
			}
			string number = "";

			for (int j = 0; j < population[i].size(); j++) {
				number += to_string(population[i][j]) + " ";
			}

			cout << i + 1 << ". " << number << " с приспособленностью " << prisbb << " и весом " << weight << endl;
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
		vector<vector<int>> reproductPopulationCopy;

		if (operatorMutation == 1) {
			mutationOne(N, reproductPopulationCopy, reproductPopulation);
		}
		else if (operatorMutation == 2) {
			mutationTwo(N, reproductPopulationCopy, reproductPopulation);
		}
		else if (operatorMutation == 3) {
			mutationThree(N, reproductPopulationCopy, reproductPopulation);
		}

		for (const auto& genome : reproductPopulationCopy) {
			reproductPopulation.push_back(genome);
		}


		// обработка исключений
		reproductPopulation = Genotype(N, reproductPopulation, Wmax, weightThings, PricesThings);

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
		double prisbTwo = 0;

		for (int i = 0; i < population.size(); i++) {
			prisbTwo = Function(N, PricesThings, weightThings, population[i]);
			if (prisbTwo > maxPrisb) {
				maxPrisb = prisbTwo;
				maxIndividual = population[i];
			}
		}

		for (int i = 0; i < reproductPopulation.size(); i++) {
			prisbTwo = Function(N, PricesThings, weightThings, reproductPopulation[i]);
			if (prisbTwo > maxPrisb) {
				maxPrisb = prisbTwo;
				maxIndividual = reproductPopulation[i];
			}
		}
		nextPopulation.push_back(maxIndividual);

		vector<vector<int>> gForPopulation;
		index = 0;
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
			tmpNewPopulation = SelectionOne(reproductPopulation.size(), reproductPopulation, g, N, weightThings, PricesThings);
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
			double prisbTwo = 0;
			for (int i = 0; i < nextPopulation.size(); i++) {
				prisbTwo = Function(N, PricesThings, weightThings, nextPopulation[i]);
				if (prisbTwo < minPrisb) {
					minPrisb = prisbTwo;
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
		double prisbThree = 0;
		for (int i = 0; i < population.size(); i++) {
			prisbThree = Function(N, PricesThings, weightThings, nextPopulation[i]);
			if (prisbThree > maxPrisb) {
				maxPrisb = prisbThree;
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
			cout << endl << "Эволюция закончена" << endl;
			cout << "Итоги эволюции:" << endl;
			cout << "Лучшая особь: ";
			for (int i = 0; i < maxIndividual.size(); i++) {
				cout << maxIndividual[i] << " ";
			}
			cout << " c приспособленностью " << maxPrisb << " и весом " << Weight(N, weightThings, maxIndividual) << endl;
			break;
		}
		step++;
		cout << endl;
	}
	return 0;
}