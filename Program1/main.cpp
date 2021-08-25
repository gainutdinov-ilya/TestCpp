#include <iostream>
#include <cmath>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <random>
#include "zlib.h"

#define CHUNK_SIZE 1024

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
		if (afterComma(number) > 6) {
			return 3;
		}
		return 2;
	}
	return 1;
}

bool compressFile(ifstream& src, ofstream& dst,int COMPRESSION_LEVEL)
{
	uint8_t inbuff[CHUNK_SIZE];
	uint8_t outbuff[CHUNK_SIZE];
	z_stream stream = { 0 };
	if (deflateInit(&stream, COMPRESSION_LEVEL) != Z_OK)
	{
		fprintf(stderr, "Failed!\n");
		return false;
	}
	int flush;
	do {
		src.read((char*)&inbuff, CHUNK_SIZE);
		stream.avail_in = sizeof(inbuff);
		flush = src.eof() ? Z_FINISH : Z_NO_FLUSH;
		stream.next_in = inbuff;
		do {
			stream.avail_out = CHUNK_SIZE;
			stream.next_out = outbuff;
			deflate(&stream, flush);
			uint32_t nbytes = CHUNK_SIZE - stream.avail_out;
			dst.write((char*)&outbuff, nbytes);
		} while (stream.avail_out == 0);
	} while (flush != Z_FINISH);
	deflateEnd(&stream);
	return true;
}

int fileType(string filename) {
	if (filename.substr(filename.find_last_of(".") + 1) == "txt") {
		return 1;
	}
	else if (filename.substr(filename.find_last_of(".") + 1) == "txtz") {
		return 2;
	}
	else {
		return -1;
	}
}

int main(int argc, char *argv[]) {
	if (argc > 9) {//проверка кол-ва аргументов командной строки
		cout << "Error: wrong number of arguments! \nRequired: 7 or 8\nCurrent: " << argc - 1;
		return -1;
	}
	//создание переменных для генерации
	bool allowDouble = false;
	string File = convert<string>(argv[1]);
	int CountLines = convert<int>(argv[2]);
	int MinNumbersInLine = convert< int>(argv[3]);
	int MaxNumbersInLine = convert<int>(argv[4]);
	double maxGeneratedNum = convert<double>(argv[6]);
	double minGeneratedNum = convert<double>(argv[5]);
	int PercentWholeNumbers = convert<int>(argv[7]);
	int compressLevel = -1;

	if (getType(argv[6]) == 3 || getType(argv[5]) == 3) {//проверка в необходимости диапазона double
		allowDouble = true;
	}

	cout << "Allow double: " << (allowDouble ? "True" : "False") << endl;

	if (fileType(File) == -1) {
		cout << "Wrong argument: type of file not supported!";
		return -1;
	}
	else if (fileType(File) == 2 && argc != 9) {
		cout << "Wrong argument: no compression raito!";
		return -1;
	}

	if (argc == 9 && fileType(File) == 2) {
		if (convert<string>(argv[8]) == "-c0") {
			compressLevel = 0;
		}
		else if (convert<string>(argv[8]) == "-c1") {
			compressLevel = 1;
		}
		else if (convert<string>(argv[8]) == "-c2") {
			compressLevel = 9;
		}
		else {
			cout << "Wrong argument: compress level not allowed!" << endl;
			return -1;
		}
	}

	stringstream r;
	r << getRandomNumber<int>(1,99999999);
	string tempFileRand;
	r >> tempFileRand;
	stringstream k(File + tempFileRand + ".temp");
	string tempFile;
	k >> tempFile;

	ofstream file;
	if (compressLevel >= 0) {
		file.open(tempFile);
	}
	else {
		file.open(File);
	}
	
	bool generatingFlag = true;
	cout << "Creating file..\n";
	double startTime = clock();
	for (int l = 0; l < CountLines; l++) {
		double startLine = clock();
		cout << "\rGenerating line " << l + 1 << " of " << CountLines << " || " << setprecision(3) << (float)(clock() - startTime) / 1000 <<" sec.." << fixed;
		int countInLine = getRandomNumber<int>(MinNumbersInLine, MaxNumbersInLine);
		bool* numbers = new bool[countInLine];
		for (int i = 0; i < countInLine; i++) {
			if (i < ceil(countInLine * PercentWholeNumbers / 100)) numbers[i] = 0;
			else numbers[i] = 1;
		}
		arrayShuffle(numbers, countInLine);
		for (int i = 0; i < countInLine; i++) {
			if (!numbers[i]) file << setprecision(0) << getRandomNumber<int>(minGeneratedNum, maxGeneratedNum) << " ";
			else {
				if (getRandomNumber(1,100) > 50 && allowDouble) {
					file << setprecision(16) << getRandomNumber<double>(minGeneratedNum, maxGeneratedNum) << " ";
				}
				else file << setprecision(6) << getRandomNumber<float>(minGeneratedNum, maxGeneratedNum) << " ";
			}
		}
		if (l + 1 < CountLines) {
			file << endl;
		}
	}
	file.close();

	cout << "OK" << endl;

	if (compressLevel >= 0) {
		cout << "Compressing file..";
		ifstream temp;
		temp.open(tempFile, ios::binary);
		ofstream out; 
		out.open(File, ios::binary);
		bool result = compressFile(temp, out, compressLevel);
		temp.close();
		out.close();
		remove(tempFile.c_str());
		if (result) {
			cout << "OK" << endl;
		}
	}
}