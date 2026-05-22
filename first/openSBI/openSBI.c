
 //chages for lib/sbi/sbi_ecall_vendor.c


#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_trap.h>
#include <sbi/riscv_asm.h>

#define SBI_EXT_CUSTOM_READ_MISA 0x08000001

static int sbi_ecall_custom_misa_handler(unsigned long extid, unsigned long funcid,
                                         const struct sbi_trap_regs *regs,
                                         unsigned long *out_val,
                                         struct sbi_trap_info *out_trap)
{
    if (extid == SBI_EXT_CUSTOM_READ_MISA) {
        unsigned long misa_val = csr_read(CSR_MISA);
        
        *out_val = misa_val;
        return SBI_SUCCESS;
    }
    
    return SBI_ENOTSUPP;
}

struct sbi_ecall_extension ecall_custom_misa = {
    .extid_start = SBI_EXT_CUSTOM_READ_MISA,
    .extid_end   = SBI_EXT_CUSTOM_READ_MISA,
    .handle      = sbi_ecall_custom_misa_handler,
};

