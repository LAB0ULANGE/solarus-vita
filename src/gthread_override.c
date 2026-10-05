// Force libstdc++ to always use its single-threaded (lock-free) internal
// code paths, bypassing the broken sceKernelGetProcessId-dependent locking
// in this toolchain's pthread-embedded implementation.
int __gthread_active_p(void) {
    return 0;
}
