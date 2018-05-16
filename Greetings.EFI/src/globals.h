#ifndef GLOBALS_H
#define GLOBALS_H

#include "gnuefi.h"

#if 1
#define DPRINTF(fmt, ...) do{Print(fmt, __VA_ARGS__);}while(0);
#else
#define DPRINTF(fmt, ...) do{}while(0);
#endif

#define EFI_SECOND 1000000

#define GET_CARETXY(x, y)                       \
        x = Xefi_systbl->ConOut->Mode->CursorColumn;    \
        y = Xefi_systbl->ConOut->Mode->CursorRow;

#define SET_CARETXY(x, y) \
        Xefi_systbl->ConOut->SetCursorPosition(Xefi_systbl->ConOut, x, y);

#define FREE_AND_NULL(ptr)                                      \
        do {                                                    \
        	Xefi_systbl->BootServices->FreePool(ptr);       \
                ptr = NULL;                                     \
        } while(0)

extern EFI_SYSTEM_TABLE *Xefi_systbl;
extern EFI_HANDLE Xefi_loaded_imgage;

/* Console In protocols' GUIDs */
extern CONST EFI_GUID Xguid_prtcl_stxt_in;
extern CONST EFI_GUID Xguid_prtcl_stxt_in_ex;
extern CONST EFI_GUID Xguid_prtcl_sptr;
extern CONST EFI_GUID Xguid_prtcl_abs_ptr;

/* Console Out protocols' GUIDs*/
extern CONST EFI_GUID Xguid_prtcl_stxt_out;
extern CONST EFI_GUID Xguid_prtcl_con_ctrl;
extern CONST EFI_GUID Xguid_ptrcl_uga_draw;
extern CONST EFI_GUID Xguid_prtcl_gop;

/* Other protocols */
extern CONST EFI_GUID Xguid_prtcl_sfs;

extern EFI_SIMPLE_TEXT_INPUT_PROTOCOL *Xprtcl_stxt_in;
//extern EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *Xprtcl_stxt_in_ex;
extern EFI_SIMPLE_POINTER_PROTOCOL *Xprtcl_sptr;
extern EFI_ABSOLUTE_POINTER_PROTOCOL *Xprtcl_abs_ptr;
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *Xprtcl_stxt_out;
//extern EFI_CONSOLE_CONTROL_PROTOCOL *Xprtcl_con_ctrl;
//externEFI_UGA_DRAW_PROTOCOL *Xprtcl_uga_draw;
extern EFI_GRAPHICS_OUTPUT_PROTOCOL *Xprtcl_gop;

extern CHAR16 *Xprogname;
extern INTN Xkeeplooping;

EFI_STATUS globals_init(EFI_HANDLE loadedimg, EFI_SYSTEM_TABLE *efi_systbl);
UINTN putc8(const char *fmt, ...);
void cpu_dead_loop(VOID);

#endif /* !GLOBALS_H */
