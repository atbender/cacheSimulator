// Compiling: g++ -std=c++11 cache_simulator.cpp -o cacheSim

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <cstdlib>
#include <ctime>
#include <list>
#include <algorithm>

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


class cache {
private:
	int cache_size;
	int currentLoad;
	int nBlocks;
	char replacementPolicy;
	
	int nSets;			// número de conjuntos
	int block_size;		// tamanho do bloco
	int associativity;	// número de blocos por conjunto
	
	int totalAccesses;
	int totalMisses;
	int compulsoryMiss;
	int capacityMiss;
	int conflictMiss;
	int totalHits;

	vector<block> blocks;
	vector<list<int>> queue;

public:
	/*
		Inicializa parâmetros da cache, reserva número de blocos da cache, cria blocos e os insere no vetor de blocos.
	*/
	cache(int nSets = 1024, int block_size = 4, int associativity = 1, char replacementPolicy = 'r') {
		this->nSets = nSets;
		this->block_size = block_size;
		this->associativity = associativity;
		this->replacementPolicy = replacementPolicy;
	
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
			emptyBlock = new block();
			this->blocks.push_back(*emptyBlock);
		}
		//cout << "äte aqui nao deu segfault";
		//this->queue.reserve(nSets);
		

		if(this->replacementPolicy != 'r') {
			for(int i = 0; i < this->nSets; i++) {
				list<int> emptyList;
				this->queue.push_back(emptyList);
			}
		}
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

	vector<block> getBlocks() {
		return this->blocks;
	}

	/*
		Gera índice aleatório dentro do conjunto especificado.
	*/
	int randomReplacement(int set) {
		srand(time(NULL));

		return set *this->associativity + (rand() % this->associativity);
	}

	/*
		Gerencia a fila de substituição em políticas LRU e FIFO.
	*/
	int queueReplacement(int set) {
		int replaceTag = this->queue[set].front();
		this->queue[set].pop_front();

		for(int index = set * this->associativity; index < (set * this->associativity + this->associativity); index++) {
			if(this->blocks[index].getTag() == replaceTag) {
				return index;
			}
		}
	}

	/*
		Recebe endereço e carga do conjunto em questão.
		Procura um espaço livre no conjunto e insere se possível.
		Se estiver cheia, substitui de acordo com a política.
	*/
	void insertAddress(int address, int setLoad) {
	
		int set = (address / this->block_size) % this->nSets;
		int tag = address / this->block_size / this->nSets;
	
		if(setLoad == this->associativity) {
			int index;
			if(this->replacementPolicy == 'r') {
				index = randomReplacement(set);	
			}
			else {
				index = queueReplacement(set);
				this->queue[set].push_back(tag);	
			}
			
			this->blocks[index].setTag(tag);
			this->blocks[index].setData(1);
			this->conflictMiss++;
			//cout << "   Conflict Miss at " << set << endl;
			return;
		}
		
		for(int index = set * this->associativity; index < (set * this->associativity + this->associativity); index++) {
			if(!(this->blocks[index].getValid())) {
				this->blocks[index].setValid(true);
				this->blocks[index].setTag(tag);
				this->blocks[index].setData(1);
				this->currentLoad++;
				this->compulsoryMiss++;
				//cout << "   Compulsory Miss at " << set << endl;
				if(this->replacementPolicy == 'f') {
					if(!(find(this->queue[set].begin(), this->queue[set].end(), tag) != this->queue[set].end())) {
						this->queue[set].push_back(tag);
					}
				}
				if(this->replacementPolicy == 'l') {
					if(!(find(this->queue[set].begin(), this->queue[set].end(), tag) != this->queue[set].end())) {
						this->queue[set].push_back(tag);
					}
					else {
						this->queue[set].remove(tag);
						this->queue[set].push_back(tag);
					}
				}
				return;
			}
		}
	}

