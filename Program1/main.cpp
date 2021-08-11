#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <fstream>
#include <ctype.h>
#include <iomanip>
#include <cmath>

using namespace std;

template<typename T> T convert(char* obj) {
	stringstream temp(obj);
	T result;
	for (int i = 0; i < strlen(obj) && typeid(T) != typeid(string); i++) {
		if (!isdigit(obj[i]) && obj[i] != '-') {
			cout << "Error: check parametrs at: " << obj;
			exit(0);
			return NULL;
		}
	}
	if (!(temp >> result)) {
		exit(0);
		return NULL;
	}
	return result;
}


template<typename T> T getRandomNumber(T min, T max)
{
	random_device rd;
	srand(rd());
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	return static_cast<T>(rand() * fraction * (max - min + 1) + min);
}

void arrayShuffle(float* arr, int size) {
	for (int i = 0; i < size; i++) {
		swap(arr[getRandomNumber(0, size-1)], arr[getRandomNumber(0, size-1)]);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 8) {
		cout << "Error: wrong number of arguments! \nRequired: 7\nCurrent: " << argc - 1;
		return -1;
	}
	string File = convert<string>(argv[1]);
	int CountLines = convert<int>(argv[2]);
	int MinNumbersInLine = convert< int>(argv[3]);
	int MaxNumbersInLine = convert<int>(argv[4]);
	float MaxNumber = convert<float>(argv[5]);
	float MinNumber = convert<float>(argv[6]);
	int PercentWholeNumbers = convert<int>(argv[7]);
	ofstream file;
	file.open(File);
	for (int l = 0; l < CountLines; l++) {
		int countInLine = getRandomNumber<int>(MinNumbersInLine, MaxNumbersInLine);
		float* temp = new float[countInLine];
		for (int i = 0; i < countInLine; i++) {
			
			if (i < ceil(countInLine * PercentWholeNumbers / 100)) {
				temp[i] = getRandomNumber<int>(MinNumber, MaxNumber);
			}
			else {
				temp[i] = getRandomNumber<float>(MinNumber, MaxNumber);
			}
		}
		arrayShuffle(temp, countInLine);
		for (int i = 0; i < countInLine; i++) {
			file << temp[i] << " ";
		}
		if (l+1 < CountLines) {
			file << endl;
		}
	}
	file.close();
}

