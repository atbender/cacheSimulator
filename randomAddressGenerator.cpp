#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main(int argc, char* argv[]) {
	srand(time(NULL));
	
	ofstream text_out("textData.txt");
	fstream bin_out("binData.bin",ios_base::binary|ios_base::out);

	int generatedAddress = 0;

	for(int i = 0; i < 100; i++) {
		generatedAddress = rand() % 10;
		bin_out.write((char*)&generatedAddress, sizeof(int));
		text_out << generatedAddress << endl;
	}
	for(int i = 0; i < 100; i++) {
		generatedAddress = rand() % 1000;
		bin_out.write((char*)&generatedAddress, sizeof(int));
		text_out << generatedAddress << endl;
	}

	text_out.close();
	bin_out.close();

	cout << "File generated successfully." << endl;
  return 0;
}