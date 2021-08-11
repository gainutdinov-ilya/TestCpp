#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

template<class T>
class DynamicArray {
private:
	int size = 0;
	T* arr = new T[size];
public:
	void add(T obj) {
		int oldSize = size;
		T* temp = new T[size];
		swap(temp, arr);
		size++;
		arr = new T[size];
		for (int i = 0; i < oldSize; i++) {
			arr[i] = temp[i];
		}
		arr[oldSize] = obj;
	}
	T get(int id) {
		return arr[id];
	}
	int getSize() {
		return size;
	}
};

template<typename T> T convert(string obj) {
	stringstream temp(obj);
	T result;
	if (!(temp >> result)) {
		exit(0);
		return NULL;
	}
	return result;
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		cout << "Error: wrong arguments!";
		return 0;
	}
	string filename = argv[1];
	ifstream reader;
	reader.open(filename);
	if (!reader.is_open()) {
		cout << "Error: File not found or file open!";
	}
	
	int countLines = 0;
	DynamicArray<string> unformatedArray;
	DynamicArray<DynamicArray<float>> formantedArray;
	string buffer;
	while (!reader.eof()) {
		getline(reader,buffer);
		unformatedArray.add(buffer);
		countLines++;
	}
	for (int i = 0; i < unformatedArray.getSize(); i++) {
		stringstream tempString;
		string tempWord = "";
		tempString << unformatedArray.get(i);
		DynamicArray<float> tempArray;
		while (tempString >> tempWord) {
			tempArray.add(convert<float>(tempWord));
		}
		formantedArray.add(tempArray);
	}
	reader.close();

	ofstream bin;
	bin.open(filename+".bin", ios::binary);
	int size = formantedArray.getSize();
	bin.write((char*)&size, sizeof(size));
	for (int j = 0; j < formantedArray.getSize(); j++) {
		size = formantedArray.get(j).getSize();
		bin.write((char*)&size, sizeof(size));
		for (int i = 0; i < formantedArray.get(j).getSize(); i++) {
			float toWrite = formantedArray.get(j).get(i);
			bin.write((char*)&toWrite, sizeof(toWrite));
		}
	}
	bin.close();
	/*
	ifstream binReader;
	binReader.open("output.bin", ios::binary);
	int totalCount;
	binReader.read((char*)&totalCount, sizeof(totalCount));
	for (int j = 0; j < totalCount; j++) {
		int countInLine;
		binReader.read((char*)&countInLine, sizeof(countInLine));
		for (int i = 0; i < countInLine; i++) {
			float temp;
			binReader.read((char*)&temp, sizeof(temp));
			cout << temp << " ";
		}
		cout << endl;
	}
	*/
}

