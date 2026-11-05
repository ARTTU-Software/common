/* STM32H5 flash back-end: 8 KB sectors, two banks, 128-bit (quad-word) programming. */

#include "ota_stm32.h"

#if defined(OTA_STM32_H5)

#define OTA_H5_PROGRAM_UNIT  16U

static void h5_icache_invalidate(void)
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
    uint32_t sector_error = 0U;
    uint32_t offset = addr - FLASH_BASE;
    uint32_t bank = (offset < FLASH_BANK_SIZE) ? FLASH_BANK_1 : FLASH_BANK_2;
    HAL_StatusTypeDef st;

    /* With SWAP_BANK set, the bank mapped at FLASH_BASE is physically bank 2. */
    if ((FLASH->OPTSR_CUR & FLASH_OPTSR_SWAP_BANK) != 0U) {
        bank = (bank == FLASH_BANK_1) ? FLASH_BANK_2 : FLASH_BANK_1;
    }

    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Banks     = bank;
    erase.Sector    = (offset % FLASH_BANK_SIZE) / FLASH_SECTOR_SIZE;
    erase.NbSectors = 1U;

    HAL_FLASH_Unlock();
    st = HAL_FLASHEx_Erase(&erase, &sector_error);
    HAL_FLASH_Lock();
    h5_icache_invalidate();

    return st == HAL_OK;
}

bool ota_stm32_flash_program(uint32_t addr, const uint8_t *data, uint32_t len)
{
    HAL_StatusTypeDef st = HAL_OK;

    HAL_FLASH_Unlock();
    for (uint32_t off = 0U; (off < len) && (st == HAL_OK); off += OTA_H5_PROGRAM_UNIT) {
        st = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, addr + off, (uint32_t)(data + off));
    }
    HAL_FLASH_Lock();
    h5_icache_invalidate();

    return st == HAL_OK;
}

uint32_t ota_stm32_flash_end(void)        { return FLASH_BASE + FLASH_SIZE; } /* FLASH_SIZE reads the size register */
uint32_t ota_stm32_flash_erase_unit(void) { return FLASH_SECTOR_SIZE; }
uint32_t ota_stm32_flash_program_unit(void) { return OTA_H5_PROGRAM_UNIT; }

#endif /* OTA_STM32_H5 */
