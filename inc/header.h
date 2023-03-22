#ifndef HEADER_H
#define HEADER_H
#include "../libmx/inc/libmx.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/xattr.h>

#define VALID_FLAGS "ACGRST@acehlrtu1"
#define INFO_NAMES "BKMGTP"

#define USUAL 1 // files without any dot
#define MODEA 2 // files with dot but no '.' or '..'
#define ALL 3 // all files

//CONSOLE COLORS
#define BLACK_FCOLOR "\033[30m"
#define RED_FCOLOR "\033[31m"
#define GREEN_FCOLOR "\033[32m"
#define YELLOW_FCOLOR "\033[33m"
#define BLUE_FCOLOR "\033[34m"
#define PURPLE_FCOLOR "\033[35m"
#define CYAN_FCOLOR "\033[36m"
#define WHITE_FCOLOR "\033[37m"

#define BLUE_ON_YELLOW "\033[34;43m"
#define BLUE_ON_CYAN "\033[34;46m"
#define RESET "\033[0m"

typedef struct s_file_info 
{
	char type;
	char * permissions;
	nlink_t link_count;
	char * owner_name;
	char * group_name;
	dev_t dev_id;
	off_t size; // total file size
	quad_t bsize; // block size
	time_t mtime; // last file data modicifation time
	time_t atime; // last access time
	time_t ctime; // last status change time
	char * path;
} t_file_info;

enum e_info_ids
{
	TYPE_I,
	PERMS_I,
	LINKSC_I,
	ONAME_I,
	GNAME_I,
	SIZE_I,
	MTIME_I,
	ATIME_I,
	CTIME_I,
	FNAME_I,
	HFSIZE_I,
	MAJMIN_I
};

enum e_func_ids
{
	MULTICOL_OUT_I,
	LONG_OUT_I,
	SINGLECOL_OUT_I
};

typedef bool (*t_sort_func)(void * a, void * b);
typedef void (*t_fname_print_func)(t_file_info * file);

//FILE UTILS
void print_total_bsize(t_list * files);
void delete_finfo(void * ptr);
void classificate(t_list ** files, t_list ** dirs, t_file_info * file);
long max_sizeof(t_list * info, enum e_info_ids ident);
char * float_to_hfstr(float bytes, char * name);
char * fname_from_path(char *  path);
//------------------------------------------->

//COMPARING
bool cmp_fnames(void * a, void * b);
bool cmp_fmtime(void * a, void * b);
bool cmp_fatime(void * a, void * b);
bool cmp_fctime(void * a, void * b);
bool cmp_fsize(void * a, void * b);
bool lexiograph_ord(void * a, void * b);
//------------------------------------------->

//VALIDATION
bool flags_are_valid(char * flags);
void throw_file_message(char * filename);
int file_valid(char * filename);
//------------------------------------------->

//GET FILE INFO
char get_file_type(mode_t mode);
char * get_file_permissions(mode_t mode);
char * get_owner_by_id(uid_t uid, char ftype);
char * get_group_by_id(uid_t gid);
char * get_time_str(time_t t);
t_file_info * get_file_data(struct stat * stats, char * path);
t_list * read_files_fro_dir(char * path, int mode);
char ** get_eattrs(char * path);
char * get_major(t_file_info * file);
char * get_minor(t_file_info * file);
//------------------------------------------->

//FILE INFO OUTPUT
void print_fname_simple(t_file_info * file);
void print_fname_colorful(t_file_info * file);
void print_multicol(t_list * names, void (*print_name)(t_file_info * file));
//void print_extneded_attrs(t_file_info * fi);
void print_fullf_info(t_list * info, t_fname_print_func print_fname, int * params);
//------------------------------------------->

//PROCESSING
void handle(t_list ** files, char * flags);
//------------------------------------------->

#endif


