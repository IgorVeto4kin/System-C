/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_trap.h>
#include <sbi/riscv_asm.h>
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
        // Читаем физический регистр CSR (MISA)
        unsigned long misa_val = csr_read(CSR_MISA);
        
        // Записываем результат для передачи в S-Mode
        *out_val = misa_val;
        return SBI_SUCCESS;
    }
    
    return SBI_ENOTSUPP;
}

// Структура расширения
struct sbi_ecall_extension ecall_custom_misa = {
    .extid_start = SBI_EXT_CUSTOM_READ_MISA,
    .extid_end   = SBI_EXT_CUSTOM_READ_MISA,
    .handle      = sbi_ecall_custom_misa_handler,
};


static inline unsigned long sbi_ecall_vendor_id(void)
{
	return SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

static int sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    struct sbi_trap_regs *regs,
				    struct sbi_ecall_return *out)
{
	return sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

struct sbi_ecall_extension ecall_vendor;

static int sbi_ecall_vendor_register_extensions(void)
{
	unsigned long extid = sbi_ecall_vendor_id();

	if (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		return 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	return sbi_ecall_register_extension(&ecall_vendor);
}

struct sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
