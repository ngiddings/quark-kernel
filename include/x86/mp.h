#include <stdint.h>

#define MP_SIGNATURE (uint32_t) 0x5F504D5F

struct mp_floating_pointer
{
    /**
     * @brief The ASCII string "_MP_" indicates the presence of the pointer structure.
     * 
     */
    uint32_t signature;

    /**
     * @brief The physical address of the MP configuration table. All zeroes if
     * the table does not exist.
     * 
     */
    uint32_t physical_addr;

    /**
     * @brief The length of the floating pointer structure in 16-byte units.
     * Should contain the value '0x01'.
     * 
     */
    uint8_t length;

    /**
     * @brief The version number of the MP specification supported.
     * 
     */
    uint8_t spec_rev;

    /**
     * @brief A checksum of the pointer structure.
     * 
     */
    uint8_t checksum;

    /**
     * @brief When zero, the configuration table is present. Otherwise, the
     * value indicates a default configuration.
     * 
     */
    uint8_t config_type;

    uint8_t undef : 7;

    /**
     * @brief 
     * 
     */
    uint8_t imcrp : 1;
} __attribute__ ((packed));