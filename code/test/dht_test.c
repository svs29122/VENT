#include <stdio.h>
#include <string.h>
#include <time.h>

#include "valgrind.h"
#include "cutest.h"
#include "dht.h"

void TestDht_SmallHashTable(CuTest* tc){
	struct DynamicHashTable* myHt = InitHashTable();

	SetInHashTable(myHt, "String1", 10);
	SetInHashTable(myHt, "String2", 20);
	SetInHashTable(myHt, "String3", 30);
	SetInHashTable(myHt, "String4", 40);
	SetInHashTable(myHt, "String5", 50);

	int entryVal = 0;	
	GetInHashTable(myHt, "String1", &entryVal);	
	CuAssertIntEquals(tc, 10, entryVal);	

	GetInHashTable(myHt, "String2", &entryVal);	
	CuAssertIntEquals(tc, 20, entryVal);	

	GetInHashTable(myHt, "String3", &entryVal);	
	CuAssertIntEquals(tc, 30, entryVal);	

	GetInHashTable(myHt, "String4", &entryVal);	
	CuAssertIntEquals(tc, 40, entryVal);	

	GetInHashTable(myHt, "String5", &entryVal);	
	CuAssertIntEquals(tc, 50, entryVal);	

	ClearInHashTable(myHt, "String3");
	bool gotEntry = GetInHashTable(myHt, "String3", &entryVal);	
	CuAssertTrue(tc, gotEntry == false);	

	FreeHashTable(myHt);	
}

void TestDht_MediumHashTable(CuTest* tc){
	struct DynamicHashTable* pokedex = InitHashTable();

	char* firstGeneration[] = {
		"bulbasaur", "ivysaur", "venusaur", "charmander", "charmeleon",
		"charizard", "squirtle", "wartortle", "blastoise", "caterpie",
		"metapod", "butterfree", "weedle", "kakuna", "beedrill",
		"pidgey", "pidgeotto", "pidgeot", "rattata", "raticate",
		"spearow", "fearow", "ekans", "arbok", "pikachu",
		"raichu", "sandshrew", "sandslash", "nidoran\u2641", "nidorina", 
		"nidoqueen", "nidoran\u2642", "nidorino", "nidoking", "clefairy", 
		"clefable", "vulpix", "ninetales", "jigglypuff", "wigglytuff",
		"zubat", "golbat", "oddish", "gloom", "vileplume", 
		"paras", "parasect", "venonat", "venomoth","diglett", 
		"dugtrio", "meowth", "persian", "psyduck", "golduck", 
		"mankey", "primeape", "growlithe", "arcanine", "poliwag", 
		"poliwhirl","polywrath", "abra", "kadabra", "alakazam", 
		"machop","machoke", "machamp", "bellsprout", "weepinbell", 
		"victreebell", "tentacool", "tentacruel", "geodude", "graveler", 
		"golem", "ponyta", "rapidash", "slowpoke", "slowbro", 
		"magnemite", "magneton", "farfetch'd", "doduo", "dodrio", 
		"seel", "dewgong", "grimer", "muk", "shelder", 
		"cloyster", "gastly", "haunter", "gengar", "onix", 
		"drowzee", "hypno", "krabby", "kingler", "voltorb", 
		"electrode", "exeggcute", "exeggutor", "cubone", "marowak", 
		"hitmonlee", "hitmonchan", "lickitung", "koffing", "weezing", 
		"rhyhorn", "rhydon", "chansey", "tangela","kangaskhan", 
		"horsea", "seadra", "goldeen", "seaking", "staryu", 
		"starmie", "mr. mime", "scyther", "jynx", "electrabuzz", 
		"magmar", "pinsir", "tauros", "magikarp", "gyarados", 
		"lapris", "ditto", "eevee", "vaporeon", "jolteon", 
		"flareon", "porygon", "omanyte", "omastar", "kabuto", 
		"kabutops", "aerodactyl", "snorlax", "articuno", "zapdos", 
		"moltres", "dratini", "dragonair", "dragonite", "mewtwo", 
		"mew"	
	};
	const int numInFirstGen = sizeof(firstGeneration) / sizeof(char*);

	for(int i=0; i<numInFirstGen; i++){
		SetInHashTable(pokedex, firstGeneration[i], i+1);
	}

	// iterate through the list manually
	const int numInTable = EntryCount(pokedex);

	for(int i=0; i<numInTable; i++){
		
		int number = 0;
		bool gotMonster = GetInHashTable(pokedex, firstGeneration[i], &number);
		if(gotMonster){
			CuAssertIntEquals(tc, i+1, number);	
#ifdef PRINT_FIRST_GENERATION
			printf("%03d: %s\r\n", number, firstGeneration[i]);
#endif
		}
	}

	FreeHashTable(pokedex);
}

static void iterateOverTable(CuTest* tc, struct DynamicHashTable* aHashTable){
	struct HashTableIterator* iter = CreateHashTableIterator(aHashTable);
	while(HasNextEntry(iter)) {
		int storedVal = GetValue(iter);
		int lookUpVal = 0;
		bool gotEntry = GetInHashTable(aHashTable, GetKey(iter), &lookUpVal);

		//printf("%03d: %s\r\n", GetValue(iter), GetKey(iter));
		CuAssertTrue(tc, gotEntry);
		CuAssertIntEquals(tc, storedVal, lookUpVal); 
	}
	DestroyHashTableIterator(iter);	
}

void TestDht_LargeHashTableWithIterator(CuTest* tc){

	int LARGE_NUMBER = 1000000;
	struct DynamicHashTable* aHashTable = InitHashTable();
	
	// to keep valgrind fast we'll reduce this when it's running
	if(RUNNING_ON_VALGRIND) LARGE_NUMBER /= 10;

	for(int i=1; i < LARGE_NUMBER; i++){
		char stringBuf[32] = "String";
		char numBuf[16];
		
		sprintf(numBuf, "%d", i*3+7);
		strcat(stringBuf, numBuf);
		SetInHashTable(aHashTable, stringBuf, i);
	}
	iterateOverTable(tc, aHashTable);

	//update some entries
	for(int i=1; i < LARGE_NUMBER; i++){
		char stringBuf[32] = "String";
		char numBuf[16];
		
		sprintf(numBuf, "%d", i*3+7);
		strcat(stringBuf, numBuf);
		SetInHashTable(aHashTable, stringBuf, i*2-1);
	}
	iterateOverTable(tc, aHashTable);

	//clear some entries
	for(int i=LARGE_NUMBER >> 1; i < LARGE_NUMBER; i++){
		char stringBuf[32] = "String";
		char numBuf[16];
		
		sprintf(numBuf, "%d", i*3+7);
		strcat(stringBuf, numBuf);
		ClearInHashTable(aHashTable, stringBuf);
	}
	iterateOverTable(tc, aHashTable);

	CuAssertTrue(tc, EntryCount(aHashTable) == LARGE_NUMBER-1);
	FreeHashTable(aHashTable);	
}

CuSuite* DhtTestGetSuite(){
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestDht_SmallHashTable);
	SUITE_ADD_TEST(suite, TestDht_MediumHashTable);
	SUITE_ADD_TEST(suite, TestDht_LargeHashTableWithIterator);

	return suite;
}
