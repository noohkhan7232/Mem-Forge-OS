#include "types.h"
#include "stat.h"
#include "user.h"

// Make sure that the number of free pages is greater than 0 - basic

int main(void)
{
	int numFreePagesInitial = getFreePagesCount();
	if(numFreePagesInitial > 0) { // Not checking for specific number of free pages
		printf(1, "XV6_COW\t SUCCESS\n");
	} else {
		printf(1, "XV6_COW\t FAILED\n");
	}

 	exit();
}