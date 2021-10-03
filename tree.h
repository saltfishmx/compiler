#ifndef tree
#define tree
#define max_child_num 10;
typedef struct _Node{
    int childnum;
    int lineno;
    int type;
    char *name;
    union Val{
        int i;
        float f;
        char *s;
    }val;
    struct _Node* childlist[10];
}Node;
struct _Node* root;
struct _Node* malloc_node(char *name,int type,void* val,int line);
void *add(Node *father,Node *son);
void preorder(Node *root,int space_num);
void print_space(int n);
#endif