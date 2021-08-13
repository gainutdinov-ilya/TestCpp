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

bool isFloat(string obj) {
	if (fabs(convert<float>(obj)) > fabs(convert<int>(obj))) return true;
	return false;
}

bool isDouble(string obj) {
	if (fabs(convert<double>(obj)) > fabs(convert<float>(obj))) return true;
	return false;
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		cout << "Error: wrong arguments!";
		return 0;
	}

	string filename = argv[1];
	string line;
	ifstream reader(filename);
	ofstream writer(filename + ".bin", ios::binary);
	if (!reader.is_open()) {
		cout << "Error: File not found or file open!";
		return 0;
	}
	while (!reader.eof()) {
		getline(reader, line);
		stringstream temp;
		string num;
		temp << line;
		DynamicArray<string> buff;
		while (temp >> num) {
			buff.add(num);
		}
		
		__int8 size = buff.getSize();
		writer.write((char*)&size, sizeof(size));
		for (int i = 0; i < buff.getSize(); i++) {
			if (isFloat(buff.get(i)) || isDouble(buff.get(i))) {
				if (isDouble(buff.get(i))) {
					cout << "double: " << buff.get(i) << endl;
					double toWrite = convert<double>(buff.get(i));
					__int8 type = 3;
					writer.write((char*)&type, sizeof(type));
					writer.write((char*)&toWrite, sizeof(toWrite));
				}
				else {
					cout << "float: " << buff.get(i) << endl;
					float toWrite = convert<float>(buff.get(i));
					__int8 type = 2;
					writer.write((char*)&type, sizeof(type));
					writer.write((char*)&toWrite, sizeof(toWrite));
				}
			}
			else {
				cout << "int: " << buff.get(i) << endl;
				int toWrite = convert<int>(buff.get(i));
				__int8 type = 1;
				writer.write((char*)&type, sizeof(type));
				writer.write((char*)&toWrite, sizeof(toWrite));
			}

		}
		
	}
}