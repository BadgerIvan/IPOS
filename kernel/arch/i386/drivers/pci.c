#include <arch/drivers/pci.h>
#include <arch/drivers/io.h>
#include <kernel/heap.h>
#include <debug/debug.h>
#include <stddef.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_NONE 0xFFFF
#define PCI_BROKEN_NONE 0x0000

typedef union {
	struct {
		uint32_t zero 		: 2;
		uint32_t reg_num     : 6;
		uint32_t func_num    : 3;
		uint32_t dev_num     : 5;
		uint32_t bus_num     : 8;
		uint32_t reserved    : 7;
		uint32_t enable_bit  : 1;
	} bits;
	uint32_t val;
} pci_config_addr_t;

static node_t* pci_headers = NULL;

void pci_read_config(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg, uint32_t* data) {
    if(bus >= PCI_MAX_BUSES || device >= PCI_MAX_DEVICES || func >= PCI_MAX_DEVICES || data == NULL)
        return;
    pci_config_addr_t addr;
    addr.val = 0;
    addr.bits.reg_num = reg;
    addr.bits.func_num = func;
    addr.bits.dev_num = device;
    addr.bits.bus_num = bus;
    addr.bits.enable_bit = 1;
    outl(PCI_CONFIG_ADDRESS, addr.val);
    io_wait();
    *data = inl(PCI_CONFIG_DATA);
}

int pci_read_header(uint32_t bus, uint32_t device, uint32_t func, pci_header_t* header) {
    if(bus >= PCI_MAX_BUSES || device >= PCI_MAX_DEVICES || func >= PCI_MAX_FUNCS || header == NULL)
        return -1;
    __builtin_memset(header, 0, sizeof(pci_header_t));
    for(int i = 0; i < sizeof(header->header)/sizeof(header->header[0]); i++) {
        pci_read_config(bus, device, func, i, &header->header[i]);
    }
    if(header->bits.vendor == PCI_NONE || header->bits.vendor == PCI_BROKEN_NONE) {
        __builtin_memset(header, 0, sizeof(pci_header_t));
        return -1;
    }
    return 0;
}

void pci_free_list(node_t** head) {
    node_t* current = *head;
    node_t* next;
    while(current != NULL) {
        next = current->next;
        free(current->data);
        current->data = NULL;
        current = next;
    }
    free_list(head);
}

node_t* pci_scan_all() {
    if(pci_headers != NULL)
        return pci_headers;
    node_t* head = NULL;
    pci_header_t* header;
    for(int i = 0; i < PCI_MAX_BUSES; i++) {
        for(int j = 0; j < PCI_MAX_DEVICES; j++) {
            header = malloc(sizeof(pci_header_t));
            if(header == NULL) {
                pci_free_list(&head);
                return NULL;
            }
            if(pci_read_header(i, j, 0, header) == -1) {
                free(header);
                continue;
            }
            push_back(&head, header);
            if(header->bits.header_type & PCI_HEADERTYPE_MULTIFUNC) {
                for(int k = 1; k < PCI_MAX_FUNCS; k++) {
                    pci_header_t* func_header = malloc(sizeof(pci_header_t));
                    if(func_header == NULL) {
                        pci_free_list(&head);
                        return NULL;
                    }
                    if(pci_read_header(i, j, k, func_header) == -1) {
                        free(func_header);
                        continue;
                    }
                    push_back(&head, func_header);
                }
            }
        }
    }
    pci_headers = head;
    return head;
}