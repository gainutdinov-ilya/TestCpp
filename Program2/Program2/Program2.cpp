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
	
	ifstream reader;
	reader.open(argv[1]);
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
	
	for (int j = 0; j < formantedArray.getSize(); j++) {
		for (int i = 0; i < formantedArray.get(j).getSize(); i++) {
			cout << formantedArray.get(j).get(i) << " ";
		}
		cout << endl;
	}

}

