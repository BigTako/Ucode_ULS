#include "../inc/header.h"


int file_valid(char * filename)
{
	errno = 0;
	DIR * dir = opendir(filename);
	if (errno != 0)
	{
		if (errno == ENOTDIR) 
		{ 
			return 1;
		}
		else // No such file or directory or any other error
		{
			return 0;
		}
	}
	closedir(dir);
	errno = 0;
	return 2; // directory
}

bool flags_are_valid(char * flags)
{
	for (int i = 0; i < mx_strlen(flags); i++)
	{
		if (mx_get_char_index(VALID_FLAGS, flags[i]) < 0)
		{
			mx_printerr("uls: illegal option -- ");
			mx_printerr((char *)&flags[i]);
			mx_printerr("\nusage: uls [-");
			mx_printerr(VALID_FLAGS);
			mx_printerr("] [file ...]\n");
			return false;
		}
	}
	return true;
}

void throw_file_message(char * filename)
{
	if (!file_valid(filename))
	{
		mx_printerr("uls: ");
		mx_printerr(filename);
		mx_printerr(": ");
		mx_printerr(strerror(errno));
		mx_printerr("\n");
	}
	errno = 0;
}

