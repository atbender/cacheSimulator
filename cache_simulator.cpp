// Compiling: g++ -std=c++11 cache_simulator.cpp -o cacheSim

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <cstdlib>
#include <ctime>

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

	bool getValid() {
		return this->valid;
	}

	void setValid(bool status) {
		this->valid = status;
	}

	void setTag(int tag) {
		this->tag = tag;
	}

	void setData(int data) {
		this->data = data;
	}
};

// a cache  é endereçada a bytes e o endereço possui 32 bits
class cache {
private:
	int cache_size;
	int currentLoad;
	int nBlocks;
	int totalAccesses;

	int nSets;			// número de conjuntos
	int block_size;		// tamanho do bloco
	int associativity;	// número de blocos por conjunto
	

	int totalMisses;
	int compulsoryMiss;
	int capacityMiss;
	int conflictMiss;
	int totalHits;

	vector<block> blocks;
	//char replacement_policy; 

public:
	cache(int nSets = 1024, int block_size = 4, int associativity = 1) {
		this->nSets = nSets;
		this->block_size = block_size;
		this->associativity = associativity;
	
		this->cache_size =  nSets * block_size * associativity;
		this->nBlocks = nSets * associativity;
		this->totalAccesses = 0;
		
		this->totalMisses = 0;
		this->compulsoryMiss = 0;
		this->capacityMiss = 0;
		this->conflictMiss = 0;
		this->totalHits = 0;
		this->currentLoad = 0;

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

	void insertTA(int address) {
		srand(time(NULL));

		int tag = address;

		if(this->currentLoad == this->nBlocks) {
			int index = rand() % nBlocks;

			//this->blocks[index].setValid(true);
			this->blocks[index].setTag(tag);
			this->blocks[index].setData(1);
			return;
		}
		else {
			for(int index = 0; index < nBlocks; index++) {
				if(!(this->blocks[index].getValid())) {
					this->blocks[index].setValid(true);
					this->blocks[index].setTag(tag);
					this->blocks[index].setData(1);
					this->currentLoad++;
					return;
				}
			}
		}
	}

	// used in totalmente associativa caches
	void accessBlockTA(int address) {
		

		int tag = address;

		//bool flag = false;
		for(int index = 0; index < nBlocks; index++) {
			if(this->blocks[index].getValid()) {
			// valid
				if(tag == this->blocks[index].getTag()) {
					// hit
					//cout << "hit "<< address << "tag " << tag <<" gettag " << this->blocks[index].getTag()<< endl;
					this->totalHits++;
					return;
				}
			}
		}

		// miss
		if(this->currentLoad == this->nBlocks) {
			// capacity miss
			this->capacityMiss++;
			this->conflictMiss++; // conferir
		}
		else {
			this->compulsoryMiss++;
		}
		// all misses are capacity misses (probably)
		this->totalMisses++;
		insertTA(address);
	}

	// used in direct mapped caches
	void accessBlockDP(int address) {
		int index = address % this->nBlocks;
		int tag = address / block_size / nBlocks;

		if(this->blocks[index].getValid()) {
			// valid
			if(tag == this->blocks[index].getTag()) {
				// hit
				this->totalHits++;
			}
			else {
				// miss
				if(this->currentLoad == this->nBlocks) {
					// capacity miss
					this->capacityMiss++;
				}
				else {
					this->currentLoad++;
				}
				// all misses are conflict misses
				this->totalMisses++;
				this->conflictMiss++;
				this->blocks[index].setTag(tag);
				this->blocks[index].setData(1);
			}
		}
		else {
			// not valid, compulsory miss
			this->blocks[index].setValid(true);
			this->blocks[index].setTag(tag);
			this->blocks[index].setData(1);

			this->totalMisses++;
			this->compulsoryMiss++;
			this->currentLoad++;
		}

	}

	void testCache(vector<int> addressesTests) {
		this->totalAccesses = addressesTests.size();
		if(this->associativity == 1) {
			// cache is direct mapped
			for(int i = 0; i < addressesTests.size(); i++) {
				//cout << i << endl;
				accessBlockDP(addressesTests[i]);
			}
		}
		else if(nSets == 1) {
			// cache is totalmente associativa
			for(int i = 0; i < addressesTests.size(); i++) {
				//cout << i << endl;
				accessBlockTA(addressesTests[i]);
			}
		}
		else {
			// cache is associativa por conjunto

		}
	}
	
	void displayLog() {
		cout << "===============================================" << endl;
		cout << "Total Misses: " << this->totalMisses << endl;
		cout << "Compulsory Misses: " << this->compulsoryMiss << endl;
		cout << "Conflict Misses: " << this->conflictMiss << endl;
		cout << "Capacity Misses: " << this->capacityMiss << endl;
		cout << "Hits: " << this->totalHits << endl;
		cout << "Miss Rate: " << (float)totalMisses / totalAccesses << endl;
		cout << "Hit Rate: " << (float)totalHits / totalAccesses << endl;
		cout << "===============================================" << endl;
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
	vector<int> vec;
	cache* myLittleCache;

	if(argc == 5) {
		vec = loadFile(argv[4]);
		myLittleCache = new cache(stoi(argv[1]), stoi(argv[2]), stoi(argv[3]));
	}
	else if(argc == 2) {
		myLittleCache = new cache();
		vec = loadFile(argv[1]);
	}
	else {
		cout << "Invalid input." << endl;
		return 0;
	}
	
	

	
	
	myLittleCache->testCache(vec);
	myLittleCache->displayLog();


	return 0;
}