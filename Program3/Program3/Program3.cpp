#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>

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
	}

	ifstream binReader;
	binReader.open(filename, ios::binary);
	int totalCount;
	binReader.read((char*)&totalCount, sizeof(totalCount));
	for (int j = 0; j < totalCount; j++) {
		int countInLine;
		binReader.read((char*)&countInLine, sizeof(countInLine));
		for (int i = 0; i < countInLine; i++) {
			float temp;
			binReader.read((char*)&temp, sizeof(temp));
			cout << setprecision(t1 + t2 * abs(temp)) << temp << " " << t1 + t2 * abs(temp) << " ";
		}
		cout << endl;
	}
}