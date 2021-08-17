#include <stdlib.h>
#include <iostream>
#include <cstring>
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
		if (!isdigit(obj[i]) && obj[i] != '-' && obj[i] != '.') {
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

template<typename T>void arrayShuffle(T* arr, int size) {
	for (int i = 0; i < size; i++) {
		swap(arr[getRandomNumber(0, size-1)], arr[getRandomNumber(0, size-1)]);
	}
}

bool isTrueFloat(char* obj) {
	if (convert<float>(obj) < convert<double>(obj)) return true;
	return false;
}

int afterComma(string num) {
	stringstream ss;
	ss << setprecision(15) << num;
	string strNum = ss.str();
	size_t pos = strNum.find('.');
	if (pos != strNum.npos) {
		return strNum.size() - 1 - pos;
	}
	else {
		return 0;
	}
}

//используется для определения типа числа
int getType(string number) {
	if (afterComma(number) > 0) {
		if (afterComma(number) > 7) {
			return 3;
		}
		return 2;
	}
	return 1;
}

int main(int argc, char *argv[]) {
	if (argc != 8) {
		cout << "Error: wrong number of arguments! \nRequired: 7\nCurrent: " << argc - 1;
		return -1;
	}
	bool allowDouble = false;
	string File = convert<string>(argv[1]);
	int CountLines = convert<int>(argv[2]);
	int MinNumbersInLine = convert< int>(argv[3]);
	int MaxNumbersInLine = convert<int>(argv[4]);
	double MaxNumber = convert<double>(argv[6]);
	double MinNumber = convert<double>(argv[5]);
	int PercentWholeNumbers = convert<int>(argv[7]);


	if (getType(argv[6]) == 3 || getType(argv[5]) == 3) {
		allowDouble = true;
	}
	cout << "Allow double = " << allowDouble << endl;
	ofstream file;
	file.open(File);
	for (int l = 0; l < CountLines; l++) {
		int countInLine = getRandomNumber<int>(MinNumbersInLine, MaxNumbersInLine);
		bool* numbers = new bool[countInLine];
		for (int i = 0; i < countInLine; i++) {
			if (i < ceil(countInLine * PercentWholeNumbers / 100)) numbers[i] = 0;
			else numbers[i] = 1;
		}
		arrayShuffle(numbers, countInLine);
		for (int i = 0; i < countInLine; i++) {
			if (!numbers[i]) file << setprecision(0) << getRandomNumber<int>(MinNumber, MaxNumber) << " ";
			else {
				if (getRandomNumber(1,100) > 50 && allowDouble) {
					file << setprecision(16) << getRandomNumber<double>(MinNumber, MaxNumber) << " ";
					
				}
				else file << setprecision(5) << getRandomNumber<float>(MinNumber, MaxNumber) << " ";
			}
		}
		if (l + 1 < CountLines) {
			file << endl;
		}
	}
	file.close();
}

