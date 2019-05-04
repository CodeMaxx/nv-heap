#include <iostream>
#include <nvheap.h>
#include <nvptr.h>
#include <nvhtx.h>

using namespace std;

struct node {
    int key;
    NVPtr left, right;
};

// A utility function to create a new BST node
struct node *newNode(int item)
{
    struct node *temp =  (struct node *)nvh_malloc(sizeof(struct node));
    temp->key = item;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

// A utility function to do inorder traversal of BST
void inorder(struct node *root)
{
    if (root != NULL)
    {
        inorder(root->left.dptr());
        cout << root->key << endl;
        inorder(root->right.dptr());
    }
}

/* A utility function to insert a new node with given key in BST */
struct node* insert(struct node* node, int key)
{
    /* If the tree is empty, return a new node */
    if (node == NULL) return newNode(key);

    /* Otherwise, recur down the tree */
    if (key < node->key)
        node->left  = insert(node->left.dptr(), key);
    else if (key > node->key)
        node->right = insert(node->right.dptr(), key);

    /* return the (unchanged) node pointer */
    return node;
}

// Driver Program to test above functions
int main()
{
    ios_base::sync_with_stdio(false);
    if (nvh_init(argv[1], argv[2]) < 0) {
        cerr << "Failed to initialize NV-heap\n";
        exit(1);
    }

    /* Let us create following BST
              50
           /     \
          30      70
         /  \    /  \
       20   40  60   80 */
    struct node *root = (struct ll *) nvh_get_root();
    root = insert(root, 50);
    insert(root, 30);
    insert(root, 20);
    insert(root, 40);
    insert(root, 70);
    insert(root, 60);
    insert(root, 80);

    // print inoder traversal of the BST
    inorder(root);

    return 0;
}
