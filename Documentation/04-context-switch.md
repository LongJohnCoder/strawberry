<img src="https://github.com/bjornbrodtkorb/BlackOS/blob/master/BlackOS%20Graphics/black_os_logo_trim.png" width="200">

# Context switch

In this tutorial I will explain how the kernel switches threads

At the very core the operating system should provide methods for running multiple threads "virtually" at the same time. This part of the kernel is called the context switcher, since it switches the current context (state of execution). In reality the processor can only run one thread at a time, but by switching threads hundreds of times per second, is gives a feeling of doing several things at once. So, how is all this possible?

Some terminology<br />
_exception_: same as interrupt<br />
_thread_: same as task. It can for example be a function<br />
_handler_: the actual function that is executed when an interrupt occurs<br />


# Interrupt routines

The ARM Cortex-M core has some nifty features that make it especially suited for building operating systems. Among them is the interrupt system that ARM provides. There are three important interrupts that you should be aware of

- SysTick interrupt
- PendSV interrupt
- SVCall interrupt

Lets go through each of them. The SysTick interrupt is a peripheral that generates interrupt on a regularily basis, allowing an operating system to carry out the context switch (when the processor switches task) each time the interrupt occurs. It can be thought of as a periodic interrupt. As we will see later we will use this interrupt to issue the context switch. The next important interrupt is the pendSV interrupt. This is used in an OS for a system-level service, when no other interrupt is active. Yes, when no other interrupt is active. The pendSV interrupt can be issued by software and will only be executed when all other interrupts are handled. The last important interrupt is the SVCall interrupt. SVCall (_supervisor call_) is used to request a system service. The interrupt is triggered by the assembly instruction SVC. This instruction can take a paramenter that is used to determine what service to run. This will be further described in the service chapter. 

# Triggering the context switch

For now we will look at a scheduler that swithces context (switches thread or task) 1000 times per second. We will configure the SysTick to give an interrupt 1000 time per second. It is important that we do NOT switch the context in the systick interrupt handler. This may delay other interrupts such as USB and GMAC, and is therefore unwanted. Instead we will use the systick handler to pend the pendSV interrupt. This is done by setting a special bit in a special register.

```c
SCB->ICSR |= (1 << SCB_ICSR_PENDSVSET_Pos);
```

When there is not other interrupt to service, the processor will execute the code in the pendSV handler. This is where we actually do the context switch. This process is quite complex so we will do it in assembly, but don't worry, once you understand what is going on you don't need to bother with the assembly code anymore.  

# Context switch in assembly

Before we can dive into the acctual context switch we must make sure we understand some important concepts. If the word stack make no sense to you I recommend looking that up before proceeding. Good. Let us first look at the table on the left. The Cortex-M microcontrollers has 13 general purpose registers labled R0 - R12. These are used for data operations (some exceptions here). The next register is the stack pointer SP which always points to the top of the current stack, not the stack, but the current stack. Next is the link register LR. This stores return information for subroutines, function calls and exceptions. Last of the non-special registers is the program counter register PC, which points to the next instruction to execute. To save the current state of the CPU we must save these 16 registers. If we do that, we can later return to exactly the same place and continue the execution. The process of saving these registers is known as the context saving, and the restore operation is known as context retrieval.
<img src="https://github.com/bjornbrodtkorb/x-operating-system/blob/master/X%20Graphics/cortex_stack_frame_reg.png" width="1000">

What happends when a processor enters an interrupt? Before executing the first instruction in the interrupt handler, the processor does something called stacking. This process involves pushing 8 registers (25 registers if the CPU use floating point instructions) onto the stack. This is called a stack frame. The stack frame is pushed automatically by the CPU. For us to complete the context saving we must push the remaining 8 bytes onto stack. 
