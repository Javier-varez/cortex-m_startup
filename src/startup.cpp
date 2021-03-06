#include <algorithm>
#include <cstdint>

volatile std::uint32_t* const CORE_DEBUG_DHCSR =
    reinterpret_cast<volatile std::uint32_t*>(0xE000EDF0UL);

extern std::uint8_t __StackInit;

//! Define this symbol in your main.cpp as the entrypoint
extern "C" int main();

extern "C"
void Reset_Handler() {
    // Initialize data section
    extern std::uint8_t __copy_start__;
    extern std::uint8_t __copy_end__;
    extern std::uint8_t __copy_origin;
    std::size_t size = static_cast<size_t>(&__copy_end__ - &__copy_start__);
    std::copy(&__copy_origin, &__copy_origin + size, &__copy_start__);

    // Initialize bss section
    extern std::uint8_t __bss_start__;
    extern std::uint8_t __bss_end__;
    std::fill(&__bss_start__, &__bss_end__, UINT8_C(0x00));

    // Initialize static objects by calling their constructors
    typedef void (*function_t)();
    extern function_t __init_array_start[0];
    extern function_t __init_array_end[0];
    std::for_each(__init_array_start, __init_array_end, [](function_t ptr) {
        ptr();
    });

    // Jump to main
    asm volatile (
        "msr msp, %[stack_top]\n"
        // We want to make sure that any stacking
        // operations after this use the correct
        // stack pointer value
        "dsb\n"
        "bl main"
        : : [stack_top] "r" (&__StackInit),
            [main] "i" (main)
    );
}

extern "C" __attribute__((noreturn)) void DefaultISR() {
    volatile int32_t exception_number;
    asm("mrs %[ipsr_reg], ipsr   \n" : [ipsr_reg] "=r" (exception_number));

    exception_number -= 16;
    if (*CORE_DEBUG_DHCSR & (1 << 0)) {
        // We trigger a breakpoint if we are debugging to warn the user
        // about the default ISR being reached.
        asm("bkpt #0");
    }

    while(true) { }
}

#define DEFINE_DEFAULT_ISR(name) \
    extern "C" \
    __attribute__((weak, noreturn)) \
    inline void name() { \
      DefaultISR(); \
    }

DEFINE_DEFAULT_ISR(NMI_Handler)
DEFINE_DEFAULT_ISR(HardFault_Handler)
DEFINE_DEFAULT_ISR(MemManage_Handler)
DEFINE_DEFAULT_ISR(BusFault_Handler)
DEFINE_DEFAULT_ISR(UsageFault_Handler)
DEFINE_DEFAULT_ISR(SVC_Handler)
DEFINE_DEFAULT_ISR(DebugMon_Handler)
DEFINE_DEFAULT_ISR(PendSV_Handler)
DEFINE_DEFAULT_ISR(SysTick_Handler)

//! This is a default exception table for Cortex-M microcontrollers.
//! Define these functions in your user code to use it.
//! Interrupts should go in the .interrupt_table section.
//! This is located right after the .exception_table section.
volatile const std::uintptr_t g_exception_table[]
__attribute__((section(".exception_table"))) {
    // Stack Ptr initialization
    reinterpret_cast<std::uintptr_t>(&__StackInit),
    // Entry point
    reinterpret_cast<std::uintptr_t>(Reset_Handler),
    // Exceptions
    reinterpret_cast<std::uintptr_t>(NMI_Handler),              /* NMI_Handler */
    reinterpret_cast<std::uintptr_t>(HardFault_Handler),        /* HardFault_Handler */
    reinterpret_cast<std::uintptr_t>(MemManage_Handler),        /* MemManage_Handler */
    reinterpret_cast<std::uintptr_t>(BusFault_Handler),         /* BusFault_Handler */
    reinterpret_cast<std::uintptr_t>(UsageFault_Handler),       /* UsageFault_Handler */
    reinterpret_cast<std::uintptr_t>(nullptr),                  /* Reserved */
    reinterpret_cast<std::uintptr_t>(nullptr),                  /* Reserved */
    reinterpret_cast<std::uintptr_t>(nullptr),                  /* Reserved */
    reinterpret_cast<std::uintptr_t>(nullptr),                  /* Reserved */
    reinterpret_cast<std::uintptr_t>(SVC_Handler),              /* SVC_Handler */
    reinterpret_cast<std::uintptr_t>(DebugMon_Handler),         /* DebugMon_Handler */
    reinterpret_cast<std::uintptr_t>(nullptr),                  /* Reserved */
    reinterpret_cast<std::uintptr_t>(PendSV_Handler),           /* PendSV_Handler */
    reinterpret_cast<std::uintptr_t>(SysTick_Handler),          /* SysTick_Handler */
};
