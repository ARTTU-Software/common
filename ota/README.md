# OTA over CAN

Firmware update for every STM32 node over the car's CAN bus, through a small
bootloader in the first 24 KB of flash. One driver for all boards: the core is
HAL-free, and a single STM32 port covers the G4, H5 and L5 families (picked
automatically from the HAL on the include path).

```
ota/
├── ota_protocol.h          CAN IDs, opcodes, status codes, node IDs (DBC names)
├── ota_app.c/.h            app side: answers PING, handles the 0x4F0 enter request (HAL-free)
├── ota_boot.c/.h           bootloader core: session state machine, START/COMMIT records (HAL-free)
├── ota_crc32.c/.h          CRC-32, same as Python zlib.crc32
├── ota.cmake               ota_add_app(<target>) / ota_add_bootloader(<target>)
├── port/
│   ├── ota_stm32.h         public port API + family detection
│   ├── ota_app_stm32.c     app glue: FDCAN send, boot flag, reset
│   ├── ota_bootloader_stm32.c   bootloader runner: filters, listen window, jump
│   ├── ota_stm32_common.c  shared FDCAN + TAMP backup-register helpers
│   └── ota_flash_stm32{g4,h5,l5}.c   flash back-ends (only the matching one compiles)
├── boards/ota_board_*.h    memory map + node ID per board (shared by app and bootloader)
└── tools/
    ├── ota_flash.py        host CLI (python-can): scan, info, enter, flash, reset
    └── test/               simulator: real ota_flash.py against real ota_boot.c (host gcc)
```

## Boards

| Board | Header | MCU | Bootloader | Meta | App | Status |
|---|---|---|---|---|---|---|
| ECU | `ota_board_ecu.h` | H563xG 1 MB | 24 KB | `0x0800_6000` | `0x0800_8000`, 992 KB | **tested on hardware** |
| Dashboard | `ota_board_dash.h` | H563RG 1 MB | 24 KB | `0x0800_6000` | `0x0800_8000`, 976 KB | header only; FDCAN2; verify flash-config area |
| Telemetry | `ota_board_telem.h` | H563RG 1 MB | 24 KB | `0x0800_6000` | `0x0800_8000`, 992 KB | header only |
| Gateway F/R | `ota_board_gateway.h` | G431CB 128 KB | 24 KB | `0x0800_6000` | `0x0800_6800`, 100 KB | port compiles + links (8.5 KB) |
| GI L/R | `ota_board_gi.h` | L562CE 512 KB | 24 KB | `0x0800_6000` | `0x0800_7000`, 484 KB | port compiles + links (9.7 KB) |
| LV SoC | `ota_board_lvsoc.h` | L5 | 24 KB | `0x0800_6000` | `0x0800_7000` | header only; verify NVM/RAM |

Node IDs follow the DBC: ECU=1, GF=2, GR=3, GIL=4, GIR=5, DASH=6, TELEM=7, LVSOC=8.
The port reads the erase/program unit from the part and clamps the app end to
the real flash size. If a board's addresses don't fit the part's erase unit,
the bootloader still boots the existing app but refuses to write flash.

## Adding OTA to a board

### 1. Application (5 small changes)

1. **Linker script**: `FLASH ORIGIN` = the board's app start, `LENGTH` = app size (table above).
2. **Vector table**: first thing in `main()` (USER CODE 1):
   ```c
   extern uint32_t g_pfnVectors[];
   SCB->VTOR = (uint32_t)g_pfnVectors;
   ```
3. **CAN filter**: let `0x4F0` and `OTA_ID_CMD(node)` (`0x6N0`) into the RX FIFO. Raise
   *Std Filters Nbr* in the `.ioc`, not only in `fdcan.c`.
4. **Init + dispatch**:
   ```c
   #include "ota_stm32.h"
   #include "ota_board_ecu.h"            /* your board's header */

   ota_app_stm32_init(&hfdcan1, OTA_BOARD_NODE_ID, my_is_safe_fn);   /* after the CAN init */

   /* in the RX dispatcher (main loop / CAN task, never an ISR): */
   case OTA_ID_ENTER_REQUEST:
   case OTA_ID_CMD(OTA_BOARD_NODE_ID):
       (void)ota_app_handle_frame(id, payload, len);
       break;
   ```
   `my_is_safe_fn` returns false when the node must not reboot now (e.g. tractive system on).
5. **CMake**:
   ```cmake
   include(${CMAKE_CURRENT_LIST_DIR}/Drivers/common/ota/ota.cmake)
   ota_add_app(${CMAKE_PROJECT_NAME})
   add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
       COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin)
   ```

### 2. Bootloader project (once per board)

1. In STM32CubeMX open the board's `.ioc`, **Save Project As** `<repo>/Bootloader/`.
2. Keep RCC (same clock tree), SYS/DEBUG, the board's CAN FDCAN instance (same bitrate,
   **Std Filters Nbr = 2**) and an LED pin. Disable everything else. Leave other outputs
   at reset/low: that is the node's safe state while it is in the bootloader.
