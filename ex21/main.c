#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE   0x00000002
#define FDT_PROP       0x00000003
#define FDT_NOP        0x00000004
#define FDT_END        0x00000009

struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

static inline uint32_t bswap32(uint32_t x) {
    return __builtin_bswap32(x);
}

static inline uint64_t bswap64(uint64_t x) {
    return __builtin_bswap64(x);
}

static inline const void* align_up(const void* ptr, size_t align) {
    return (const void*)(((uintptr_t)ptr + align - 1) & ~(align - 1));
}

int strncmp2(char* node_name, char* path, int len) {
    int i=0;
    int same = 1;
    int have_at = 0;
    while(1) {
        if(path[i] == '@') {
            have_at = 1;
            break;
        }
        if(path[i] == '\0') {
            break;
        }
        i++;
    }
    
    for(int i=0;i<len;i++){
        if(node_name[i] != path[i]) {
            if(!have_at){
                if(node_name[i]=='@' && (path[i]=='/' || path[i]=='\0')) {
                    break;
                }
            }
            same = 0;
            break;
        }
    }
    return same;
}

int fdt_path_offset(const void* fdt, const char* path) {
    // TODO: Implement this function
    // path層數       當前traverse層數    比較到的層數
    int path_depth=0, cur_depth=-2, i=0, matching_depth=0;
    int path_idx[10]={0};
    unsigned char* ptr_align;
    while(*(path+i) != '\0') {
        if(*(path+i) == '/') {
            path_idx[path_depth++] = i+1;
        }
        i++;
    }
    uint32_t cur_off = bswap32(((struct fdt_header*)fdt)->off_dt_struct);
    uint32_t str_off = bswap32(((struct fdt_header*)fdt)->off_dt_strings);
    
    while(1) {
        switch(bswap32(*(uint32_t*)((unsigned char*)fdt + cur_off)))
        {
        case FDT_BEGIN_NODE:
            cur_depth+=1;
            char* node_name = (char*)fdt+cur_off+4;
            // check in correspond depth dir/dev
            if(cur_depth == matching_depth){
                // check the same dir/dev name
                if(strncmp2(node_name, path+path_idx[cur_depth], strlen(node_name))) {
                    if (++matching_depth == path_depth) {
                        return cur_off;
                    }
                }
            }
            // update cur_off
            ptr_align = align_up((unsigned char*)fdt+cur_off+4+strlen(node_name)+1, 4);
            cur_off = (unsigned long long)ptr_align - (unsigned long long)fdt;
            break;
        case FDT_END_NODE:
            cur_depth--;
            // device not found
            if( cur_depth+1 < matching_depth ){
                printf("device not found!\n");
                return -1;
            }
            cur_off += 4;
            break;
        case FDT_PROP:
            uint32_t value_len = bswap32(*(uint32_t*)((char*)fdt+cur_off+4));
            // update cur_off
            ptr_align = align_up((unsigned char*)fdt+cur_off+0xc+value_len, 4);
            cur_off = (unsigned long long)ptr_align - (unsigned long long)fdt;
            break;
        case FDT_NOP:
            cur_off += 4;
            break;
        case FDT_END:
            // shouldn't be here?
            return -1;
            break;
        default:
            return -1;
            break;
        }
    }
}

const void* fdt_getprop(const void* fdt,
                        int nodeoffset,
                        const char* name,
                        int* lenp) {
    // TODO: Implement this function
    uint32_t cur_off = nodeoffset;
    uint32_t str_off = bswap32(((struct fdt_header*)fdt)->off_dt_strings);
    uint32_t value_len;
    uint32_t name_off;
    unsigned char* ptr_align;
    char* node_name = (char*)fdt + cur_off + 4;
    ptr_align = align_up((unsigned char*)fdt+cur_off+4+strlen(node_name)+1, 4);
    cur_off = (unsigned long long)ptr_align - (unsigned long long)fdt;

    while (bswap32(*(uint32_t*)((char*)fdt+cur_off)) == FDT_PROP){
        value_len = bswap32(*(uint32_t*)((char*)fdt+cur_off+4));
        name_off = bswap32(*(uint32_t*)((char*)fdt+cur_off+8));

        if(!strcmp((char*)fdt+str_off+name_off, name)){
            *lenp = value_len;
            return (char*)fdt+cur_off+0xc;
        }

        ptr_align = align_up( (char*)fdt+cur_off+0xc+value_len, 4 );
        cur_off = (unsigned long long)ptr_align - (unsigned long long)fdt;
    }
    
    return NULL;
}

int main() {
    /* Prepare the device tree blob */
    FILE* fp = fopen("qemu.dtb", "rb");
    if (!fp) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    void* fdt = malloc(sz);
    fseek(fp, 0, SEEK_SET);
    if (fread(fdt, 1, sz, fp) != sz) {
        fprintf(stderr, "Failed to read the device tree blob\n");
        free(fdt);
        fclose(fp);
        return EXIT_FAILURE;
    }
    fclose(fp);

    /* Find the node offset */
    int offset = fdt_path_offset(fdt, "/cpus/cpu@0/interrupt-controller");
    // printf("offset = 0x%x\n", offset);
    // return 0;
    if (offset < 0) {
        fprintf(stderr, "fdt_path_offset\n");
        free(fdt);
        return EXIT_FAILURE;
    }

    /* Get the node property */
    int len;
    const void* prop = fdt_getprop(fdt, offset, "compatible", &len);
    if (!prop) {
        fprintf(stderr, "fdt_getprop\n");
        free(fdt);
        return EXIT_FAILURE;
    }
    printf("compatible: %.*s\n", len, (const char*)prop);

    offset = fdt_path_offset(fdt, "/memory"); // ?
    prop = fdt_getprop(fdt, offset, "reg", &len);
    const uint64_t* reg = (const uint64_t*)prop;
    printf("memory: base=0x%lx size=0x%lx\n", bswap64(reg[0]), bswap64(reg[1]));

    offset = fdt_path_offset(fdt, "/chosen");
    prop = fdt_getprop(fdt, offset, "linux,initrd-start", &len);
    const uint64_t* initrd_start = (const uint64_t*)prop;
    printf("initrd-start: 0x%lx\n", bswap64(initrd_start[0]));

    free(fdt);
    return 0;
}
