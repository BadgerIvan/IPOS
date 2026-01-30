#ifndef _PCI_H
#define _PCI_H

#include <stdint.h>
#include <lib/list.h>

#define PCI_MAX_BUSES 256
#define PCI_MAX_DEVICES 32
#define PCI_MAX_FUNCS 8

#define PCI_HEADERTYPE_NORMAL 0
#define PCI_HEADERTYPE_BRIDGE 1
#define PCI_HEADERTYPE_CARDBUS 2
#define PCI_HEADERTYPE_MULTIFUNC 0x80

typedef union {
	struct {
		uint16_t vendor;
		uint16_t device;
		uint16_t command_reg;
		uint16_t status_reg;
		uint8_t revision;
		uint8_t progIF;
		uint8_t subclass;
		uint8_t class;
		uint8_t cacheline_size;
		uint8_t latency;
		uint8_t header_type;
		uint8_t bist;
		uint32_t bar0;
		uint32_t bar1;
		uint32_t bar2;
		uint32_t bar3;
		uint32_t bar4;
		uint32_t bar5;
		uint32_t cardbus_cis_pointer;
		uint16_t subsystem_vendor;
		uint16_t subsystem_device;
		uint32_t expansion_rom_base_address;
		uint8_t capabilities_pointer;
		uint8_t reserved[7];
		uint8_t int_line;
		uint8_t int_pin;
		uint8_t min_grant;
		uint8_t max_latency;
	} __attribute__((packed)) bits;
	uint32_t header[16];
} __attribute__((packed)) pci_header_t;

int pci_read_header(uint32_t bus, uint32_t device, uint32_t func, pci_header_t* header);

node_t* pci_scan_all();

#endif