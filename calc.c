#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

// Convert IPv4 dotted-decimal string to a 32-bit integer.
uint32_t ipv4_to_int(const char *ip_str) {
    unsigned int a, b, c, d;
    sscanf(ip_str, "%u.%u.%u.%u", &a, &b, &c, &d);
    uint32_t ip = ((a & 0xFF) << 24) |
                  ((b & 0xFF) << 16) |
                  ((c & 0xFF) << 8)  |
                  (d & 0xFF);
    return ip;
}

// Convert a 32-bit IP integer into dotted-decimal format.
void int_to_ipv4(uint32_t ip, char *buffer) {
    sprintf(buffer, "%u.%u.%u.%u",
            (ip >> 24) & 0xFF,
            (ip >> 16) & 0xFF,
            (ip >> 8) & 0xFF,
            ip & 0xFF);
}

// Generate a mask value (32-bit integer) based on the prefix length.
uint32_t prefix_to_mask(int prefix) {
    if (prefix == 0)
        return 0;
    return (uint32_t)(0xFFFFFFFF << (32 - prefix));
}

int main() {
    char base_ip_str[16];
    int base_prefix;
    int num_subnets;

    // Input base network address and prefix (e.g., 192.168.1.0 and 24)
    printf("Enter base IPv4 address (e.g., 192.168.1.0): ");
    scanf("%15s", base_ip_str);

    printf("Enter base network prefix length (e.g., 24): ");
    scanf("%d", &base_prefix);

    // Input the number of subnets.
    printf("Enter number of subnets: ");
    scanf("%d", &num_subnets);

    // Allocate array to hold the number of hosts required for each subnet.
    int *hosts = malloc(sizeof(int) * num_subnets);
    if (!hosts) {
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }

    for (int i = 0; i < num_subnets; i++) {
        printf("Enter number of hosts for subnet %d: ", i + 1);
        scanf("%d", &hosts[i]);
    }

    // Determine the starting network address by applying the base mask.
    uint32_t base_ip = ipv4_to_int(base_ip_str);
    uint32_t base_mask = prefix_to_mask(base_prefix);
    uint32_t current_network = base_ip & base_mask;

    // Print header for the output table.
    printf("\nVLSM Calculation Table:\n");
    printf("---------------------------------------------------------\n");
    printf("| Subnet | Hosts Required | Network Address | Subnet Mask |\n");
    printf("---------------------------------------------------------\n");

    for (int i = 0; i < num_subnets; i++) {
        int required = hosts[i];
        /* 
         * For each subnet, we need a block that can accommodate:
         * required hosts + 2 (one for network and one for broadcast).
         */
        int total_needed = required + 2;
        // Find the minimum number of host bits necessary.
        int host_bits = (int)ceil(log2(total_needed));
        // Block size in terms of number of addresses.
        int block_size = 1 << host_bits;
        // The new subnet mask has (32 - host_bits) bits for the network.
        int subnet_prefix = 32 - host_bits;

        uint32_t subnet_mask = prefix_to_mask(subnet_prefix);

        char network_str[16];
        char mask_str[16];
        int_to_ipv4(current_network, network_str);
        int_to_ipv4(subnet_mask, mask_str);

        // Display the calculated subnet
        printf("| %-6d | %-14d | %-15s | %-11s |\n", i + 1, required, network_str, mask_str);

        // Move to the next available network block.
        current_network += block_size;
    }
    printf("---------------------------------------------------------\n");

    free(hosts);
    return 0;
}