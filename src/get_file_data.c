#include "../inc/header.h"

char get_file_type(mode_t mode)
{
	switch (mode & S_IFMT) 
	{
        case S_IFBLK:  
        	return 'b';
        case S_IFCHR:   
        	return 'c';
        case S_IFDIR:  
        	return 'd';
        case S_IFIFO:  
        	return 'p';
        case S_IFLNK:  
        	return 'l';
        case S_IFREG:  
        	return '-';
        case S_IFSOCK: 
        	return 's';
        default:
            break;
    }
    return '+';
}

static void check_chmod(int corresponds, char nonxchar, char xchar, char * actual)
{
	if (corresponds)
	{
		if (*actual == '-') // non executable
		{
			*actual = nonxchar;
		}
		else
		{
			*actual = xchar;
		}
	}
}

char * get_file_permissions(mode_t mode)
{
	char * result = mx_strnew(10);
	// owner permissions
	int i = 0;
	if (mode & S_IRUSR) result[i] = 'r';
	else result[i] = '-';
	i++;
	if (mode & S_IWUSR) result[i] = 'w';
	else result[i] = '-';
	i++;
	if (mode & S_IXUSR) result[i] = 'x';
	else result[i] = '-';
	i++;
	// group members permissions
	if (mode & S_IRGRP) result[i] = 'r';
	else result[i] = '-';
	i++;
	if (mode & S_IWGRP) result[i] = 'w';
	else result[i] = '-';
	i++;
	if (mode & S_IXGRP) result[i] = 'x';
	else result[i] = '-';
	i++;
	// other user permissions
	if (mode & S_IROTH) result[i] = 'r';
	else result[i] = '-';
	i++;
	if (mode & S_IWOTH) result[i] = 'w';
	else result[i] = '-';
	i++;
	if (mode & S_IXOTH) result[i] = 'x';
	else result[i] = '-';
	//result[9] = '\0';
	check_chmod(S_ISUID & mode, 'S', 's', result + 2);
	check_chmod(S_ISGID & mode, 'S', 's', result + 5);
	check_chmod(S_ISVTX & mode, 'T', 't', result + 8);

	return result;
}

char * get_owner_by_id(uid_t uid, char ftype)
{
	ftype = 0;
	errno = 0;
	struct passwd * pws;
	pws = getpwuid(uid);
	
	if (pws)
	{
		return mx_strdup(pws->pw_name);
	}
	else
	{	
		return mx_itoa(uid);
	}
}

char * get_group_by_id(uid_t gid)
{
	errno = 0;
	struct group * g;
	g = getgrgid(gid);
	if (g)
	{
		return mx_strdup(g->gr_name);
	}
	else
	{
		return mx_itoa(gid);
	}	
}

char * get_time_str(time_t t){
	char * result;
	time_t curtime = time(NULL);
	char * time_str = ctime(&t);
   	time_str[mx_strlen(time_str)-1] = '\0';
   	char ** parts = mx_strsplit(time_str, ' ');
	parts[3][5] = '\0'; // delete seconds from time
	
	result = mx_strnew(12);
	result = mx_strcat(result, parts[1]);
	if (mx_strlen(parts[2]) == 2)
		result = mx_strcat(mx_strcat(result, " "), parts[2]);// DAY < 10
	else
		result = mx_strcat(mx_strcat(result, "  "), parts[2]); // DAY > 10
	
	if (curtime - t > 6*2629743) // format MON DAY YEAR
	{
		result = mx_strcat(mx_strcat(result, "  "), parts[4]);
	}
	else // format MON DAY TIME
	{
		result = mx_strcat(mx_strcat(result, " "), parts[3]);
	}
	//mx_del_strarr(&parts);
	//free(time_str);
	return result;
}

t_file_info * get_file_data(struct stat * stats, char * path)
{
	if (!stats || !path) return NULL;
	t_file_info * elem = malloc(sizeof(struct s_file_info));
	elem->type = get_file_type(stats->st_mode);
	elem->permissions = get_file_permissions(stats->st_mode);
	elem->link_count = stats->st_nlink;
	elem->owner_name = get_owner_by_id(stats->st_uid, elem->type);
	elem->group_name = get_group_by_id(stats->st_gid);
	elem->size = stats->st_size;
	elem->bsize = stats->st_blocks;
	elem->mtime = stats->st_mtime;
	elem->atime = stats->st_atime;
	elem->ctime = stats->st_ctime;
	if (elem->type == 'c' 
	 || elem->type == 'b')
	{
		elem->dev_id = stats->st_rdev;
	}
	elem->path = path;
	return elem;
}

