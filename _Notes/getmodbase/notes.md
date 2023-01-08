ac_client.exe+0x109B74​
In Cheat Engine this syntax isn't a problem because CE will always get the address of client.dll at runtime. But when you transition to making real hacks, you have to resolve the module's address yourself using our function. That's what this tutorial is all about.

GHB Considerations​
We are making you read this tutorial before we show you how to do this in a video because it's the 2nd most common question when learning game hacking. We want you to know this tutorial is here, so you can come back to it later if necessary. This is officially taught in video form in our first C++ External Trainer Tutorial. Reading through this article first, will set you up for success before you start the video. So don't focus too hard on understanding this perfectly right now, it should all click when you watch the video later, if it doesn't, come back and read this.

GetModuleBaseAddress Tutorial​
If you're completely unfamiliar with this, we will explain everything in an easy to understand way. We use this function in all future tutorials, so it's important that you learn it now.

What will you learn?​
How to find the module base address in Cheat Engine
How to find the base address of a module using C++
How to navigate through the module list
How to obtain a list of modules for a specified process

Why do you need these skills?​
Why would you need to know the base address of a module? Everything you do in game hacking will involve relative offsets, the module will not always be loaded into the same base address. To compensate for this you are required to find the base address of the module at run time using the Windows API. The ToolHelp32 library provides us the functionality needed to loop through the modules and get their base addresses.

You should never use hard coded addresses, this is a bad behavior that will result in problems down the road. That's why we always teach you to use relative addresses in the form of binary.exe+0xBADF00D, when resolved at runtime using our function you will never have a problem.

What is a module?​
An .EXE or .DLL file (both are Portable Executable files) are referred to as binaries when they are on disk but they are called modules when loaded into memory.

Why don't modules have the same address every time?​
In the PE header for each binary, there is a field called ImageBase which defines the address it should be loaded. But what happens when another module is already in that location? Then the Windows loader will find a different address for it, this is called DLL re-location. That's why it's referred to as the preferred image base.

What about ASLR?​
You can imagine that exploiting vulnerabilities is quite simple when the things you're trying to exploit are in the same address on every computer, every time the program runs, right? That's why Microsoft added Address Space Layout Randomization as a security feature. With ASLR enabled, the address where a module is loaded is randomized. But in reality this is a very poor security feature, getting the address at runtime is quite simple as we will show.

.EXE vs .DLL Preferred Image Base​
When an EXE is loaded, the virtual address space of the process is empty, so it can always load into it's preferred image base address.
The only time an .exe isn't loaded into the PreferredImageBase is when ASLR is enabled on the OS and the DynamicBase flag is set to enable the OS to randomize the virtual address of the module.

But none of this really matters, ASLR is standard procedure now so we just assume it's enabled and use our function to ensure we get the right address.

Visual Example Using Cheat Engine​
To view all the modules loaded by a process using Cheat Engine:
Click Memory View
Click Tools
Click Dissect PE Headers

In the resulting window click on any DLL or .EXE and then expand PE Header to show this:

#3 is the preferred image base from the PE header and #4 is the current base address. They are the same in this example because ASLR is disabled and it's an EXE.

Common mistake: Make sure you click "info" each time you select a new module or it won't load the new results.

Our GetModuleBaseAddress Function​
There are actually multiple different ways to get the base address. The Windows API function named CreateToolhelp32Snapshot is the easiest and learning it will enable you to use this entire library as most of the functions work in the same way.

Why not just use the old dwGetModuleBaseAddress?​
The old function uses DWORD so it only supports x86. Our function uses uintptr_t which will resolve to a x86 or x64 address depending what architecture you build for. This makes it work on both, so you only need one function.

Our GetModuleBaseAddress Function​
There are actually multiple different ways to get the base address. The Windows API function named CreateToolhelp32Snapshot is the easiest and learning it will enable you to use this entire library as most of the functions work in the same way.

Why not just use the old dwGetModuleBaseAddress?​
The old function uses DWORD so it only supports x86. Our function uses uintptr_t which will resolve to a x86 or x64 address depending what architecture you build for. This makes it work on both, so you only need one function.

External:
uintptr_t clientDLLBaseAddr = GetModuleBaseAddress(ProcId, L"client.dll");

Internal:
uintptr_t clientDLLBaseAddr = (uintptr_t)GetModuleHandle(L"client.dll");