#include "../inc/header.h"
#include <sys/acl.h>


int main(int argc, char * argv[])
{
	struct stat stats;
	t_list * files = NULL;
	t_list * invalid_files = NULL;
	char * flags = NULL;
	
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1] != '\0') // flags and not cat -e 
		{
			if (!flags_are_valid(argv[i] + 1))
			{
				mx_clear_list(&files, delete_finfo);
				mx_clear_list(&invalid_files, NULL);
				exit(-1);
			}
			flags = argv[i];
		}
		else //files or dirs 
		{
			if (file_valid(argv[i]))
			{
				lstat(argv[i], &stats);
				mx_push_back(&files, get_file_data(&stats, argv[i]));
			}
			else{
				mx_push_back(&invalid_files, argv[i]);
			}
		}
	}
	for (t_list * t = invalid_files; t; t = t->next)
	{
		throw_file_message(t->data);
	}
	if (!(!files && invalid_files))
	{
		handle(&files, flags);
	}
	mx_clear_list(&invalid_files, NULL);
	mx_clear_list(&files, delete_finfo);
	return 0;
}


