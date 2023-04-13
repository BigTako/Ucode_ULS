#include "../inc/header.h"

t_filetree_node *new_node(void * data)
{
    t_filetree_node *temp = (t_filetree_node *)malloc(sizeof(t_filetree_node));
    temp->data = data;
    temp->left = temp->right = NULL;
    return temp;
}

void display(t_filetree_node *root) 
{
    if (root != NULL)
    {
        t_file_info * fi = (t_file_info *)(root->data);
        display(root->left);
		printf("%lld -> size\n", fi->size);
        display(root->right);
    }
}

t_filetree_node * insert(t_filetree_node** node, comparing_func foo, bool reversed, void * data) 
{
    t_filetree_node * New_node = new_node(data);
    
    if (*node == NULL){ 
        *node = New_node;
        return New_node;
    }
    int comp_res = foo(New_node, (*node));
	bool insert_condition = false;
	if (reversed){
		comp_res = -comp_res;
		insert_condition = (comp_res >= 0);
	}
	else
		insert_condition = (comp_res > 0);

	if (insert_condition)
	{
		(*node)->left = insert(&((*node)->left), foo, reversed, data);
	}
	else
	{
		(*node)->right = insert(&((*node)->right), foo, reversed, data);
	}
	
    free(New_node);
    return *node;
}

long long compare_by_size(t_filetree_node * a, t_filetree_node * b)
{
	t_file_info * f1 = (t_file_info *)(a->data);
	t_file_info * f2 = (t_file_info *)(b->data);
	return f1->size - f2->size;
}

long long compare_by_path(t_filetree_node * a, t_filetree_node * b)
{
	t_file_info * f1 = (t_file_info *)(a->data);
	t_file_info * f2 = (t_file_info *)(b->data);
	return -mx_strcmp(f1->path, f2->path);
}

long long compare_by_mtime(t_filetree_node * a, t_filetree_node * b)
{
	t_file_info * f1 = (t_file_info *)(a->data);
	t_file_info * f2 = (t_file_info *)(b->data);
	return f1->mtime - f2->mtime;
}

long long compare_by_atime(t_filetree_node * a, t_filetree_node * b)
{
	t_file_info * f1 = (t_file_info *)(a->data);
	t_file_info * f2 = (t_file_info *)(b->data);
	return f1->atime - f2->atime;
}

long long compare_by_ctime(t_filetree_node * a, t_filetree_node * b)
{
	t_file_info * f1 = (t_file_info *)(a->data);
	t_file_info * f2 = (t_file_info *)(b->data);
	return f1->ctime - f2->ctime;
}

void smart_insert(t_filetree_node ** tree, char * flags, void * data)
{
	bool reversed_sort = mx_get_char_index(flags, 'r') >= 0;
	if (mx_get_char_index(flags, 't') >= 0)
	{
		if (mx_get_char_index(flags, 'u') >= 0){ //-u
			insert(tree, compare_by_atime, reversed_sort, data);
		}
		else if (mx_get_char_index(flags, 'c') >= 0){  //-c
			insert(tree, compare_by_ctime, reversed_sort, data);
		}
		else{
			insert(tree, compare_by_mtime, reversed_sort, data);
		}
	}
	else if (mx_get_char_index(flags, 'S') >= 0)
	{
		//t_file_info * info = (t_file_info *)data;
		insert(tree, compare_by_size, reversed_sort, data);
	}
	else{
		insert(tree, compare_by_path, reversed_sort, data);
	}
}

void tree_files_classificate(t_list ** files, t_list ** dirs, t_filetree_node * node)
{
	if (node)
	{
		tree_files_classificate(files, dirs, node->left);
		t_file_info * fi = (t_file_info *)(node->data);
		if (fi->type == '-'){ // valid file
			mx_push_back(files, fi);
		}
		else if (fi->type == 'd'){ // valid directory
			mx_push_back(dirs, fi);
		}
		tree_files_classificate(files, dirs, node->right);
	}
}

void delete_tree(t_filetree_node ** leaf, void(*free_content)(void * ptr))
{
    if( *leaf != NULL ) {
        delete_tree(&(*leaf)->left, free_content);
        if (free_content)
		{
			free_content((*leaf)->data);
		}
		delete_tree(&(*leaf)->right, free_content);
		free( (*leaf));
    }
	*leaf = NULL;
}

t_file_info * copy_finfo(t_file_info * template)
{
	if (template)
	{
		t_file_info * elem = malloc(sizeof(struct s_file_info));
		elem->type = template->type;
		elem->permissions = mx_strdup(template->permissions);
		elem->link_count = template->link_count;
		elem->owner_name = mx_strdup(template->owner_name);
		elem->group_name = mx_strdup(template->group_name);
		elem->size = template->size;
		elem->bsize = template->bsize;
		elem->mtime = template->mtime;
		elem->atime = template->atime;
		elem->ctime = template->ctime;
		if (template->type == 'c' || template->type == 'b')
			elem->dev_id = template->dev_id;
		elem->path = mx_strdup(template->path);
		return elem;
	}
	return NULL;
}

void tree_to_list(t_filetree_node * root, t_list ** output, char copy)
{
	if (!root) return;
	tree_to_list(root->left, output, copy);
	switch(copy)
	{
		case 'f':
			mx_push_back(output, copy_finfo((t_file_info *)(root->data)));
			break;
		case 's':
			mx_push_back(output, mx_strdup(root->data));
			break;
		case '0':
			mx_push_back(output, root->data);
			break;
	}
	tree_to_list(root->right, output, copy);
}

void list_to_tree(t_list * list,t_filetree_node ** tree, char * flags , char copy)
{
	if (!list) return;
	for (t_list * i = list; i; i = i->next)
	{
		switch(copy)
		{
			case 'f':
				smart_insert(tree, flags, copy_finfo((t_file_info *)(i->data)));	
				break;
			case 's':
				smart_insert(tree, flags, mx_strdup(i->data));	
				break;
			case '0':
				smart_insert(tree, flags, i->data);
				break;
		}
	}	
}



