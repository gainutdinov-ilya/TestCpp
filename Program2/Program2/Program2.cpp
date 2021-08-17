#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <bitset>

using namespace std;

//класс динамического массива
template<class T>
class DynamicArray {
private:
	size_t size = 0;
	T* arr = new T[size];
public:
	void add(T obj) {
		int oldSize = size;
		T* temp = new T[size];
		swap(temp, arr);
		if(size != 0) delete[] arr;
		size++;
		arr = new T[size];
		for (int i = 0; i < oldSize; i++) {
			arr[i] = temp[i];
		}
		delete[] temp;
		arr[oldSize] = obj;
		
	}
	T get(int id) {
		return arr[id];
	}
	size_t getSize() {
		return size;
	}
};

//клас для шаблонизации блоков и маркеров
class BlockElement {
public:
	int type;
	string element;
	void setType(int t) {
		type = t;
	}
	void setElement(string e) {
		element = e;
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

//используется для определения типа числа
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

//записывает кол-во блоков в записи
void writeLineSize(ofstream& writer, int data) {
	if (data > 255) {
		int temp = 255;
		bitset<8> lineSize1(temp);
		writer.write((char*)&lineSize1, sizeof(lineSize1));
		//cout << "\nwrite " << temp << endl;
		temp = data - 255;
		bitset<8> lineSize2(temp);
		//cout << "write " << lineSize2.to_ulong() << endl;
		writer.write((char*)&lineSize2, sizeof(lineSize2));
	}
	else {
		bitset<8> lineSize(data);
		writer.write((char*)&lineSize, sizeof(lineSize));
	}

}

//записывает информацию о блоке
void writeBlockInfo(ofstream& writer, int size, int type) {
	bitset<8> blockSize(size);
	bitset<4> blockType(type);
	blockSize = blockSize << 4;
	for (int i = 0; i < 4; i++) {
		blockSize[i] = blockType[i];
	}
	writer.write((char*)&blockSize, sizeof(blockSize));
}

void writeNum(ofstream& writer, string number, int type) {
	int int_;
	double double_;
	float float_;
	switch (type)
	{
	case 3:
		double_ = convert<double>(number);
		writer.write((char*)&double_, sizeof(double_));
		break;
	case 2:
		float_ = convert<float>(number);
		writer.write((char*)&float_, sizeof(float_));
		break;
	default:
		int_ = convert<int>(number);
		writer.write((char*)&int_, sizeof(int_));
		break;
	}
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

	if (!reader.is_open()) {
		cout << "Error: File not found or file open!";
		return 0;
	}
	DynamicArray<DynamicArray<BlockElement>> unformatedArray;
	while (!reader.eof()) {
		getline(reader, line);
		DynamicArray<string> arrLine;
		stringstream temp;
		string num;
		temp << line;
		DynamicArray<BlockElement> tempArray;
		while (temp >> num) {
			int type = getType(num);
			BlockElement element;
			element.setElement(num);
			element.setType(type);
			tempArray.add(element);
		}
		unformatedArray.add(tempArray);
	}
	reader.close();


	DynamicArray<DynamicArray<DynamicArray<BlockElement>>> formatedArray;
	for (int i = 0; i < unformatedArray.getSize();	i++) {
		int countBlocks = 0;
		int last_type = 0;
		DynamicArray<DynamicArray<BlockElement>> line;
		DynamicArray<BlockElement> block;
		for (int k = 0; k < unformatedArray.get(i).getSize(); k ++) {
			int type = unformatedArray.get(i).get(k).type;
			BlockElement element;
			element.setElement(unformatedArray.get(i).get(k).element);
			element.setType(type);
			
			if (block.getSize() == 15) {
				line.add(block);
				DynamicArray<BlockElement> new_;
				block = new_;
				block.add(element);
			}
			
			if (k + 1 ==  unformatedArray.get(i).getSize()) {
				if (last_type == type) {
					block.add(element);
					line.add(block);
					break;
				}
				else {
					line.add(block);
					DynamicArray<BlockElement> new_;
					block = new_;
					block.add(element);
					line.add(block);
					break;
				}
			}
			if (last_type == type) {
				block.add(element);
			}
			else {
				line.add(block);
				DynamicArray<BlockElement> new_;
				block = new_;
				block.add(element);
			}

			last_type = type;
		}
		formatedArray.add(line);
	}

	ofstream writer(filename + ".bin", ios::binary);
	for (int l = 0; l < formatedArray.getSize(); l++) {
		cout << "Line: "<< l + 1 << "  BLOCKS: " << formatedArray.get(l).getSize()-1 << " ";
		writeLineSize(writer, formatedArray.get(l).getSize() - 1);
		for (int f = 0; f < formatedArray.get(l).getSize(); f++) {
			for (int k = 0; k < formatedArray.get(l).get(f).getSize(); k++) {
				if (k == 0) {
					cout << "\n\t{ B SIZE:" << formatedArray.get(l).get(f).getSize() << (formatedArray.get(l).get(f).get(k).type == 1 ? " TYPE INT [ " : formatedArray.get(l).get(f).get(k).type == 2 ? " TYPE FLOAT [ " : " TYPE DOUBLE [ ");
					int blockSize = formatedArray.get(l).get(f).getSize();
					int blockType = formatedArray.get(l).get(f).get(k).type;
					writeBlockInfo(writer, blockSize, blockType);
				}
				writeNum(writer, formatedArray.get(l).get(f).get(k).element, formatedArray.get(l).get(f).get(k).type);
				cout << formatedArray.get(l).get(f).get(k).element << " ";
			}
			if (f != 0) cout << "] ";
		}
		cout << endl;
	}
}


/*
switch (type)
	{
	case 3:
		cout << " " << setprecision(15) << convert<double>(unformatedArray.get(i).get(k).element);

		element.setElement(unformatedArray.get(i).get(k).element);
		element.setType(type);
		break;
	case 2:
		cout << " " << setprecision(6) << convert<float>(unformatedArray.get(i).get(k).element);
		element.setElement(unformatedArray.get(i).get(k).element);
		element.setType(type);
		break;
	default:
		cout << " " << convert<int>(unformatedArray.get(i).get(k).element);
		element.setElement(unformatedArray.get(i).get(k).element);
		element.setType(type);
		break;
	}
}
else {
	switch (type)
	{
	case 3:
		cout << endl << "Double: " << setprecision(15) << convert<double>(unformatedArray.get(i).get(k).element);
		break;
	case 2:
		cout << endl << "Float: " << setprecision(6) << convert<float>(unformatedArray.get(i).get(k).element);
		break;
	default:
		cout << endl << "Int: " << convert<int>(unformatedArray.get(i).get(k).element);
		break;
	}
}
*/