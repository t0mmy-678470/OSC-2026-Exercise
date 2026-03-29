#include <iostream>
#include <list>
#include <vector>

#define NUM_PAGES 0x280000
#define MAX_ORDER 10

struct page {
    int order = 0;
    int refcount = 0; // in use or not
};

std::vector<page> mem_map;
std::vector<std::list<page*>> free_area;


struct page* get_buddy(struct page* page, unsigned int order) {
    return &mem_map[(page - mem_map.data()) ^ (1 << order)];
}

unsigned int get_page_idx(struct page* page) {
    return (unsigned int)(page - mem_map.data());
}

struct page* alloc_pages(unsigned int order) {
    // TODO: Implement this function
    struct page* alloced_pages, * tmp_pages, * buddy_page;
    for(int i=order;i<=MAX_ORDER;i++) {
        if(free_area[i].size()>0) {
            int tmp_i=i;
            while(tmp_i>order) {
                // get large chunk
                tmp_pages = free_area[tmp_i].front();
                free_area[tmp_i].pop_front();
                // todo ---v
                // devide by 2
                tmp_i--;
                buddy_page = get_buddy(tmp_pages, tmp_i);
                buddy_page->order = tmp_i; // freed page
                buddy_page->refcount = 0;
                free_area[tmp_i].push_front( buddy_page );
                free_area[tmp_i].push_front( tmp_pages );
                
            }
            alloced_pages = free_area[order].front();
            alloced_pages->order = order;
            alloced_pages->refcount = 1;
            free_area[order].pop_front();
            return alloced_pages;
        }
    }
    return NULL;
}

void free_pages(struct page* page) {
    // TODO: Implement this function
    // 
    if(!page || page->refcount==0) return;
    page->refcount = 0;

    struct page* buddy_page, * tmp_page;
    while (page->order < MAX_ORDER) {
        buddy_page = get_buddy(page, page->order);
        if(buddy_page->order != page->order || buddy_page->refcount!=0) {
            break;
        }
        // remove buddy page from free list
        free_area[page->order].remove(buddy_page);

        // get lower address page
        if((unsigned long)page > (unsigned long)buddy_page) {
            page = buddy_page;
        }

        page->order++;
    }
    free_area[page->order].push_front(page);
}

void dump() {
    for (int i = MAX_ORDER; i >= 0; i--)
        std::cout << "free_area[" << i << "] " << free_area[i].size()
                  << std::endl;
}

int main() {
    mem_map.resize(NUM_PAGES);
    free_area.resize(MAX_ORDER + 1);
    for (size_t i = 0; i < NUM_PAGES; i += (1 << MAX_ORDER)) {
        mem_map[i].order = MAX_ORDER;
        free_area[MAX_ORDER].push_back(&mem_map[i]);
    }
    // dump();
    std::cout << "\np1:\n";
    struct page* p1 = alloc_pages(1);
    dump();

    std::cout << "\np2:\n";
    struct page* p2 = alloc_pages(1);
    dump();

    std::cout << "\np3:\n";
    struct page* p3 = alloc_pages(1);
    dump();

    free_pages(p1);
    free_pages(p2);
    free_pages(p3);

    std::cout << "\nfree:\n";
    dump();
    return 0;
}