	/*
		Recebe um endereço e o procura na cache.
		É uma funcão genérica para mapeamento associativo por conjunto.
		Se a cache utilizar política de mapeamento direto, irá procurar apenas em um bloco da cache.
		Senão irá procurar em todos os blocos do conjunto.
		Em caso de totalmente associativa, esse conjunto é toda a cache.

		Se achar o endereço, incrementa hits.
		Caso contrário, incrementa misses e chama função de inserir endereço na cache.
	*/
	void accessAddress(int address) {
		int set = (address / this->block_size) % this->nSets;//(address %  this->nSets) * associativity;
		int tag = address / this->block_size / this->nSets;
		int setLoad = 0;

		for(int index = set * this->associativity; index < (set * this->associativity + this->associativity); index++) {
			if(this->blocks[index].getValid()) {
				setLoad++;
				if(tag == this->blocks[index].getTag()) {
					this->totalHits++;
					//cout << "   Hit at set " << set << endl;
					return;
				}
			}
		}
		if(this->currentLoad == this->nBlocks) {
			//cout << "   Capacity Miss at set " << set << endl;
			this->capacityMiss++;
		}
		this->totalMisses++;

		insertAddress(address, setLoad);
	}

	/*
		Tenta acessar cada um dos elementos do vector de endereços
	*/
	void testCache(vector<int> addressesTests) {
		this->totalAccesses = addressesTests.size();

		for(int i = 0; i < addressesTests.size(); i++) {
			accessAddress(addressesTests[i]);
		}
	}
	
	/*
		Imprime resultados da simulação.
	*/
	void displayLog() {
		cout << "================Simulation Log================" << endl;
		cout << "Total Accesses: " << this->totalAccesses << endl;
		cout << "Hits: " << this->totalHits << endl;
		cout << "Misses: " << this->totalMisses << endl;
		cout << "     Compulsory Misses: " << this->compulsoryMiss << endl;
		cout << "     Conflict Misses: " << this->conflictMiss << endl;
		cout << "     Capacity Misses: " << this->capacityMiss << endl;
		cout << "Miss Rate: " << (float)totalMisses / totalAccesses << endl;
		cout << "Hit Rate: " << (float)totalHits / totalAccesses << endl;
		cout << "===============================================" << endl;
	}
};



/*
	Recebe nome do arquivo e retorna vector inteiro de endereços.
*/
vector<int> loadFile(char* fileName) {
	fstream bin_in(fileName, ios_base::binary|ios_base::in);
	bin_in.unsetf(ios::skipws);

	vector<int> addressVector;
    
    int address;
    while(bin_in.read( reinterpret_cast<char*>(&address), sizeof(int))) {
        addressVector.push_back(address);
    }

	return addressVector;
}

/*
	Recebe recebe parâmetros de argv em uma string e separa utilizando delimitador :, retorna vector das substrings.	
*/
vector<string> processParams(char* inputString) {
	string s = inputString;
	string delimiter = ":";

	size_t pos = 0;
	string token;
	vector<string> params;
	while ((pos = s.find(delimiter)) != string::npos) {
		token = s.substr(0, pos);
		params.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	params.push_back(s);
	return params;
}

/*
	Carrega parâmetros, arquivo de entrada, simula cache e imprime resultados.
*/
int main(int argc, char* argv[]) {
	
	vector<int> addressVector;
	cache* myLittleCache;

	if(argc == 3) {
		addressVector = loadFile(argv[2]);
		vector<string> params = processParams(argv[1]);
		myLittleCache = new cache(stoi(params[0]), stoi(params[1]), stoi(params[2]), params[3][0]);
	}
	else if(argc == 2) {
		addressVector = loadFile(argv[1]);
		myLittleCache = new cache();
	}
	else {
		cout << "Invalid input." << endl;
		return 0;
	}
	
	myLittleCache->testCache(addressVector);
	myLittleCache->displayLog();

	return 0;
}



/*
	// used in direct mapped caches (obsolete)
	void accessAddressDP(int address) {
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

/*
	void insertFA(int address) {
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
	void accessBlockFA(int address) {
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
		insertFA(address);
	}
	*/