static char * relate_path(char * path, char * name)
{
	if (path && name)
	{
		char * file_path = mx_strnew(mx_strlen(path) + mx_strlen(name) + 1);
		file_path = mx_strcpy(file_path, path);
		file_path[mx_strlen(file_path)] = '/';
		mx_strcpy(file_path + mx_strlen(path) + 1, name);
		return file_path;
	}
	return NULL;
}

t_list * read_files_fro_dir(char * path, int mode)
{ // reads all filenames from folder and returns a list containing them
	if (!path) return NULL;
	struct stat stats;
	t_list * names = NULL;
	DIR * dir = opendir(path);
	for (struct dirent * temp = readdir(dir); temp ; temp = readdir(dir))
	{
		char * name = temp->d_name;
		char * file_path = relate_path(path, name);
		if (!file_path) continue;
		lstat(file_path, &stats);
		if (mode == USUAL && name[0] != '.'){
			mx_push_back(&names, get_file_data(&stats, file_path));
		}
		else if (mode == MODEA && 
				mx_strcmp(name, ".") && 
				mx_strcmp(name, ".."))
		{
			mx_push_back(&names, get_file_data(&stats, file_path));
		}
		else if (mode == ALL)
		{
			mx_push_back(&names, get_file_data(&stats, file_path));
		}
		else
		{
			free(file_path);
		}
	}
	closedir(dir);
	return names;
}	

t_filetree_node * read_files_fro_dir_tree(char * path, char * flags, int mode)
{ // reads all filenames from folder and returns a list containing them
	if (!path) return NULL;
	struct stat stats;
	t_filetree_node * names = NULL;
	DIR * dir = opendir(path);
	for (struct dirent * temp = readdir(dir); temp ; temp = readdir(dir))
	{
		char * name = temp->d_name;
		char * file_path = relate_path(path, name);
		lstat(file_path, &stats);
		if (mode == USUAL && name[0] != '.')
		{
			smart_insert(&names, flags, get_file_data(&stats, file_path));
		}
		else if (mode == MODEA && 
				mx_strcmp(name, ".") && 
				mx_strcmp(name, ".."))
		{
			smart_insert(&names, flags, get_file_data(&stats, file_path));
		}
		else if (mode == ALL)
		{
			smart_insert(&names, flags, get_file_data(&stats, file_path));
		}
		else
		{
			free(file_path);
		}
	}
	closedir(dir);
	return names;
}

char ** get_eattrs(char * path)
{
	char buf[1024];
	int size = listxattr(path, buf, 1024, XATTR_NOFOLLOW);
	int i = 0;
	while (i < size - 1)
	{
		if (buf[i] == '\0')
			buf[i] = '\n';
		i++;
	}
	return mx_strsplit(buf, '\n');
}

static char * minor_to_hex(char * minor)
{
	char * h_minor = mx_strdup("0x00000000");
	mx_strcpy(h_minor + (mx_strlen(h_minor) - mx_strlen(minor)), minor);
	return h_minor;
}

char * get_major(t_file_info * file) {
	if (file->dev_id)
	{
		return mx_itoa((int)(((unsigned int)file->dev_id >> 24) & 0xff));
	}
	return malloc(sizeof(char *));
}

char * get_minor(t_file_info * file) {
	if (file->dev_id)
	{
		int minor_num = (int)(file->dev_id & 0xffffff);
    	char *minor = NULL;
    	if (minor_num > 255)
    	{
    		char * hex_minor = mx_nbr_to_hex(minor_num);	
    		minor = minor_to_hex(hex_minor);
    		free(hex_minor);
    	}
    	else
    	{
    		minor = mx_itoa(minor_num);
    	}
    	return minor;
    }	
    return malloc(sizeof(char *));
}



