# Gunyah C Runtime

A runtime for light-weight OS-less application VMs supporting *bare metal*
applications on the Gunyah Hypervisor.

The runtime provides a minimal kernel emulation layer for Linux system calls
which Musl libc internally uses.

The runtime is compiled as stand alone ELF executable. It is loaded into memory
along side a musl libc linked application and is executed with an argument to
the application entry-point and a simple environment structure. It hooks the
CPU exception vectors to provide system call and interrupt handling. The
runtime does not reside at a higher privilege level and has no memory
protection to isolate it from the application/libc (it is not a kernel).

The runtime handles initialization and starting the application. It sets up the
environment and stack that would normally be handled by a dynamic loader.

## Compile and Run

The runtime is used by the Gunyah Hypervisor project. See
https://github.com/quic/gunyah-hypervisor for details.

Note, The C runtime should support any Linux targeted libc, however it has only
been tested with the Gunyah hypervisor's Resource Manager, built with musl
libc, and only implements the minimal set of syscalls this uses.

## System Overview

### Build system

The build system is a simplified version of the hypervisor's, it has:

* No explicit module configurations.
* Only contains basic rules to compile C / assembly code into the target
  binary.
* Uses a subset of the Gunyah configuration syntax.
* Adds new configuration syntax to support customized rule/target.

The project layout looks like:

```
root
 +
 |
 +--->build.conf
 |
 +--->README.md
 |
 +--->src
 |     +
 |     |
 |     +--->build.conf
 |     |
 |     +--->source.c
 |     |
 |     +--->assemble.S
 |     |
 |     +--->package1
 |             +
 |             |
 |             +--->build.conf
 |             |
 |             +--->src
 |             |
 |             +--->include
 |
 +--->include
```

The ```build.conf``` contains all compilation information, and it can refer to
another configuration file in specified sub directory.


### Syscalls and Interrupts

The runtime implements a syscall handling exception vector, however only a
small subset of the Linux AArch64 ABI is implemented, and it includes some
non-Linux like behaviour such as application interrupt handling support via
special `/dev/` nodes using `ioctl()`.

### Exception vector table

Only AArch64 64-bit SVCs are implemented. Interrupts are also supported. Other
exceptions and unimplemented system calls result in a panic().

### Application initialization

Sets up the ELF execution environment and directly jumps to the application
entry address.
