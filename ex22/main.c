#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cpio_t {
    char magic[6];
    char ino[8];
    char mode[8];
    char uid[8];
    char gid[8];
    char nlink[8];
    char mtime[8];
    char filesize[8];
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8];
    char check[8];
};

/**
 * @brief Convert a hexadecimal string to integer
 *
 * @param s hexadecimal string
 * @param n length of the string
 * @return integer value
 */
static int hextoi(const char* s, int n) {
    int r = 0;
    while (n-- > 0) {
        r = r << 4;
        if (*s >= 'A')
            r += *s++ - 'A' + 10;
        else if (*s >= 0)
            r += *s++ - '0';
    }
    return r;
}

/**
 * @brief Align a number to the nearest multiple of a given number
 *
 * @param n number
 * @param byte alignment
 * @return aligned number
 */
static unsigned long align(unsigned long n, int byte) {
    return (n + byte - 1) & ~(byte - 1);
}

void initrd_list(const void* rd) {
    // TODO: Implement this function
    struct cpio_t* cur = (struct cpio_t*)rd;
    unsigned int namesize, filesize;
    while(1) {
        namesize = hextoi( cur->namesize, 8 );
        filesize = hextoi( cur->filesize, 8 );
    
        if(!strcmp("TRAILER!!!", (char*)cur+sizeof(struct cpio_t))) break;

        printf("%8ld %s\n", filesize, (char*)cur+sizeof(struct cpio_t));

        cur = (struct cpio_t*)align(align((unsigned long)cur+sizeof(struct cpio_t)+namesize, 4)+filesize, 4);
    }
}

void initrd_cat(const void* rd, const char* filename) {
    // TODO: Implement this function
    struct cpio_t* cur = (struct cpio_t*)rd;
    unsigned int namesize, filesize;
    while(1) {
        namesize = hextoi( cur->namesize, 8 );
        filesize = hextoi( cur->filesize, 8 );
    
        if(!strcmp("TRAILER!!!", (char*)cur+sizeof(struct cpio_t))) break;

        // printf("%8ld %s\n", filesize, (char*)cur+sizeof(struct cpio_t));
        if(!strcmp(filename, (char*)cur+sizeof(struct cpio_t))){
            char* file = (char*)align((unsigned long)cur+sizeof(struct cpio_t)+namesize, 4);
            printf("%s\n", file);
            return;
        }

        cur = (struct cpio_t*)align(align((unsigned long)cur+sizeof(struct cpio_t)+namesize, 4)+filesize, 4);
    }

    printf("initrd_cat: %s: No such file\n", filename);
}

int main() {
    /* Prepare the initial RAM disk */
    FILE* fp = fopen("initramfs.cpio", "rb");
    if (!fp) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    void* rd = malloc(sz);
    fseek(fp, 0, SEEK_SET);
    if (fread(rd, 1, sz, fp) != sz) {
        fprintf(stderr, "Failed to read the device tree blob\n");
        free(rd);
        fclose(fp);
        return EXIT_FAILURE;
    }
    fclose(fp);

    initrd_list(rd);
    initrd_cat(rd, "osc.txt");
    initrd_cat(rd, "test.txt");
    // initrd_cat(rd, "penguin.txt");
    
    free(rd);
    return 0;
}
