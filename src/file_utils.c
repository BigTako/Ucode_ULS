#include "../inc/header.h"

static long total_dir_bsize(t_list * files)
{
	long int total = 0;
	for (t_list * i = files; i; i = i->next){
		t_file_info * fi = (t_file_info *)i->data;
		total += fi->bsize;
	}
	return total;
}

void print_total_bsize(t_list * files)
{
	char * total_str = mx_itoa(total_dir_bsize(files)); 
	mx_printstr("total ");
	mx_printstr(total_str);
	mx_printstr("\n");
	free(total_str);
}

void delete_finfo(void * ptr)
{
	t_file_info * fi = (t_file_info *)ptr;
	if (malloc_size(fi->permissions)){
		free(fi->permissions);
	}
	if (malloc_size(fi->owner_name)){
		free(fi->owner_name);	
	}
	if (malloc_size(fi->group_name)){
		free(fi->group_name);	
	}
	if (malloc_size(fi->path)){
		free(fi->path);
	}
	free(fi);
}


char * fname_from_path(char *  path)
{
	if (!mx_strchr(path, '/')) return path;
	char * result = mx_strchr(path, '/') + 1;
	while (mx_strchr(result, '/')){
		result = mx_strchr(result, '/') + 1;
	} 
	return result;
}

long max_sizeof(t_list * info, enum e_info_ids ident)
{
	if (!info) return 0;
	long max = -1;
	char * str;
	for (t_list * i = info; i; i = i->next)
	{
		t_file_info * fi = i->data;
		switch (ident)
		{
			case LINKSC_I:
				if (max < fi->link_count){
					max = fi->link_count;
				}
				break;
			case ONAME_I:
				if (max < mx_strlen(fi->owner_name)){
					max = mx_strlen(fi->owner_name);
				}
				break;
			case GNAME_I:
				if (max < mx_strlen(fi->group_name)){
					max = mx_strlen(fi->group_name);
				}
				break;
			case SIZE_I:
				if (max < mx_digits_count(fi->size)) {
					max = mx_digits_count(fi->size);
				}
				break;
			case FNAME_I:
				if (max < mx_strlen(fi->path)){
					max = mx_strlen(fi->path);
				}
			case HFSIZE_I:
				str = float_to_hfstr(fi->size, INFO_NAMES);
				if (max < mx_strlen(str)){
					max = mx_strlen(str);
				}
				free(str);
			case MAJMIN_I:
				if (fi->type == 'b' || fi->type == 'c')
				{
					char * major = get_major(fi);
					char * minor = get_minor(fi);
					int maj_len = mx_strlen(major);
					int min_len = mx_strlen(minor);
					if (max < maj_len + min_len + 3)
					{
						max = maj_len + min_len + 3;
					}
					free(major);
					free(minor);
				}
				break; 
			default:
				break;
		}
	}
	return max;
}

void classificate(t_list ** files, t_list ** dirs, t_file_info * file)
{
	if (file){
		if (file->type == '-'){ // valid file
			mx_push_back(files, file);
		}
		else if (file->type == 'd'){ // valid directory
			mx_push_back(dirs, file);
		}
	}
}

char * float_to_hfstr(float bytes, char * name)
{
	if (bytes / 1024 < 1)
	{
		// transform num into
		char * result = mx_strnew(20);
		int decim = (int)bytes;
		float f = bytes - (float)(int)(bytes);
		int last_digit = (int)(f*10.0);
		int full_size = mx_digits_count(decim) + 3;
		char * decim_str = mx_itoa(decim);
		char * ld_str = mx_itoa(last_digit);
		mx_strcpy(result, decim_str);
		result[full_size - 3] = ',';
		result[full_size - 2] = ld_str[0];
		result[full_size - 1] = (name)[0];
		result[full_size] = '\0';
		free(decim_str);
		free(ld_str);
		return result;
	}
	return float_to_hfstr(bytes / 1024, name+1);
}






