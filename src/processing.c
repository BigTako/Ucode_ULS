#include "../inc/header.h"

void process_action(t_list * items, t_fname_print_func print_fname, bool print_tbsize, int* params)
{
	//int params[7] = {file_mode, action, time_mode, reversed_sort, full_time_format, human_friendly_size, print_eattrs};
	switch (params[1])
	{
		case MULTICOL_OUT_I:
			print_multicol(items, print_fname);
			break;
		case LONG_OUT_I:
			if (print_tbsize)
			{
				print_total_bsize(items);
			}
			print_fullf_info(items, print_fname, params);
			break;
		case SINGLECOL_OUT_I:
			for (t_list * i = items; i; i = i->next)
			{
				t_file_info * file = (t_file_info *)i->data;
				print_fname(file);
				mx_printstr("\n");
			}
			break;
	}
}

static void read_sort_process_clear(t_file_info * dir, t_sort_func sort_func, t_fname_print_func print_fname, bool print_bsize, int * params)
{
	t_list * info = read_files_fro_dir(dir->path, params[0]);
	mx_sort_list(info, sort_func, params[3]);
	process_action(info, print_fname, print_bsize, params);
	mx_clear_list(&info, delete_finfo);
}

void deep_processing(t_list ** data, t_sort_func sort_func, t_fname_print_func print_fname, int * params)
{ 
	//int params[8] = {file_mode, action, time_mode, reversed_sort, full_time_format, human_friendly_size, print_eattrs, recursive_out};
	if (!*data || !print_fname || !params) return;
	t_list * files = NULL;
	t_list * dirs = NULL;
	t_file_info * fi;
	for (t_list * f = *data; f; f = f->next)
	{
		fi = f->data;
		classificate(&files, &dirs, fi);
	}
	mx_sort_list(files, sort_func, params[3]);
	mx_sort_list(dirs, sort_func, params[3]);		
	process_action(*data, print_fname, 1, params);
	if (dirs)
	{
		t_file_info * dir;
		for (t_list * i = dirs; i; i = i->next)
		{
			dir = i->data;
			char * dirname = fname_from_path(dir->path); 
			if (mx_strcmp(dirname, ".") && mx_strcmp(dirname, ".."))
			{
				t_list * info = read_files_fro_dir(dir->path, params[0]);
				if (mx_list_size(dirs) > 1){
					mx_printstr("\n");
					mx_printstr(dir->path);
					mx_printstr(":\n");
				}
				deep_processing(&info, sort_func, print_fname, params);
				mx_clear_list(&info, delete_finfo);
			}
		}
	}
	mx_clear_list(&dirs, NULL);
	mx_clear_list(&files, NULL);
}

void process_separately(t_list * files, t_list * dirs, t_sort_func sort_func, t_fname_print_func print_fname, int * params) 
// print separately names and files
{   //int params[8] = {file_mode, action, time_mode, reversed_sort, full_time_format, human_friendly_size, print_eattrs, recursive_out};
	if (params[7]) // recursive out
	{
		process_action(files, print_fname, 0, params);
		for (t_list * t = dirs; t; t = t->next)
		{	
			t_file_info * dir = t->data;
			t_list * fs = read_files_fro_dir(dir->path, params[0]);
			mx_printstr("\n");
			mx_printstr(dir->path);
			mx_printstr(":\n");
			deep_processing(&fs, sort_func, print_fname, params);
			mx_clear_list(&fs, delete_finfo);
		}
	}
	else if (mx_list_size(files) == 0 && mx_list_size(dirs) == 1)
	{
		t_file_info * fi = (t_file_info *)dirs->data;
		read_sort_process_clear(fi, sort_func, print_fname, 1, params);
	}
	else
	{
		if (files){
			process_action(files, print_fname, 0, params);
			if (dirs) mx_printstr("\n");
		}
		for (t_list * i = dirs; i; i = i->next)
		{
			t_file_info * dir = (t_file_info *)i->data;
			mx_printstr(dir->path);
			mx_printstr(":\n");
			read_sort_process_clear(dir, sort_func, print_fname, 1, params);
			if (i->next) mx_printstr("\n");
		}
	}
}


