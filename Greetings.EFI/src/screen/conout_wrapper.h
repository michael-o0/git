#ifndef UB_CONSOLE_WRAPPER_H
#define UB_CONSOLE_WRAPPER_H

#include <efi/efi.h>
#include <efi/efilib.h>

EFI_STATUS ConOutWrapperInit(EFI_HANDLE imghandle, EFI_SYSTEM_TABLE *systab);
EFI_STATUS ConOutWrapperExit(EFI_HANDLE imghandle, EFI_SYSTEM_TABLE *systab);

#endif /* !UB_CONSOLE_WRAPPER_H */
