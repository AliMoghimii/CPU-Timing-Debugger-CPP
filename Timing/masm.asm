
ifdef RAX
; ******************************************************************************************
; x64 implementation
; ******************************************************************************************

.DATA

.CODE

c_ext_getCPUClock PROC PUBLIC
; Save state
push rbx
push rcx
push rdx

ifdef 0

; Calculates the overhead of calling the CPUID and RDTSC functions
; Run through the same code 3 times to warm up the CPUID and RDTSC functions .... see intel paper
xor rax, rax
cpuid
MFENCE
LFENCE
rdtsc
LFENCE

xor rax, rax
cpuid
MFENCE
LFENCE
rdtsc
LFENCE

xor rax, rax
cpuid
MFENCE
LFENCE
rdtsc
LFENCE

; Get the start time
xor rax, rax
cpuid
MFENCE
LFENCE
rdtsc
LFENCE

else

MFENCE
rdtscp
LFENCE

endif

; Restore state
pop rdx
pop rcx
pop rbx

ret
c_ext_getCPUClock ENDP


else


; ******************************************************************************************
; x86 implementation
; ******************************************************************************************

.MODEL FLAT, C
.DATA

.CODE

c_ext_getCPUClock PROC PUBLIC
; Save state
push ebx
push ecx
push edx
 
if 0

; Calculates the overhead of calling the CPUID and RDTSC functions
; Run through the same code 3 times to warm up the CPUID and RDTSC functions .... see intel paper
xor eax, eax
cpuid
MFENCE
LFENCE
rdtsc
LFENCE

xor eax, eax
cpuid
MFENCE
LFENCE
rdtsc
LFENCE

xor eax, eax
cpuid
MFENCE
LFENCE
rdtsc
LFENCE

; Get the start time
xor eax, eax
cpuid
MFENCE
LFENCE
rdtsc
LFENCE

else

MFENCE
rdtscp
LFENCE

endif

; Restore state
pop edx
pop ecx
pop ebx

ret
c_ext_getCPUClock ENDP


endif

END