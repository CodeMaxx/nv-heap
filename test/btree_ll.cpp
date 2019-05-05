#include <iostream>
#include "../include/nvheap.h"
using namespace std;

struct LL {
    int64_t val;
    NVPtr next;
};

struct BT {
    int val;
    NVPtr left, right;
};

struct ROOT {
    NVPtr ll;
    NVPtr bt;
};

// Will be called when NVH root is null. Create a NVH root and ll = bt = Null
struct ROOT * create_root() {
    ROOT * root = (struct ROOT *)nvh_malloc (sizeof(struct ROOT));
    if (root) {
        root -> ll = NULL;
        root -> bt = NULL;
    }
    nvh_set_root(root);
    return root;
}

LL * ll_insert (struct LL * head, int64_t val) {
    struct LL * temp, *temp1;
    if (!head) {
        head = (struct LL *) nvh_malloc(sizeof(struct LL));
        tx_add_direct(head, sizeof(struct LL));
        head -> val = val;
        head -> next = -1;
    }
    else {
        temp = head;
        while (temp->next.dptr()) {
            temp = (struct LL *) temp->next.dptr();
        }
        tx_add_direct(temp, sizeof(struct LL));
        temp1 = (struct LL *)nvh_malloc (sizeof(struct LL));
        temp -> next = (void *)temp1;
        temp1 -> val = val;
        temp1 -> next = -1;
    }
    // nvh_set_root(head);
    return head;
}

LL * ll_remove (struct LL * head, int val) {
    struct LL *temp, *curr, *next;
    if (!head)
        return head;
    if (head -> val == val) {
        temp = (struct LL *) head->next.dptr();
        nvh_free (head, sizeof(struct LL));
        tx_add_direct(head, sizeof(LL));            //Abhik
        head = temp;
    }
    else {
        curr = head;
        next = (struct LL *) curr->next.dptr();
        while (next) {
            if (next -> val == val) {
                temp = next;
                next = (struct LL *) next->next.dptr();
                tx_add_direct(next, sizeof(LL));    // Abhik
                curr -> next = next;
                nvh_free(temp, sizeof(struct LL));
                break;
            }
            curr = next;
            next = (struct LL *) next->next.dptr();
        }
    }
    // nvh_set_root(head);
    return head;
}

void ll_print (struct LL * head) {
    struct LL * temp = head;
    while (temp) {
        cout << temp -> val << "->";
        temp = (struct LL *) temp->next.dptr();
    }
    cout << "NULL" << endl;
}

void bt_inorder(struct BT* root)
{
    if (root != NULL)
    {
        bt_inorder((struct BT *) root->left.dptr());
        cout << root->val << " ";
        bt_inorder((struct BT *) root->right.dptr());
    }
}

/* A utility function to insert a new node with given key in BST */
BT* bt_insert(struct BT* node, int key)
{
    /* If the tree is empty, return a new node */
    // No need to store trx of this part, bcz all allocation will be undone
    if (node == NULL) {
        struct BT *temp =  (struct BT *)nvh_malloc(sizeof(struct BT));
        temp->val = key;
        temp->left = NULL;
        temp->right = NULL;
        node = temp;
    }
    else {
        /* Otherwise, recur down the tree */
        tx_add_direct(node, sizeof(BT));        // Abhik
        if (key < node->val)
            node->left  = bt_insert((struct BT *) node->left.dptr(), key);
        else if (key > node->val)
            node->right = bt_insert((struct BT *) node->right.dptr(), key);
    }
    // nvh_set_root(node);
    /* return the (unchanged) node pointer */
    return node;
}


BT * inPre (BT *root) 
{
    if (!root || !root -> left.dptr())
        return NULL;
    BT * temp = (BT *)root -> left.dptr();
    while (temp -> right.dptr())
        temp = (BT *)temp -> right.dptr();
    return temp;
}


