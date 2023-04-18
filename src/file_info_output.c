#include "../inc/header.h"

static void print_colorful(char * str, char * color)
{
	if (!str || !color) return;
	mx_printstr(color);
	mx_printstr(str);
	mx_printstr(RESET);
}

void print_fname_simple(t_file_info * file)
{
	//if (file) mx_printstr(fname_from_path(file->path))
	if (file)
	{
		if (file->from_user)
			mx_printstr(file->path);
		else
		{
			mx_printstr(fname_from_path(file->path));
			//mx_printstr("im here\n");
		}
	}
	//if (file) mx_printstr(file->path);
}

void print_fname_colorful(t_file_info * file)
{
	if (!file) return;
	char * fname = NULL;
	if (file->from_user)
		fname = mx_strdup(file->path);
	else
		fname = fname_from_path(file->path);
	//char * fname = fname_from_path(file->path);
	//char * fname = mx_strdup(file->path);
	switch(file->type)
	{
		case '-': // simple file
			if (mx_get_char_index(file->permissions, 'x') >= 0) //executable -> red
			{
				print_colorful(fname, RED_FCOLOR);
			}
			else // simple file -> white
			{
				mx_printstr(fname);				
			}
			break;
		case 'l':
			print_colorful(fname, PURPLE_FCOLOR);
			break;
		case 'd':
			print_colorful(fname, BLUE_FCOLOR);
			break;
		case 'b':
			print_colorful(fname, BLUE_ON_CYAN);
			break;
		case 'c':
			print_colorful(fname, BLUE_ON_YELLOW);
			break; 
		case 'p':
			print_colorful(fname, YELLOW_FCOLOR);
			break;
	}
}

void print_multicol(t_list * names, void (*print_name)(t_file_info * file))
{
	if (!names) return;
	struct winsize win;
	// find window width
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
	int longest = max_sizeof(names, FNAME_I);;
	int columns = win.ws_col / (longest + 2);
	int rows = mx_ceil((float)mx_list_size(names) / (float)columns);
	if (rows == 0) rows++;
	//char * fname;
	int offset = 2;
	for (int i = 0; i < rows; i++){
		for (int j = i; j < mx_list_size(names); j += rows)
		{ 
			t_file_info * file = (t_file_info *)find_by_i(names, j);
			if (!file) break;
			//fname = fname_from_path(file->path);
			print_name(file);
			//mx_printstr(fname);
			int spaces = longest + offset - mx_strlen(fname_from_path(file->path));
			for (int s = 0; s < spaces; s++){
				mx_printstr(" ");
			}
		}
		mx_printstr("\n");
	}
}

static void print_ftime(time_t time, bool time_full)
{
	char * str;
	if (!time_full)
	{
		str = get_time_str(time);
		mx_printstr(str);
		free(str);
	}
	else
	{
		str = ctime(&time);
		str[mx_strlen(str) - 1] = '\0'; 
		mx_printstr(str);
	}
}

static void print_formated_ftime(t_file_info * fi, enum e_info_ids time_i, bool time_full)
{
	switch(time_i)
	{
		case MTIME_I:
			print_ftime(fi->mtime, time_full);
			break;
		case ATIME_I:
			print_ftime(fi->atime, time_full);
			break;
		case CTIME_I:
			print_ftime(fi->ctime, time_full);
			break;
		default:
			break;
	}
}

static void print_extneded_attrs(t_file_info * fi)
{
	char ** attrs = get_eattrs(fi->path);
	char * len_str;
	for (int i = 0; attrs[i]; i++)
	{
		len_str = mx_itoa(getxattr(fi->path, attrs[i], NULL, 0, 0, XATTR_NOFOLLOW));
		mx_printstr("\t");
		mx_printstr(attrs[i]);
		mx_printstr("\t");
		mx_printstr(len_str);
		mx_printstr("\n");
		free(len_str);
	}
	mx_del_strarr(&attrs);
}

static bool has_device(t_list * files)
{
	for (t_list * f = files; f; f = f->next)
	{
		t_file_info * fi = f->data;
		if (fi->type == 'b' || fi->type == 'c')
			return true;
	}
	return false;
}

static void print_link_dest(t_file_info * fi)
{
	char buf[256];
	ssize_t read = readlink(fi->path, buf, 256);
	if (read != -1)
	{
		mx_printstr(" -> ");
		buf[read] = '\0';
		mx_printstr(buf);
	}
}

void print_fullf_info(t_list * info, t_fname_print_func print_fname, int * params)
{
	//int params[7] = {file_mode, action, time_mode, reversed_sort, full_time_format, human_friendly_size, print_eattrs};
	if (!info || !print_fname || !params) return;
	nlink_t max_link = max_sizeof(info, LINKSC_I);
	int max_oname_len = max_sizeof(info, ONAME_I);
	int max_gname_len = max_sizeof(info, GNAME_I);
	int max_size;
	if (params[5]) // full time format
	{
		max_size = max_sizeof(info, HFSIZE_I);
	}
	else
	{
		max_size = max_sizeof(info, SIZE_I);
	}
	if (has_device(info))
	{
		int mmsize = max_sizeof(info, MAJMIN_I); 
		if (mmsize > max_size)
		{
			max_size = mmsize;
		}
	}

	char * size_str;
	char * link_count;

	for (t_list * f = info; f; f = f->next)
	{
		t_file_info * fi = f->data;
		link_count = mx_itoa(fi->link_count);
		mx_printchar(fi->type);
		mx_printstr(fi->permissions);
		str_repeat(mx_digits_count(max_link) - mx_digits_count(fi->link_count) + 2, " ");
		mx_printstr(link_count);
		mx_printstr(" ");
		mx_printstr(fi->owner_name);
		str_repeat(max_oname_len - mx_strlen(fi->owner_name + 2), " ");		
		mx_printstr(fi->group_name);
		str_repeat(max_gname_len - mx_strlen(fi->group_name + 1), " ");	
		if (fi->type == 'c' || fi->type == 'b')
		{
			char * major = get_major(fi);
			char * minor = get_minor(fi);
			int maj_len = mx_strlen(major);
			int min_len = mx_strlen(minor);
			str_repeat(max_size - min_len - maj_len, " ");
			mx_printstr(major);
			mx_printstr(",   ");
			mx_printstr(minor);
			size_str = mx_strdup("\0");
			free(major);
			free(minor);
		}
		else if (params[5]) // hfriendly
		{
			size_str = float_to_hfstr(fi->size, INFO_NAMES);
			str_repeat(max_size - mx_strlen(size_str) + 1, " "); 
		} 
		else
		{
			str_repeat(max_size - mx_digits_count(fi->size) + 1, " ");
			size_str = mx_itoa(fi->size);
		}
		mx_printstr(size_str);
		mx_printstr(" ");
		print_formated_ftime(fi, params[2], params[4]);
		mx_printstr(" ");
		print_fname(fi);
		if(fi->type == 'l')
		{
			print_link_dest(fi);
		}
		mx_printstr("\n");
		if(params[6])
		{
			print_extneded_attrs(fi);
		}
		free(size_str);
		free(link_count);
	}
}



