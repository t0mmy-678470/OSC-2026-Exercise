// #include <stdio.h>
// #include <string.h>

// int main(){
//     char* s = "hello";
//     printf("%d\n", strlen(s));
// }

// #include <stdio.h>

// typedef struct list_head {
//     struct list_head* prev;
//     struct list_head* next;
// }LIST_HEAD;

// typedef struct page {
//     int order;
//     int refcount;
//     int refcount2;
//     char f[3];
//     struct list_head list;
// } PAGE;

// #define LIST_ENTRY(ptr, type, memb)  ((type*)((unsigned long)ptr-(unsigned long)&(((type*)0)->memb)))

// int main() {
//     PAGE p = {1,2,3,'c', 'a', '\0', {(struct list_head*)0x8080, (struct list_head*)0x9090}};
//     printf("%p: %d %d %d %s %p %p\n", &p, p.order, p.refcount, p.refcount2, p.f, p.list.next, p.list.prev);
//     printf("%p\n",LIST_ENTRY(&p.list, PAGE, list));
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* a1 = (char*) malloc(sizeof(char) * 0x10000);
    strcpy(a1, "AAAAAAA");
    char* a2 = (char*) malloc(sizeof(char) * 0x10000);
    strcpy(a2, "BBBBBBBBBBBBBBB");
    char* a3 = (char*) malloc(sizeof(char) * 0x10000);
    strcpy(a3, "CCCCCCCCCCCCCCCCCCCCCCC");

    free(a2);
    free(a1);
    free(a3);

    return 0;
}