#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PAGE_SIZE 4096

#define PGD_SHIFT 39
#define PUD_SHIFT 30
#define PMD_SHIFT 21
#define PTE_SHIFT 12
#define INDEX_MASK 0x1FF  

void walk_page_table(int pagemap_fd, uint64_t start_va, uint64_t end_va) {
    int last_pgd = -1;
    int last_pud = -1;
    int last_pmd = -1;

    for (uint64_t va = start_va; va < end_va; va += PAGE_SIZE) {
        uint64_t offset = (va / PAGE_SIZE) * sizeof(uint64_t);
        uint64_t pagemap_entry;

        if (lseek(pagemap_fd, offset, SEEK_SET) == -1) continue;
        if (read(pagemap_fd, &pagemap_entry, sizeof(pagemap_entry)) != sizeof(pagemap_entry)) continue;

        if (!(pagemap_entry & (1ULL << 63))) {
            continue;
        }

        uint64_t pfn = pagemap_entry & ((1ULL << 55) - 1);

        int pgd_idx = (va >> PGD_SHIFT) & INDEX_MASK;
        int pud_idx = (va >> PUD_SHIFT) & INDEX_MASK;
        int pmd_idx = (va >> PMD_SHIFT) & INDEX_MASK;
        int pte_idx = (va >> PTE_SHIFT) & INDEX_MASK;

        if (pgd_idx != last_pgd) {
            printf("PGD [%3d] (VA Base: 0x%012lx)\n", pgd_idx, (va & ~((1ULL << PGD_SHIFT) - 1)));
            last_pgd = pgd_idx;
            last_pud = -1; 
            last_pmd = -1;
        }
        if (pud_idx != last_pud) {
            printf("  ├── PUD [%3d]\n", pud_idx);
            last_pud = pud_idx;
            last_pmd = -1;
        }
        if (pmd_idx != last_pmd) {
            printf("  │   ├── PMD [%3d]\n", pmd_idx);
            last_pmd = pmd_idx;
        }
        
        printf("  │   │   └── PTE [%3d] ──> PFN: 0x%lx\n", pte_idx, pfn);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Invalid input. Correct: %s <PID>\n", argv[0]);
        return 1;
    }

    char maps_path[256];
    char pagemap_path[256];
    snprintf(maps_path, sizeof(maps_path), "/proc/%s/maps", argv[1]);
    snprintf(pagemap_path, sizeof(pagemap_path), "/proc/%s/pagemap", argv[1]);

    FILE *maps_file = fopen(maps_path, "r");
    if (!maps_file) {
        perror("Invalid PID");
        return 1;
    }

    int pagemap_fd = open(pagemap_path, O_RDONLY);
    if (pagemap_fd < 0) {
        perror("Permission denied, try with sudo");
        fclose(maps_file);
        return 1;
    }

    printf("=== Tree of page table for PID %s ===\n", argv[1]);

    char line[512];
    while (fgets(line, sizeof(line), maps_file)) {
        uint64_t start, end;
        if (sscanf(line, "%lx-%lx", &start, &end) == 2) {
            walk_page_table(pagemap_fd, start, end);
        }
    }

    fclose(maps_file);
    close(pagemap_fd);
    return 0;
}