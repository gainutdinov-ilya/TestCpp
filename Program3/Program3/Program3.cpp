#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <bitset>

using namespace std;

template<typename T> T convert(string obj) {
	stringstream temp(obj);
	T result;
	if (!(temp >> result)) {
		exit(0);
		return NULL;
	}
	return result;
}

void readBlockInfo(ifstream& reader, int &size, int &type) {
	bitset<8> blockSize;
	bitset<4> blockType;
	reader.read((char*)&blockSize, sizeof(blockSize));
	for (int i = 0; i < 4; i++) {
		 blockType[i] = blockSize[i];
		 blockSize[i] = 0;
	}
	for (int i = 0; i < 4; i++) {
		blockSize[i] = blockSize[i+4];
		blockSize[i+4] = 0;
	}
	size = blockSize.to_ulong();
	type = blockType.to_ulong();

}

int main(int argc, char* argv[])
{
	string filename;
	float t1 = 0.1, t2 = 0.001;
	if (argc == 3) {
		filename = convert<string>(argv[1]);
		t1 = convert<float>(argv[2]);
		t2 = convert<float>(argv[3]);
	}
	else if (argc == 2) {
		filename = convert<string>(argv[1]);
	}
	else {
		cout << "Error: Wrong arguments";
		return 0;
	}


	ifstream reader;
	bitset<8> lineSizeBuffer;
	int lineSize = 0, line = 0;
	reader.open(filename, ios::binary);
	while (reader.read((char*)&lineSizeBuffer, sizeof(lineSizeBuffer))) {
		line++;
		lineSize = 0;
		if (lineSizeBuffer.to_ulong() == 255) {
			lineSize += lineSizeBuffer.to_ulong();
			reader.read((char*)&lineSizeBuffer, sizeof(lineSizeBuffer));
			lineSize += lineSizeBuffer.to_ulong();
		}
		else {
			lineSize += lineSizeBuffer.to_ulong();
		}
		cout << "LINE: " << line << " BLOCKS: " << lineSize;
		for (int j = 0; j < lineSize; j++) {
			int blockType, blockSize;
			readBlockInfo(reader, blockSize, blockType);
			double double_;
			int int_;
			float float_;
			cout << "\n\tBLOCK SIZE: " << blockSize << (blockType == 1 ? " TYPE INT [ " : blockType == 2 ? " TYPE FLOAT [ " : " TYPE DOUBLE [ ");
			for (int i = 0; i < blockSize; i++) {
				switch (blockType)
				{
				case 3:
					reader.read((char*)&double_, sizeof(double_));
					cout << double_ << " ";
					break;
				case 2:
					reader.read((char*)&float_, sizeof(float_));
					cout << float_ << " ";
					break;
				default:
					reader.read((char*)&int_, sizeof(int_));
					cout << int_ << " ";
					break;
				}
			}
			cout << "]";
		}
		cout << endl;
	}

}

/*
//записывает кол-во блоков в записи
void writeLineSize(ofstream& writer, int data) {
	bitset<8> lineSize(data);
	writer.write((char*)&lineSize, sizeof(lineSize));
}

//записывает информацию о блоке
void writeBlockInfo(ofstream& writer, int size, int type) {
	bitset<4> blockSizeBits(size);
	bitset<4> blockTypeBits(type);
}

void writeNum(ofstream& writer, string number, int type) {
	int int_;
	double double_;
	float float_;
	switch (type)
	{
	case 3:
		double_ = convert<float>(number);
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
					cout << "{ B SIZE:" << formatedArray.get(l).get(f).getSize() << (formatedArray.get(l).get(f).get(k).type == 1 ? " TYPE INT [ " : formatedArray.get(l).get(f).get(k).type == 2 ? " TYPE FLOAT [ " : " TYPE DOUBLE [ ");
					writeBlockInfo(writer, formatedArray.get(l).get(f).getSize(), formatedArray.get(l).get(f).get(k).type);
				}
				writeNum(writer, formatedArray.get(l).get(f).get(k).element, formatedArray.get(l).get(f).get(k).type);
				cout << formatedArray.get(l).get(f).get(k).element << " ";
			}
			if (f != 0) cout << "] ";
		}
		cout << endl;
	}
}

*/