#include <nvheap.h>
#include <nvptr.h>
#include <nvhtx.h>

using namespace std;

struct ll {
    int64_t val;
    NVPtr next;
};

struct ll * insert (struct ll * head, int64_t val) {
    struct ll * temp, *temp1;
    // cout << "Insert: strted\n";
    if (!head) {
        // cout << "Insert: null head\n";
        head = (struct ll *)nvh_malloc (sizeof(struct ll));
        head -> val = val;
        head -> next = -1;
    }
    else {
        // cout << "Insert: valid head\n";
        temp = head;
        while (temp->next.dptr()) {
            temp = (struct ll *) temp->next.dptr();
        }
        temp1 = (struct ll *)nvh_malloc (sizeof(struct ll));
        temp1 -> val = val;
        temp1 -> next = -1;
        temp -> next = (void *)temp1;
    }
    nvh_set_root(head);
    // cout << nvh_get_root() << endl;
    // cout << "Insert: done\n";
    return head;
}

struct ll * remove (struct ll * head, int val) {
    struct ll *temp, *curr, *next;
    if (!head)
        return head;
    if (head -> val == val) {
        temp = (struct ll *) head->next.dptr();
        nvh_free (head, sizeof(struct ll));
        head = temp;
    }
    else {
        curr = head;
        next = (struct ll *) curr->next.dptr();
        while (next) {
            if (next -> val == val) {
                temp = next;
                next = (struct ll *) next->next.dptr();
                curr -> next = next;
                nvh_free(temp, sizeof(struct ll *));
                break;
            }
            curr = next;
            next = (struct ll *) next->next.dptr();
        }
    }
    nvh_set_root(head);
    return head;
}

void print (struct ll * head) {
    struct ll * temp = head;
    // cout << "In print \n";
    while (temp) {
        // cout << "print: In while";
        cout << temp -> val << "->";
        temp = (struct ll *) temp->next.dptr();
    }
    cout << "NULL" << endl;
}

int main (int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    if (nvh_init(argv[1], argv[2]) < 0) {
        cerr << "Failed to initialize NV-heap\n";
        exit(1);
    }
    struct ll * head;
    head = (struct ll *) nvh_get_root();
    char option;
    int val, run = 1;
    cout << "Insert Front: i val, Print: p, Delete val: d val, Quit: q\n";
    while (run) {
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
            head = insert (head, val);
            break;
            case ('p'):
            case ('P'):
            print (head);
            break;
            case ('d'):
            case ('D'):
            cin >> val;
            head = remove (head, val);
            break;
        }
    }
    // Enable this to check Pointer safety
    // cout << "Main: Calling lower abort\n";
    // head -> next = nvh_pptr((void *)100);
    nvh_close ();
    return 0;
}
