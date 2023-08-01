#include <stdio.h>
#include <time.h>

#include "cutest.h"
#include "dht.h"

void TestDht_SimpleHashTable(CuTest* tc){
	struct DynamicHashTable* myHt = InitHashTable();

	SetEntryInHashTable(myHt, "String1", 10);
	SetEntryInHashTable(myHt, "String2", 20);
	SetEntryInHashTable(myHt, "String3", 30);
	SetEntryInHashTable(myHt, "String4", 40);
	SetEntryInHashTable(myHt, "String5", 50);

	int entryVal = 0;	
	GetEntryInHashTable(myHt, "String1", &entryVal);	
	CuAssertIntEquals(tc, 10, entryVal);	

	GetEntryInHashTable(myHt, "String2", &entryVal);	
	CuAssertIntEquals(tc, 20, entryVal);	

	GetEntryInHashTable(myHt, "String3", &entryVal);	
	CuAssertIntEquals(tc, 30, entryVal);	

	GetEntryInHashTable(myHt, "String4", &entryVal);	
	CuAssertIntEquals(tc, 40, entryVal);	

	GetEntryInHashTable(myHt, "String5", &entryVal);	
	CuAssertIntEquals(tc, 50, entryVal);	

	ClearEntryInHashTable(myHt, "String3");
	bool gotEntry = GetEntryInHashTable(myHt, "String3", &entryVal);	
	CuAssertTrue(tc, gotEntry == false);	

	FreeHashTable(myHt);	
}

CuSuite* DhtTestGetSuite(){
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestDht_SimpleHashTable);

	return suite;
}
