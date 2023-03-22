#include "../inc/header.h"

bool cmp_fnames(void * a, void * b)
{
	t_file_info * ta = a;
	t_file_info * tb = b;
	return mx_strcmp(fname_from_path(ta->path), fname_from_path(tb->path)) > 0;
}

bool cmp_fmtime(void * a, void * b)
{
	t_file_info * ta = a;
	t_file_info * tb = b;
	return ta->mtime < tb->mtime;
}

bool cmp_fatime(void * a, void * b)
{
	t_file_info * ta = a;
	t_file_info * tb = b;
	return ta->atime < tb->atime;
}

bool cmp_fctime(void * a, void * b)
{
	t_file_info * ta = a;
	t_file_info * tb = b;
	return ta->ctime < tb->ctime;
}

bool cmp_fsize(void * a, void * b)
{
	t_file_info * ta = a;
	t_file_info * tb = b;
	return ta->size < tb->size;
}

bool lexiograph_ord(void * a, void * b)
{
	char * stra = (char *)a;
	char * strb = (char *)b;
	if (stra[0] == '.' && stra[1] != '.') stra++;
	if (strb[0] == '.' && strb[1] != '.') strb++;
	return mx_strcmp(stra, strb) > 0;
}