void process_options(t_list ** info, t_sort_func sort_func, t_fname_print_func print_fname, int * params)
{ //int params[8] = {file_mode, action, time_mode, reversed_sort, full_time_format, human_friendly_size, print_eattrs, recursive_out};
	if ( !sort_func || !print_fname || !params) return;
	t_list * fs = NULL;
	t_list * ds = NULL;
	if (!*info) // no input files (print all files together)
	{
		*info = read_files_fro_dir(".", params[0]);
		mx_sort_list(*info, sort_func, params[3]);
	}
	else // some input files( print files and dirs separately )
	{
		t_file_info * fi;
		for (t_list * f = *info; f; f = f->next)
		{
			fi = f->data;
			classificate(&fs, &ds, fi);
		}
		mx_sort_list(fs, sort_func, params[3]);
		mx_sort_list(ds, sort_func, params[3]);
	}
	if (fs || ds) // print separately
	{
		process_separately(fs, ds, sort_func, print_fname, params);
	}
	else if (params[7]) // recursive out
	{
		deep_processing(info, sort_func, print_fname, params);
	}
	else
	{
		process_action(*info, print_fname, 1, params);
	}
	if (fs) mx_clear_list(&fs, NULL);
	if (ds) mx_clear_list(&ds, NULL);
}

void handle(t_list ** files, char * flags)
{
	//INPUT CORRECTION
	int file_mode = USUAL;	
	enum e_func_ids action;
	if (isatty(1)) // output is to terminal(1)
	{
		action = MULTICOL_OUT_I;
	}
	else // output is not to terminal(0)
	{
		action = SINGLECOL_OUT_I;
	}
	enum e_info_ids time_mode = MTIME_I;
	t_sort_func sorting_func = cmp_fnames;
	t_fname_print_func fname_print_func = print_fname_simple;
	bool reversed_sort = 0;
	bool full_time_format = 0;
	bool human_friendly_size = 0;
	bool recursive_out = 0;
	bool print_eattrs = 0;
	if (mx_get_char_index(flags, 'A') >= 0)
	{
		file_mode = MODEA;
	}
	if (mx_get_char_index(flags, 'a') >= 0)
	{
		file_mode = ALL;
	}
	if (mx_get_char_index(flags, '1') >= 0)
	{
		action = SINGLECOL_OUT_I;
	}
	if (mx_get_char_index(flags, 'C') >= 0)
	{
		action = MULTICOL_OUT_I;
	}
	if (mx_get_char_index(flags, 'r') >= 0)
	{
		reversed_sort = 1;
	}
	if (mx_get_char_index(flags, 'l') >= 0)
	{
		action = LONG_OUT_I;
	}
	if (mx_get_char_index(flags, 'T') >= 0)
	{
		full_time_format = 1;
	}
	if (mx_get_char_index(flags, 'h') >= 0)
	{
		human_friendly_size = 1;
	}
	if (mx_get_char_index(flags, 'u') >= 0)
	{
		time_mode = ATIME_I;
	}
	if (mx_get_char_index(flags, 'c') >= 0)
	{
		time_mode = CTIME_I;
	}
	if (mx_get_char_index(flags, 't') >= 0)
	{
		if (time_mode == MTIME_I){
			sorting_func = cmp_fmtime;
		}
		else if (time_mode == ATIME_I){
			sorting_func = cmp_fatime;
		}
		else if (time_mode == CTIME_I){
			sorting_func = cmp_fctime;
		}
	}
	if (mx_get_char_index(flags, 'S') >= 0)
	{
		sorting_func = cmp_fsize;
	}
	if (mx_get_char_index(flags, 'R') >= 0)
	{
		recursive_out = true;
	}
	if (mx_get_char_index(flags, '@') >= 0)
	{
		print_eattrs = true;
	}
	if (mx_get_char_index(flags, 'G') >= 0)
	{
		fname_print_func = print_fname_colorful;
	}

	int params[8] = {file_mode, action, time_mode, reversed_sort, 
					 full_time_format, human_friendly_size, print_eattrs,
					 recursive_out};

	process_options(files, sorting_func, fname_print_func, params);
}

