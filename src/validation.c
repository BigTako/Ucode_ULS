#include "../inc/header.h"


int file_valid(char * filename)
{
	errno = 0;
	DIR * dir = opendir(filename);
	if (errno != 0)
	{
		if (errno == ENOTDIR) 
		{ 
			if (filename[mx_strlen(filename) - 1] == '/') {
				return 0;
			}
			return 1;
		}
		else // No such file or directory or any other error
		{
			return 0;
		}
	}
	closedir(dir);
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

char * throw_file_message(char * filename, char * error)
{
	if (!filename || !error) return NULL;
	char *r = mx_strnew(mx_strlen(filename) + mx_strlen(error) + 9);
	r = mx_strcat(r, "uls: ");
	r = mx_strcat(r, filename);
	r = mx_strcat(r, ": ");
	r = mx_strcat(r, error);
	r = mx_strcat(r, "\n");
	return r;
}

