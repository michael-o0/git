#ifndef UB_FS_H
#define UB_FS_H

#include <efi/efi.h>
#include <efi/efilib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

INTN fsaccess_init(EFI_HANDLE ubhand, EFI_SYSTEM_TABLE *systab);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !UB_FS_H */
