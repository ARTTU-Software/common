/*
 * STM32G4 flash back-end: 2 KB pages (4 KB on dual-bank-capable parts in
 * single-bank mode), 64-bit (double-word) programming, ART instruction/data
 * cache in FLASH->ACR. G431 (gateways, GI_V1) is single bank.
 */

#include "ota_stm32.h"

#if defined(OTA_STM32_G4)

#include <string.h>

#define OTA_G4_PROGRAM_UNIT  8U

#if defined(FLASH_OPTR_DBANK)
/* Category 3 parts (G47x/G48x) only; the G431 is always single bank. */
static bool g4_dual_bank(void)
{
    return (FLASH->OPTR & FLASH_OPTR_DBANK) != 0U;
}
#endif

uint32_t ota_stm32_flash_erase_unit(void)
{
#if defined(FLASH_OPTR_DBANK)
    return g4_dual_bank() ? 0x800U : 0x1000U;
#else
    return FLASH_PAGE_SIZE;
#endif
}

/* HAL page erase flushes the caches itself; programming does not, so the verify read could hit stale lines. */
static void g4_flush_caches(void)
{
    if ((FLASH->ACR & FLASH_ACR_ICEN) != 0U) {
        __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
        __HAL_FLASH_INSTRUCTION_CACHE_RESET();
        __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    }
    if ((FLASH->ACR & FLASH_ACR_DCEN) != 0U) {
        __HAL_FLASH_DATA_CACHE_DISABLE();
        __HAL_FLASH_DATA_CACHE_RESET();
        __HAL_FLASH_DATA_CACHE_ENABLE();
    }
}

bool ota_stm32_flash_erase(uint32_t addr)
{
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t page_error = 0U;
    uint32_t offset = addr - FLASH_BASE;
    uint32_t unit = ota_stm32_flash_erase_unit();
    HAL_StatusTypeDef st;

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Banks     = FLASH_BANK_1;
    erase.Page      = offset / unit;
    erase.NbPages   = 1U;
#if defined(FLASH_OPTR_DBANK)
    if (g4_dual_bank()) {
        uint32_t bank_size = FLASH_SIZE / 2U;
        erase.Banks = (offset < bank_size) ? FLASH_BANK_1 : FLASH_BANK_2;
        erase.Page  = (offset % bank_size) / unit;
    }
#endif

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS); /* stale flags (e.g. after a debug session) fail the next op */
    st = HAL_FLASHEx_Erase(&erase, &page_error);
    HAL_FLASH_Lock();

    return st == HAL_OK;
}

bool ota_stm32_flash_program(uint32_t addr, const uint8_t *data, uint32_t len)
{
    HAL_StatusTypeDef st = HAL_OK;

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    for (uint32_t off = 0U; (off < len) && (st == HAL_OK); off += OTA_G4_PROGRAM_UNIT) {
        uint64_t dword;

        memcpy(&dword, &data[off], sizeof(dword)); /* G4 HAL takes the value, not a pointer */
        st = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + off, dword);
    }
    HAL_FLASH_Lock();
    g4_flush_caches();

    return st == HAL_OK;
}

uint32_t ota_stm32_flash_end(void)          { return FLASH_BASE + FLASH_SIZE; } /* FLASH_SIZE reads the size register */
uint32_t ota_stm32_flash_program_unit(void) { return OTA_G4_PROGRAM_UNIT; }

#endif /* OTA_STM32_G4 */
