*** Settings ***
Documentation    Portable CAN core on ARM. The submit callback models backpressure, not a physical FDCAN controller.
Resource         ${RENODEKEYWORDS}
Suite Setup      Setup
Suite Teardown   Teardown
Test Setup       Prepare CAN Case
Test Teardown    Test Teardown
Test Template    Run CAN Case

*** Variables ***
${EXPECTED_RESULT}    0

*** Test Cases ***
Immediate events are submitted separately                 1
Immediate busy status is not replayed                     2
Periodic phase coalesces missed deadlines                 3
Periodic retry submits the latest STOP payload            4
Bounded periodic work rotates between messages            5
Periodic millisecond clock wraps                          6
Two buses submit independently                            7
RX preserves order timestamps and overflow accounting      8
RX rejects invalid IDs lengths and pointers               9
Incomplete configuration leaves the driver closed         10
Standard and extended IDs remain distinct                 11
Immediate status precedes due periodic demand             12
Byte helpers and Hz conversion preserve their contracts    13

*** Keywords ***
Prepare CAN Case
    Test Setup
    Execute Monitor Command    mach create "can-core"
    Execute Monitor Command    machine LoadPlatformDescription @${PLATFORM}
    Execute Monitor Command    sysbus LoadELF @${ELF}
    ${entry}=    Execute Monitor Command    sysbus GetSymbolAddress "main"
    Execute Monitor Command    sysbus.cpu SetRegisterUnsafe 13 0x2001fff0
    Execute Monitor Command    sysbus.cpu PC ${entry}

Run CAN Case
    [Arguments]    ${number}
    ${address}=    Execute Monitor Command    sysbus GetSymbolAddress "can_test_case"
    Execute Monitor Command    sysbus WriteDoubleWord ${address} ${number}
    Execute Monitor Command    emulation RunFor "0.01"
    CAN Probe Should Equal    can_test_done    1
    CAN Probe Should Equal    can_test_progress    1
    CAN Probe Should Equal    can_test_result    ${EXPECTED_RESULT}

CAN Probe Should Equal
    [Arguments]    ${symbol}    ${expected}
    ${address}=    Execute Monitor Command    sysbus GetSymbolAddress "${symbol}"
    ${actual}=    Execute Monitor Command    sysbus ReadDoubleWord ${address}
    Should Be Equal As Integers    ${actual}    ${expected}    ${symbol}: ${actual}

Execute Monitor Command
    [Arguments]    ${command}
    ${result}=    Execute Command    ${command}
    RETURN    ${result}
