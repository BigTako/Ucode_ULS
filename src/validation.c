#include "../inc/header.h"


int file_valid(char * filename)
{
	errno = 0;
	char buf[256];
	DIR * dir = opendir(filename);
	if (errno != 0)
	{
		if (errno == ENOTDIR) 
		{ 
			if (filename[mx_strlen(filename) - 1] == '/')
			{
				mx_strncpy(buf, filename, strlen(filename) - 1);
				//filename[strlen(filename) - 1] = '\0';
				struct stat stats;
				lstat(buf, &stats);
				char type = get_file_type((&stats)->st_mode);
				if (type != 'l')
				{
					return 0;
				}
				else
				{
					return 1;
				}
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

void throw_file_message(t_error_info * info)
{
	if (!file_valid(info->filename))
	{
		mx_printerr("uls: ");
		mx_printerr(info->filename);
		mx_printerr(": ");
		mx_printerr(strerror(info->error));
		mx_printerr("\n");
	}
}

