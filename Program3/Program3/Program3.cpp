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
		return 0;
	}


	ifstream reader;
	__int8  type;
	__int8 inLine;
	float float_;
	int int_;
	double double_;
	reader.open(filename, ios::binary);
	while (reader.read((char*)&inLine, sizeof(inLine))) {
		for (int i = 0; i < inLine; i++) {
			reader.read((char*)&type, sizeof(type));
			switch (type)
			{
			case 1:
				reader.read((char*)&int_, sizeof(int_));
				cout << int_ << " ";
				break;
			case 2:
				reader.read((char*)&float_, sizeof(float_));
				cout  << setprecision(7)<< float_ << " ";
				break;
			case 3:
				reader.read((char*)&double_, sizeof(double_));
				cout << setprecision(16) << double_ << " ";
				break;
			default:
				break;
			}
		}
		cout << endl;
	}

}