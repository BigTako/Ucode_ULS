#include "../inc/header.h"
#include <sys/acl.h>

bool cmp_error_info_fname(void * a, void * b)
{
	t_error_info * ta = a;
	t_error_info * tb = b;
	return mx_strcmp(ta->filename, tb->filename) > 0;
}

t_error_info * new_error_info(char * filename, int error)
{
	t_error_info * new_info = malloc(sizeof(t_error_info));
	new_info->filename = mx_strdup(filename);
	new_info->error = error;
	return new_info;
}

void delete_einfo(void * data)
{
	t_error_info * info = (t_error_info *)data;
	if (!info) return;
	free(info->filename);
	free(info);
}

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
				mx_push_back(&files, get_file_data(&stats, argv[i], true));
			}
			else{
				//mx_push_back(&invalid_files, throw_file_message(argv[i], strerror(errno)));
				mx_push_back(&invalid_files, new_error_info(argv[i], errno));
			}
			errno = 0;
		}
	}
	mx_sort_list(invalid_files, cmp_error_info_fname, false);
	for (t_list * t = invalid_files; t; t = t->next)
	{
		throw_file_message(t->data);
	}
	if (!(!files && invalid_files))
	{
		handle(&files, flags);
	}
	mx_clear_list(&invalid_files, delete_einfo);
	mx_clear_list(&files, delete_finfo);
	return 0;
}


