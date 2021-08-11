#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>

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

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cout << "Error: wrong arguments!";
		return 0;
	}
	
	ifstream reader;
	reader.open(argv[1]);
	if (!reader.is_open()) {
		cout << "Error: File not found or file open!";
	}
	
	int countLines = 0;
	DynamicArray<string> arr;
	string buffer;
	while (!reader.eof()) {
		getline(reader,buffer);
		arr.add(buffer);
		countLines++;
	}
	for (int i = 0; i < arr.getSize(); i++) {
		cout << arr.get(i) << endl;
	}
	
}

