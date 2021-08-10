#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <fstream>
#include <ctype.h>

using namespace std;

template<typename T> T Convert(char* obj) {
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

int main(int argc, char *argv[]) {
	if (argc != 8) {
		cout << "Error: wrong number of arguments! \nRequired: 7\nCurrent: " << argc - 1;
		return -1;
	}
	string File = Convert<string>(argv[1]);
	int CountLines = Convert<int>(argv[2]);
	int MinNumbersInLine = Convert< int>(argv[3]);
	int MaxNumbersInLine = Convert<int>(argv[4]);
	float MaxNumber = Convert<float>(argv[5]);
	float MinNumber = Convert<float>(argv[6]);
	int PercentWholeNumbers = Convert<int>(argv[7]);
	
	ofstream file;
	file.open(File);
	for (int l = 0; l < CountLines; l++) {
		int countInLine = getRandomNumber<int>(MinNumbersInLine, MaxNumbersInLine);
		for (int i = 0; i < countInLine; i++) {
			
			if (i < ceil(countInLine * PercentWholeNumbers / 100)) {
				file << getRandomNumber<int>(MinNumber, MaxNumber) << " ";
			}
			else {
				file << getRandomNumber<float>(MinNumber, MaxNumber) << " ";
			}
		}
		if (l+1 < CountLines) {
			file << endl;
		}
	}
	file.close();
}

