/*
 * STM32U5 flash back-end: 8 KB pages, two banks (1 MB each on the U575CI),
 * 128-bit (quad-word) programming with ECC, ICACHE peripheral. TrustZone must
 * be disabled (TZEN=0): the HAL then uses the non-secure flash registers and
 * the app runs at 0x08xxxxxx.
 *
 * Same scheme as the H5 back-end, with pages instead of sectors.
 */

#include "ota_stm32.h"

#if defined(OTA_STM32_U5)

#define OTA_U5_PROGRAM_UNIT  16U

static void u5_icache_invalidate(void)
{
    if ((ICACHE->CR & ICACHE_CR_EN) != 0U) {
        ICACHE->CR |= ICACHE_CR_CACHEINV;
        while ((ICACHE->SR & ICACHE_SR_BUSYF) != 0U) {
        }
    }
}

bool ota_stm32_flash_erase(uint32_t addr)
{
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t page_error = 0U;
    uint32_t offset = addr - FLASH_BASE;
    uint32_t bank = (offset < FLASH_BANK_SIZE) ? FLASH_BANK_1 : FLASH_BANK_2;
    HAL_StatusTypeDef st;

    /* With SWAP_BANK set, the bank mapped at FLASH_BASE is physically bank 2. */
    if ((FLASH->OPTR & FLASH_OPTR_SWAP_BANK) != 0U) {
        bank = (bank == FLASH_BANK_1) ? FLASH_BANK_2 : FLASH_BANK_1;
    }

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Banks     = bank;
    erase.Page      = (offset % FLASH_BANK_SIZE) / FLASH_PAGE_SIZE;
    erase.NbPages   = 1U;

    HAL_FLASH_Unlock();
#if defined(FLASH_FLAG_ALL_ERRORS)
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
#endif
    st = HAL_FLASHEx_Erase(&erase, &page_error);
    HAL_FLASH_Lock();
    u5_icache_invalidate();

    return st == HAL_OK;
}

bool ota_stm32_flash_program(uint32_t addr, const uint8_t *data, uint32_t len)
{
    HAL_StatusTypeDef st = HAL_OK;

    HAL_FLASH_Unlock();
#if defined(FLASH_FLAG_ALL_ERRORS)
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
#endif
    for (uint32_t off = 0U; (off < len) && (st == HAL_OK); off += OTA_U5_PROGRAM_UNIT) {
        /* U5 HAL takes the address of the 16 source bytes, like H5 */
        st = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, addr + off, (uint32_t)(data + off));
    }
    HAL_FLASH_Lock();
    u5_icache_invalidate();

    return st == HAL_OK;
}

uint32_t ota_stm32_flash_end(void)          { return FLASH_BASE + FLASH_SIZE; } /* FLASH_SIZE reads the size register */
uint32_t ota_stm32_flash_erase_unit(void)   { return FLASH_PAGE_SIZE; }
uint32_t ota_stm32_flash_program_unit(void) { return OTA_U5_PROGRAM_UNIT; }

#endif /* OTA_STM32_U5 */
