#include "../inc/libmx.h"

bool in(char * array, char item, int len)
{ // check if an item in array(if it is - return true, else - false)
	for (int i = 0; i < len; i++){
		if (array[i] == item){
			return true;
		}
	}
	return false;
}

