#include <iostream>
#include <cmath>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <random>
#include <stdlib.h>
#include <bitset>
#include <thread>
#include "zlib.h"


#define CHUNK_SIZE 1024

using namespace std;
using namespace std::chrono_literals;

//класс динамического массива
template<class T> class DynamicArray {
private:
	size_t size = 0;
	T* arr = new T[size];
public:
	void add(T obj) {
		int oldSize = size;
		T* temp_ = new T[size];
		swap(temp_, arr);
		delete[] arr;
		size++;
		arr = new T[size];
		for (int i = 0; i < oldSize; i++) {
			arr[i] = temp_[i];
		}
		delete[] temp_;
		arr[oldSize] = obj;
	}
	T get(int id) {
		return arr[id];
	}
	size_t getSize() {
		return size;
	}
	void clear() {
		size = 0;
		delete[] arr;
		arr = new T[size];
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
	BlockElement() {
		return;
	}
	BlockElement(string e, int t) {
		type = t;
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
	int afterCommaCount = afterComma(number);
	if (afterCommaCount > 0) {
		if (afterCommaCount > 7) {
			return 3;
		}
		return 2;
	}
	return 1;
}

bool compressFile(ifstream& src, ofstream& dst, int COMPRESSION_LEVEL)
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

bool decompressFile(ofstream& dst, ifstream& src)
{
	uint8_t inbuff[CHUNK_SIZE];
	uint8_t outbuff[CHUNK_SIZE];
	z_stream stream = { 0 };
	int result = inflateInit(&stream);
	if (result != Z_OK)
	{
		fprintf(stderr, "Failed!\n");
		return false;
	}
	do {
		src.read((char*)&inbuff, CHUNK_SIZE);
		stream.avail_in = sizeof(inbuff);
		if (stream.avail_in == 0)
			break;
		stream.next_in = inbuff;
		do {
			stream.avail_out = CHUNK_SIZE;
			stream.next_out = outbuff;
			result = inflate(&stream, Z_NO_FLUSH);
			if (result == Z_NEED_DICT || result == Z_DATA_ERROR ||
				result == Z_MEM_ERROR)
			{
				fprintf(stderr, "failed: %d\n", result);
				inflateEnd(&stream);
				return false;
			}
			uint32_t nbytes = CHUNK_SIZE - stream.avail_out;
			dst.write((char*)&outbuff, nbytes);
		} while (stream.avail_out == 0);
	} while (result != Z_STREAM_END);
	inflateEnd(&stream);
	return result == Z_STREAM_END;
}

//записывает кол-во блоков в записи
void writeLineSize(ofstream& writer, int data) {
	if (data >= 255) {
		int data_ = data;
		while (data_ >= 255) {
			int temp = 255;
			bitset<8> lineSize(temp);
			writer.write((char*)&lineSize, sizeof(lineSize));
			data_ -= 255;
		}
		bitset<8> lineSize(data_);
		writer.write((char*)&lineSize, sizeof(lineSize));

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

template<typename T> T getRandomNumber(T min, T max)
{
	random_device rd;
	srand(rd());
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	return static_cast<T>(rand() * fraction * (max - min + 1) + min);
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

void splitLine(DynamicArray<BlockElement> &out,string line, int &OK) {
	stringstream temp;
	temp << line;
	string num;
	DynamicArray<BlockElement> tempArray;
	while (temp >> num) {
		int type = getType(num);
		BlockElement element(num, type);
		tempArray.add(element);
	}
	out = tempArray;
	OK += 1;
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		cout << "Error: wrong arguments!";
		return 0;
	}
	ifstream reader;
	int compressLevel = -1;
	string filename = argv[1];

	stringstream r;
	r << getRandomNumber<int>(1, 99999999);
	string tempFileRand;
	r >> tempFileRand;
	stringstream k(filename + tempFileRand + ".temp");
	string tempFile;
	k >> tempFile;

	if (fileType(filename) == 1) {
		reader.open(filename);
	}
	else if (fileType(filename) == 2) {
		cout << "Decompressing..";
		ifstream temp1(filename, ios::binary);
		ofstream temp2(tempFile, ios::binary);
		bool result = decompressFile(temp2, temp1);
		temp1.close();
		temp2.close();
		reader.open(tempFile);
		if (result) {
			cout << "OK" << endl;
		}
		else {
			return -1;
		}
	}
	else {
		cout << "Wrong argument: type of file not supported!";
	}
	if (argc == 3) {
		if (convert<string>(argv[2]) == "-c0") {
			compressLevel = 0;
		}
		else if (convert<string>(argv[2]) == "-c1") {
			compressLevel = 1;
		}
		else if (convert<string>(argv[2]) == "-c2") {
			compressLevel = 9;
		}
		else {
			cout << "Wrong argument: compress level not allowed!" << endl;
			return -1;
		}
	}

	if (!reader.is_open()) {
		cout << "Error: File not found or file open!";
		return 0;
	}

	cout << "Reading file..";
	string line;
	DynamicArray<string> fileText;
	while (!reader.eof()) {
		getline(reader, line);
		fileText.add(line);
	}
	reader.close();
	cout << "OK" << endl;

	cout << "Splitting lines..";
	DynamicArray<DynamicArray<BlockElement>> unformatedArray;
	for (int i = 0; i < fileText.getSize(); i++) {
		const int countThreads = 10;
		DynamicArray<BlockElement> tempArray[countThreads];
		int created = 0, threadResult = 0;
		for (int l = 0; i < fileText.getSize() && l < countThreads; l++, i++) {
			line = fileText.get(i);
			created++;
			thread t(splitLine, ref(tempArray[l]), line, ref(threadResult));
			t.detach();
		}
		while (threadResult != created);
		for (int l = 0; l < created; l++) {
			unformatedArray.add(tempArray[l]);
		}
	}
	cout << "OK" << endl;
	
	cout << "Formatting..";
	DynamicArray<DynamicArray<DynamicArray<BlockElement>>> formatedArray;
	for (int i = 0; i < unformatedArray.getSize(); i++) {
		int countBlocks = 0;
		int last_type = 0;
		DynamicArray<DynamicArray<BlockElement>> line;
		DynamicArray<BlockElement> block;
		for (int k = 0; k < unformatedArray.get(i).getSize(); k++) {
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
			if (k + 1 == unformatedArray.get(i).getSize()) {
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
		unformatedArray.get(i).clear();
	}
	unformatedArray.clear();
	
	cout << "OK" << endl << "Writing file..";

	ofstream writer(filename + ".bin", ios::binary);
	for (int l = 0; l < formatedArray.getSize(); l++) {
		writeLineSize(writer, formatedArray.get(l).getSize() - 1);
		for (int f = 0; f < formatedArray.get(l).getSize(); f++) {
			for (int k = 0; k < formatedArray.get(l).get(f).getSize(); k++) {
				if (k == 0) {
					int blockSize = formatedArray.get(l).get(f).getSize();
					int blockType = formatedArray.get(l).get(f).get(k).type;
					writeBlockInfo(writer, blockSize, blockType);
				}
				writeNum(writer, formatedArray.get(l).get(f).get(k).element, formatedArray.get(l).get(f).get(k).type);
			}
		}
	}
	writer.close();
	cout << "OK" << endl;
	if (compressLevel != -1) {
		cout << "Compressing..";
		ifstream temp1(filename + ".bin", ios::binary);
		ofstream temp2(filename + ".binz", ios::binary);
		compressFile(temp1, temp2, compressLevel);
		temp1.close();
		temp2.close();
		::remove((filename + ".bin").c_str());
		cout << "OK";
	}
	if (fileType(filename) == 2) {
		::remove(tempFile.c_str());
	}
	

}