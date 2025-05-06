#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

uint32_t ipv4_to_int(const char *ip_str) {
    unsigned int a, b, c, d;
    sscanf(ip_str, "%u.%u.%u.%u", &a, &b, &c, &d);
    return (a << 24) | (b << 16) | (c << 8) | d;
}

void int_to_ipv4(uint32_t ip, char *buffer) {
    sprintf(buffer, "%u.%u.%u.%u",
            (ip >> 24) & 0xFF,
            (ip >> 16) & 0xFF,
            (ip >> 8) & 0xFF,
            ip & 0xFF);
}

uint32_t prefix_to_mask(int prefix) {
    if (prefix == 0)
        return 0;
    return (0xFFFFFFFF << (32 - prefix)) & 0xFFFFFFFF;
}

typedef struct {
    int hosts_required;
    int new_prefix;      
    uint32_t block_size; 
    uint32_t network_address; 
    uint32_t subnet_mask;     
} SubnetInfo;

int compare_desc(const void *a, const void *b) {
    SubnetInfo *sub1 = (SubnetInfo *)a;
    SubnetInfo *sub2 = (SubnetInfo *)b;
    return sub2->hosts_required - sub1->hosts_required;
}

int main(void) {
    char base_ip_str[16];
    int base_prefix;
    int num_subnets;
    
    printf("Enter base IPv4 address (e.g., 192.168.1.0): ");
    scanf("%15s", base_ip_str);
    
    printf("Enter base network prefix length (e.g., 24): ");
    scanf("%d", &base_prefix);
    
    printf("Enter number of subnets: ");
    scanf("%d", &num_subnets);
    
    SubnetInfo *subnets = malloc(sizeof(SubnetInfo) * num_subnets);
    if (!subnets) {
        fprintf(stderr, "Memory allocation error!\n");
        return 1;
    }
    
    for (int i = 0; i < num_subnets; i++) {
        printf("Enter number of hosts for subnet %d: ", i + 1);
        scanf("%d", &subnets[i].hosts_required);
    }
    
    qsort(subnets, num_subnets, sizeof(SubnetInfo), compare_desc);
    
    uint32_t base_ip = ipv4_to_int(base_ip_str);
    uint32_t base_mask = prefix_to_mask(base_prefix);
    uint32_t current_network = base_ip & base_mask;
    
    for (int i = 0; i < num_subnets; i++) {
        int total_needed = subnets[i].hosts_required + 2;
        int host_bits = (int)ceil(log2(total_needed));
        subnets[i].block_size = 1 << host_bits;      
        subnets[i].new_prefix = 32 - host_bits;      
        subnets[i].subnet_mask = prefix_to_mask(subnets[i].new_prefix);
        subnets[i].network_address = current_network;  
        
        current_network += subnets[i].block_size;
    }
    
    printf("----------------------------------------------------------------\n");
    printf("| Hosts Required |   Network Address   |      Subnet Mask      |\n");
    printf("----------------------------------------------------------------\n");
    char network_str[16], mask_str[16];
    for (int i = 0; i < num_subnets; i++) {
        int_to_ipv4(subnets[i].network_address, network_str);
        int_to_ipv4(subnets[i].subnet_mask, mask_str);
        printf("| %-14d | %-19s | %-21s |\n",
               subnets[i].hosts_required,
               network_str,
               mask_str);
    }
    printf("-----------------------------------------------------------------------------\n");
    
    free(subnets);
    return 0;
}