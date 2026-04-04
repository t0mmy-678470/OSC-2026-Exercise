#include <iostream>
#include <list>
#include <vector>

#define PAGE_SIZE (1UL << 12)
#define NUM_PAGES 0x280000
#define MAX_ORDER 10

typedef unsigned long phys_addr_t;

struct page {
    int order = 0;
    int refcount = 0;
};

std::vector<page> mem_map;
std::vector<std::list<page*>> free_area;

struct page* get_buddy(struct page* page, unsigned int order) {
    return &mem_map[(page - mem_map.data()) ^ (1 << order)];
}

unsigned int get_pfn(struct page* page) {
    return (unsigned int)(page - mem_map.data());
}

void dump() {
    for (int i = MAX_ORDER; i >= 0; i--)
        std::cout << "free_area[" << i << "] " << free_area[i].size()
                  << std::endl;
}

void memory_reserve(phys_addr_t base, size_t size) {
    // TODO: Implement this function
    std::cout << "[Reserve] Reserve address [ "<<base<<", 0x"<<std::hex<<size<<"). Range of pages: ["<<base/PAGE_SIZE<<", "<<(base+size)/PAGE_SIZE<<")" << std::endl;

    unsigned int start_pfn = base/PAGE_SIZE;
    unsigned int end_pfn = (base+size-1)/PAGE_SIZE; // include
    int order = MAX_ORDER;
    while(1) {
        unsigned int page_order_head_pfn = start_pfn - (start_pfn%(1<<order));
        int found = 0;
        // search all free area for this order
        for(auto cur = free_area[order].begin() ; cur != free_area[order].end() ; cur++) {
            // found in free list
            if (*cur == &mem_map[page_order_head_pfn]) {
                struct page* cur_page = *cur;
                struct page* buddy_page;
                found = 1;
                // full overlap
                if (page_order_head_pfn == start_pfn && end_pfn >= (page_order_head_pfn+(1<<order)-1)) {
                    (cur_page)->refcount = 1;
                    (cur_page)->order = order;
                    free_area[order].remove(cur_page);

                    start_pfn = page_order_head_pfn+(1<<order);
                    // check finish
                    if(start_pfn > end_pfn) {
                        break;
                    }
                    // check still in same MAX_ORDER region
                    int offset = start_pfn % (1<<MAX_ORDER);
                    int base = 0;
                    int next_order=MAX_ORDER-1;
                    if (offset) {
                        // compute new order
                        while(1) {
                            // find new order
                            if ( offset == base+(1<<next_order) ) {
                                break;
                            }
                            else if (offset > base+(1<<next_order)) {
                                base += (1<<next_order);
                            }
                            next_order--;
                            if(next_order<0) {
                                std::cout<<"ERROR ORDER"<<std::endl;
                                exit(0);
                            }
                        }
                    }
                    else {
                        next_order = MAX_ORDER;
                    }
                    // if remain same order, continue the for loop
                    if(order == next_order) {
                        continue;
                    }
                }
                // partail overlap
                // 先 free 掉然後丟到下一個 order 的 free list
                else if(page_order_head_pfn < start_pfn || end_pfn < (page_order_head_pfn+(1<<order)-1)) {
                    free_area[order].remove(cur_page);
                    (cur_page)->order = order - 1;
                    buddy_page = get_buddy(cur_page, order-1);
                    buddy_page->order = order - 1;
                    free_area[order-1].push_front(buddy_page);
                    free_area[order-1].push_front(cur_page);
                    order--;
                }

                break;
            }
        }
        // finished !!!
        if(start_pfn > end_pfn) {
            break;
        }
        // shouldn't be here
        if(!found) {
            std::cout << "\nNot found!" << std::endl;
            std::cout << "order = " << order << std::endl;
            std::cout << "start_pfn = 0x" << std::hex << start_pfn << std::endl;
            std::cout << "end_pfn = 0x" << std::hex << end_pfn << std::endl;
            std::cout << "page_order_head_pfn = 0x" << std::hex << page_order_head_pfn << std::endl;
            exit(0);
        }
    }
}

void mm_init() {
    mem_map.resize(NUM_PAGES);
    free_area.resize(MAX_ORDER + 1);
    for (size_t i = 0; i < NUM_PAGES; i += (1 << MAX_ORDER)) {
        mem_map[i].order = MAX_ORDER;
        free_area[MAX_ORDER].push_back(&mem_map[i]);
    }
    dump();
    memory_reserve(0, 0x82a69510);
}

int main() {
    mm_init();
    dump();
    return 0;
}
