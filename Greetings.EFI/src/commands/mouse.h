/* */

#ifndef _PMP_EBSS_EFI_SHELL_COMMAND_MOUSE_
#define _PMP_EBSS_EFI_SHELL_COMMAND_MOUSE_

#include <efi/efi.h>
#include <efi/efilib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

EFI_STATUS
rzr_efi_shell_command_efi_mse(
        INTN argc,
        CHAR16 ** argv
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* not _PMP_EBSS_EFI_SHELL_COMMAND_MOUSE_ */
