#ifndef DEFINITION_H
#define DEFINITION_H

#define NUM_MEASUREMENTS 150



// Define FIXED_VALUES for reproducibility
#define FIXED_VALUES
#define LOG_MAIN
//#define LOG_SPREF

//#define RESET_NVS 
//#define INIT_SPREF
//#define GEN_PROCEDURE
//#define REP_PROCEDURE

#define PUF_RESPONSE_BYTES 64
#define RTC_FAST_MEM_START 0x500010CC
#define RTC_FAST_MEM_SIZE  0x50001FFF - RTC_FAST_MEM_START + 1  
#define PUF_RESPONSE_BITS (PUF_RESPONSE_BYTES * 8)


// #define PUF_RESPONSE_BYTES 32
// #define RTC_FAST_MEM_START 0x500010CC
// #define RTC_FAST_MEM_SIZE  0x50001FFF - RTC_FAST_MEM_START + 1  
// #define PUF_RESPONSE_BITS (PUF_RESPONSE_BYTES * 8)


#define RANDOM_POOL_BITS 32  // Size of B
#define LOCKS_PER_BIT 8  // Number of locks per bit in B
#define BITS_PER_XOR  5 // Number of bits to use in each XOR
#define K_SIZE 32  // Size of the key
#define KEY_SIZE 32     // Size of the key in bytes
#define KEY_SIZE_BIT KEY_SIZE*8     // Size of the key in bytes



#endif