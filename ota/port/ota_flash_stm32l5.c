/*
 * STM32L5 flash back-end: dual bank (DBANK=1, default) with 2 KB pages, or
 * single bank (DBANK=0) with 4 KB pages; 64-bit (double-word) programming;
 * ICACHE peripheral. TrustZone must be disabled (TZEN=0): the HAL then uses
 * the non-secure flash registers and the app runs at 0x08xxxxxx.
 */

#include "ota_stm32.h"

#if defined(OTA_STM32_L5)

#include <string.h>

#define OTA_L5_PROGRAM_UNIT  8U

static bool l5_dual_bank(void)
{
    return (FLASH->OPTR & FLASH_OPTR_DBANK) != 0U;
}

static void l5_icache_invalidate(void)
{
    if ((ICACHE->CR & ICACHE_CR_EN) != 0U) {
        ICACHE->CR |= ICACHE_CR_CACHEINV;
        while ((ICACHE->SR & ICACHE_SR_BUSYF) != 0U) {
        }
    }
}

uint32_t ota_stm32_flash_erase_unit(void)
{
    return l5_dual_bank() ? FLASH_PAGE_SIZE : FLASH_PAGE_SIZE_128_BITS;
}

bool ota_stm32_flash_erase(uint32_t addr)
{
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t page_error = 0U;
    uint32_t offset = addr - FLASH_BASE;
    HAL_StatusTypeDef st;

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.NbPages   = 1U;
    if (l5_dual_bank()) {
        erase.Banks = (offset < FLASH_BANK_SIZE) ? FLASH_BANK_1 : FLASH_BANK_2;
        erase.Page  = (offset % FLASH_BANK_SIZE) / FLASH_PAGE_SIZE;
        /* With SWAP_BANK set, the bank mapped at FLASH_BASE is physically bank 2. */
        if ((FLASH->OPTR & FLASH_OPTR_SWAP_BANK) != 0U) {
            erase.Banks = (erase.Banks == FLASH_BANK_1) ? FLASH_BANK_2 : FLASH_BANK_1;
        }
    } else {
        erase.Banks = FLASH_BANK_1;
        erase.Page  = offset / FLASH_PAGE_SIZE_128_BITS;
    }

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    st = HAL_FLASHEx_Erase(&erase, &page_error);
    HAL_FLASH_Lock();
    l5_icache_invalidate();

    return st == HAL_OK;
}

bool ota_stm32_flash_program(uint32_t addr, const uint8_t *data, uint32_t len)
{
    HAL_StatusTypeDef st = HAL_OK;

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    for (uint32_t off = 0U; (off < len) && (st == HAL_OK); off += OTA_L5_PROGRAM_UNIT) {
        uint64_t dword;

        memcpy(&dword, &data[off], sizeof(dword)); /* L5 HAL takes the value, not a pointer */
        st = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + off, dword);
    }
    HAL_FLASH_Lock();
    l5_icache_invalidate();

    return st == HAL_OK;
}

uint32_t ota_stm32_flash_end(void)          { return FLASH_BASE + FLASH_SIZE; } /* FLASH_SIZE reads the size register */
uint32_t ota_stm32_flash_program_unit(void) { return OTA_L5_PROGRAM_UNIT; }

#endif /* OTA_STM32_L5 */
