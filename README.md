# Copy-on-Write (CoW) `fork()` in xv6 kernel

This is a **C** implementation of the Copy-on-Write (CoW) `fork()` in the xv6 kernel.

Background: `fork()` is a system call that creates a new process by replication. The original `fork()` which copies all of the parent process's user-space memory into the child.

Memory optimization: The new implementation only copies the page which the child process tries to write into, increasing memory efficiency.

Usage:

- `make qemu-nox`: Run xv6 in QEMU
- Type in any command you want to run in xv6, which essentially runs the `fork()` system call to allocate a new process
- `make clean`: Clean up

Acknowledgments: Thanks to CS 537 Spring 2023 course staff at University of Wisconsin–Madison for providing starter code, project description, and test cases.