BT * bt_delete (BT *root, int key)
{
    BT * temp;
    if(!root)
        return root;

    if ( root -> val > key )
    {
        root ->left = (void *)bt_delete ((BT *)(root -> left.dptr()), key);
    } 

    else if ( root -> val < key )
    {
        root -> right = (void *)bt_delete ((BT *)(root -> right.dptr()), key);
    }
    
    else if (root -> left.dptr() && root -> right.dptr())
    {
        temp = inPre (root);
        root -> val = temp -> val;
        root -> left = (void *)bt_delete((BT *)(root -> left.dptr()), root -> val);
    } 

    else if (root -> right.dptr()) 
    {
        temp = root;
        root = (BT *)temp -> right.dptr();
        nvh_free (temp, sizeof(BT));
    }
        
    else if (root -> left.dptr()){
        temp = root;
        root = (BT *)root -> left.dptr();
        nvh_free(temp, sizeof(BT));
    }

    else {
        nvh_free(root, sizeof(BT));
        root = NULL;     
    }
    return root; 
}


int main (int argc, char * argv[]) {

    ios_base::sync_with_stdio(false);

    if (nvh_init(argv[1], argv[2]) < 0) {
        cerr << "Failed to initialize NV-heap\n";
        exit(1);
    }

    struct ROOT *root;
    if (!(root = (struct ROOT *) nvh_get_root())) {
        root = create_root ();
    }

    if (!root) {
        cerr << "Root is Null. Exiting\n" ;
    }

    // Getting dereferencable head for Linked list and Bin Tree
    struct LL *ll = (struct LL *)root -> ll.dptr();
    struct BT *bt = (struct BT *)root -> bt.dptr();

    char option, type;
    int val, run = 1;
    cout << "Type: l for linked list, b for binary search tree\n";
    cout << "Command: Insert (Front for ll): i val, Print (in order for BT): p, Delete val: d val, Quit: q\n";
    cout << "Usage: Type Command e.g. b i 5 = insert 5 in binary tree\n";
    while (run) {
        cin >> type;

        // When the type is linked list
        if (type == 'l' || type == 'L') 
        {
            cin >> option;
            switch (option)
            {
                case ('q'):
                case ('Q'):
                run = 0;
                break;
                case ('i'):     //Insert
                case ('I'):
                cin >> val;
                tx_begin();         // Abhik, removed from function added here
                tx_add_direct(ll, sizeof(LL));  //Abhik
                ll = ll_insert (ll, val);
                tx_add_direct(root, sizeof(ROOT));
                root -> ll = (void *)ll; // NOTE: nvh_set_root is unnecesseary
                tx_commit();        // Abhik
                break;
                case ('p'):
                case ('P'):
                ll_print (ll);
                break;
                case ('d'):
                case ('D'):
                cin >> val;
                tx_begin();         //Abhik
                ll = ll_remove (ll, val);
                root -> ll = (void *)ll; // NOTE: nvh_set_root is unnecesseary
                tx_commit();        //Abhik
                break;
            }
        }

        // For binary tree
        else if (type == 'b' || type == 'B')
        {
            cin >> option;
            switch (option)
            {
                case ('q'):
                case ('Q'):
                run = 0;
                break;
                case ('i'):     //Insert
                case ('I'):
                cin >> val;
                tx_begin();
                tx_add_direct(bt, sizeof(BT));      //Abhik
                bt = bt_insert (bt, val);
                tx_add_direct(root, sizeof(ROOT));  // Abhik
                root -> bt = (void *)bt;
                tx_commit();
                cout << "Inserted " << val << endl;
                break;
                case ('p'):
                case ('P'):
                bt_inorder(bt); cout << endl;
                break;
                case ('d'): case('D'):
                cin >> val;
                tx_begin();
                tx_add_direct(bt, sizeof(BT));  // Abhik
                bt = bt_delete (bt, val);
                tx_add_direct(root, sizeof(ROOT));  //Abhik
                root -> bt = (void *)bt;
                tx_commit();
                break;
            }
        }
        else if (type == 'q' || type == 'Q')
            run = 0;
        else
            cout << "Input invalid. Try again\n";
    }
    nvh_close ();
    return 0;
}