3. Generate (CMake toolchain). Then:
   - linker: `FLASH ORIGIN = 0x08000000, LENGTH = 24K`
   - `CMakeLists.txt`:
     ```cmake
     include(${CMAKE_CURRENT_LIST_DIR}/../Drivers/common/ota/ota.cmake)
     ota_add_bootloader(${CMAKE_PROJECT_NAME})
     foreach(_t ${CMAKE_PROJECT_NAME} STM32_Drivers)   # -O0 Debug builds do not fit 24 KB
         target_compile_options(${_t} PRIVATE $<$<CONFIG:Debug>:-Og>)
     endforeach()
     ```
   - `main.c` (USER CODE Includes / 2):
     ```c
     #include "ota_stm32.h"
     #include "ota_board_ecu.h"
     ...
     static const ota_layout_t layout = OTA_BOARD_LAYOUT;
     ota_bootloader_run(&hfdcan1, &layout);   /* never returns */
     ```
   - optional LED: define `void ota_bootloader_on_poll(uint8_t state, uint32_t tick)`
     (`state == OTA_STATE_RECEIVING` while an image comes in).
4. Flash once with ST-Link: full erase, bootloader, then the app. From then on: CAN only.

### Boards that build one firmware for two nodes

Gateway (front/rear) and GI (left/right) select the side at compile time. Define
`OTA_BOARD_NODE_ID` before including the header:

```c
/* app, e.g. from the gateway's DEVICE_TYPE */
#define OTA_BOARD_NODE_ID ((DEVICE_TYPE == DEVICE_FRONT_GATEWAY) ? OTA_NODE_GF : OTA_NODE_GR)
#include "ota_board_gateway.h"
```

and build the bootloader twice, e.g. `-DOTA_BOARD_NODE_ID=OTA_NODE_GF` / `OTA_NODE_GR` in two
CMake presets. Two nodes with the same ID on the bus will both answer.

### Family notes

- **G4 (Gateways, GI_V1)**: the current gateway firmware jumps to the ST ROM bootloader on
  `0x4F0` with byte 0 = `0x02`/`0x03`. Replace that `case 0x4F0` with the OTA dispatch above
  when integrating. The host tool never sends `0x4F0` to a node that has not answered an OTA
  PING, so it cannot trip old gateway firmware. The last 2 KB page stays the EEPROM.
- **L5 (GI, LV SoC)**: TrustZone must be disabled (TZEN=0). Works with DBANK=0 or 1.
- **H5 Dashboard**: CAN is FDCAN2 (`ota_app_stm32_init(&hfdcan2, ...)`), and the bootloader
  needs FDCAN2 too.
- **RTOS boards (Telemetry/ThreadX, Dashboard)**: call `ota_app_handle_frame()` from the CAN
  thread. A granted request waits up to 20 ms for the TX FIFO, then resets.

## Flashing

```
pip install python-can pyserial
python ota/tools/ota_flash.py -i slcan -c COM6 scan
python ota/tools/ota_flash.py -i slcan -c COM6 flash --node ECU --bin build/Debug/ART_26_ECU_V2.bin
python ota/tools/ota_flash.py -i slcan -c COM6 enter --node GF      # then info / reset
```

`-i pcan -c PCAN_USBBUS1` for a PCAN-USB. `--catch` + power cycle reaches a node whose app
does not answer (the bootloader listens 50 ms after every reset). An interrupted or corrupted
update leaves the node in the bootloader; just flash again.

About 16 KB/s on a loaded 500 kbit/s bus (123 KB ECU image: 7.5 s).

## Protocol

| ID | Dir | Frame |
|---|---|---|
| `0x4F0` | host -> all | `[node, 'O','T','A', 0...]` enter bootloader (DBC `PRG_Bootloader`) |
| `0x6N0` | host -> node | CMD `[op, args]`: PING, INFO, START, END, ABORT, RESET, STATUS |
| `0x6N1` | node -> host | RSP `[op, status, b2, b3, u32]` |
| `0x6N2` | host -> node | DATA `[seq, 7 bytes]`, 64 frames per acknowledged block (448 B) |

Boot decision: start the app if its vector table is plausible and there is no START marker
without a COMMIT (an interrupted update). A valid app also starts after 30 s without traffic.

## Tests

```
python -m unittest discover -s ota/tools/test -v     # 15 end-to-end simulator tests
```

## Debugging notes

- With the bootloader in flash, ST-Link debugging of the app works as usual (only app sectors
  are rewritten). A node stuck in the bootloader after an interrupted OTA: flash it over CAN
  again, or erase the meta sector/page.
- Attaching STM32CubeProgrammer in hot-plug mode to a running H5 app was seen to leave the
  NVIC with every interrupt disabled (CAN RX stops). Reset after such reads.
