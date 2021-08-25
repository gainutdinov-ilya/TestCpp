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

int main(int argc, char* argv[])
{
	string filename;
	float t1 = 1, t2 = 1;
	if (argc == 4) {
		filename = convert<string>(argv[1]);
		for (int i = 0; i < afterComma(argv[2]); i++) t1 *= 10;
		for (int i = 0; i < afterComma(argv[3]); i++) t2 *= 10;
	}
	else if (argc == 2) {
		filename = convert<string>(argv[1]);
	}
	else {
		cout << "Error: Wrong arguments";
		return 0;
	}


	ifstream reader;
	ofstream writer;
	bitset<8> lineSizeBuffer;
	int lineSize = 0, line = 0;
	reader.open(filename, ios::binary);
	writer.open(filename + ".txt");
	while (reader.read((char*)&lineSizeBuffer, sizeof(lineSizeBuffer))) {
		line++;
		lineSize = 0;
		if (lineSizeBuffer.to_ulong() == 255) {
			lineSize += lineSizeBuffer.to_ulong();
			int readLine;
			do {
				reader.read((char*)&lineSizeBuffer, sizeof(lineSizeBuffer));
				readLine = lineSizeBuffer.to_ulong();
				lineSize += readLine;
			} while (readLine == 255);
		}
		else {
			lineSize += lineSizeBuffer.to_ulong();
		}
		cout << "LINE: " << line << " BLOCKS: " << lineSize;
		for (int j = 0; j < lineSize; j++) {
			int blockType, blockSize;
			readBlockInfo(reader, blockSize, blockType);
			double double_, noAbsoluteDouble, absoluteDouble;
			int int_;
			float float_, noAbsoluteFloat, absoluteFloat;
			cout << "\n\tBLOCK SIZE: " << blockSize << (blockType == 1 ? " TYPE INT [ " : blockType == 2 ? " TYPE FLOAT [ " : " TYPE DOUBLE [ ");
			for (int i = 0; i < blockSize; i++) {
				switch (blockType)
				{
				case 3:
					reader.read((char*)&double_, sizeof(double_));
					cout << double_ << " ";
					noAbsoluteDouble = round(double_ * t2) / t2;
					absoluteDouble = round(double_ * t1) / t1;
					double_ = absoluteDouble + (noAbsoluteDouble - absoluteDouble);
					writer << double_ << " ";
					break;
				case 2:
					reader.read((char*)&float_, sizeof(float_));
					cout << float_ << " ";
					cout << round(float_ * t2) / t2 << " ";
					noAbsoluteFloat = round(float_ * t2) / t2;
					absoluteFloat = round(float_ * t1) / t1;
					float_ = absoluteFloat + (noAbsoluteFloat - absoluteFloat);
					writer << float_ << " ";
					break;
				default:
					reader.read((char*)&int_, sizeof(int_));
					cout << int_ << " ";
					writer << int_ << " ";
					break;
				}
			}
			cout << "]";
		}
		cout << endl;
		writer << endl;
	}
	writer.close();
}