#include <stdio.h>
#include <time.h>

#include "cutest.h"
#include "dht.h"

void TestDht_SimpleHashTable(CuTest* tc){
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

void TestDht_MediumSizeTable(CuTest* tc){
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

void TestDht_HashTableWithIterator(CuTest* tc){

	struct DynamicHashTable* aHashTable = InitHashTable();

	struct HashTableIterator* iter = CreateHashTableIterator(aHashTable);
	while(HasNextEntry(iter)) {
		printf("%03d: %s\r\n", GetValue(iter), GetKey(iter));
	}
	DestroyHashTableIterator(iter);	

	FreeHashTable(aHashTable);	
}

CuSuite* DhtTestGetSuite(){
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestDht_SimpleHashTable);
	SUITE_ADD_TEST(suite, TestDht_MediumSizeTable);

	return suite;
}
