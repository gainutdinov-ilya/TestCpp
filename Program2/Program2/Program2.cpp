#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;


//класс динамического массива
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

//функция для конверации строки в требуемый тип
template<typename T> T convert(string obj) {
	stringstream temp(obj);
	T result;
	if (!(temp >> result)) {
		exit(0);
		return NULL;
	}
	return result;
}

bool isTrueFloat(string obj) {
	if (convert<float>(obj) > convert<int>(obj)) return true;
	return false;
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

	DynamicArray<DynamicArray<float>> formantedArray;

	string line, num;

	while (!reader.eof()) {
		getline(reader, line);
		stringstream buffer;
		buffer << line;
		DynamicArray<float> tempArray;
		while (buffer >> num) {
			tempArray.add(convert<float>(num));
		}
		formantedArray.add(tempArray);
	}
	reader.close();

	int lineSize;

	ofstream writer;
	string objString;
	bool isFloat;
	float float_;
	int int_;
	writer.open(filename + ".bin", ios::binary);
	for (int j = 0; j < formantedArray.getSize(); j++) {
		lineSize = formantedArray.get(j).getSize();
		writer.write((char*)&lineSize, sizeof(lineSize));
		for (int i = 0; i < lineSize; i++) {
			stringstream temp;
			temp << formantedArray.get(j).get(i);
			temp >> objString;
			isFloat = formantedArray.get(j).get(i);
			if (isFloat) {
				writer.write((char*)&isFloat, sizeof(isFloat));
				float_ = convert<float>(objString);
				writer.write((char*)&float_, sizeof(float_));
			}
			else {
				writer.write((char*)&isFloat, sizeof(isFloat));
				int_ = convert<int>(objString);
				writer.write((char*)&float_, sizeof(int_));
			}
		}
	}
	writer.close();
/*
	int inLine;
	bool type;


	reader.open(filename + ".bin", ios::binary);
	while (reader.read((char*)&inLine, sizeof(inLine))) {
		for (int i = 0; i < inLine; i++) {
			reader.read((char*)&type, sizeof(type));
			if (type) {
				reader.read((char*)&float_, sizeof(float_));
				cout << float_ << " ";
			}
			else {
				reader.read((char*)&int_, sizeof(int_));
				cout << int_ << " ";
			}
		}
		cout << endl;
	}
*/
}