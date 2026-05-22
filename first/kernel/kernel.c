//changes for file linux7.0.9/kernel/sys.c

#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <asm/sbi.h>

#define SBI_EXT_CUSTOM_READ_MISA 0x08000001

SYSCALL_DEFINE1(get_hw_misa, unsigned long __user *, user_val)
{
    struct sbiret ret;
    unsigned long kernel_val;

    ret = sbi_ecall(SBI_EXT_CUSTOM_READ_MISA, 0, 0, 0, 0, 0, 0, 0);

    if (ret.error) {
        return -EINVAL; 
    }

    kernel_val = ret.value;

    if (copy_to_user(user_val, &kernel_val, sizeof(kernel_val))) {
        return -EFAULT; 
    }

    return 0; 
}