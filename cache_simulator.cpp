// Compiling: g++ -std=c++11 cache_simulator.cpp -o cacheSim

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>

using namespace std;

class block {
private:
	int tag;
	int data;
	bool valid;

public:
	block() {
		this->tag = 0;
		this->data = 0;
		this->valid = false;
	}

	~block() {
		delete this;
	}

	int getTag() {
		return this->tag;
	}
};

// a cache  é endereçada a bytes e o endereço possui 32 bits
class cache {
private:
	int cache_size;

	int nSets;			// número de conjuntos
	int block_size;		// tamanho do bloco
	int associativity;	// número de blocos por conjunto
	int nBlocks;

	vector<block> blocks;
	//char replacement_policy; 

public:
	cache(int nSets, int block_size, int associativity) {
		this->nSets = nSets;
		this->block_size = block_size;
		this->associativity = associativity;
	
		this->cache_size =  nSets * block_size * associativity;
		this->nBlocks = nSets * associativity;
		
		//create and add blocks 
		this->blocks.reserve(nBlocks);

		block* emptyBlock;
		for(int i = 0; i < nBlocks; i++) {
			//cout << i << endl;
			emptyBlock = new block();
			this->blocks.push_back(*emptyBlock);
		}
		//for(int i = 0; i<nBlocks;i++){
		//	cout << blocks[i].getTag()<< endl;
		//}
		//cout << blocks.size() << endl;
	}

	~cache() {
		delete this;
	}

	int getSize() {
		return this->cache_size;
	}

	int getNumberOfBlocks() {
		return this->nBlocks;
	}

	block getBlock(int index) {
		return blocks[index];
	}
	//temp
	vector<block> getBlocks() {
		return this->blocks;
	}
	
};




















vector<int> loadFile(char* fileName) {
	fstream bin_in(fileName, ios_base::binary|ios_base::in);
	bin_in.unsetf(ios::skipws);

	// get file size (if needed)
	//streampos fileSize;
    //bin_in.seekg(0, ios::end);
    //fileSize = bin_in.tellg();
    //bin_in.seekg(0, ios::beg);
    //vec.reserve(fileSize);

	vector<int> vec;
    
    int address;
    while(bin_in.read( reinterpret_cast<char*>(&address), sizeof(int))) {
        vec.push_back(address);
    }

    

	return vec;
}

int main(int argc, char* argv[]) {
	//cout << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << endl;

	
	vector<int> vec = loadFile(argv[4]);

	cache* myLittleCache = new cache(stoi(argv[1]), stoi(argv[2]), stoi(argv[3]));
	cout << "===============================================" << endl;
	


	return 0;
}