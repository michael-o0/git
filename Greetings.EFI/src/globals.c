#include "globals.h"
#include "./efi_protocols/SimpleTextInEx.h"
#include "./efi_protocols/ConsoleControl.h"
#include "./efi_protocols/UgaDraw.h"
#include "./cdefs.h"

/* EFI SystemTable pointer */
EFI_SYSTEM_TABLE *Xefi_systbl = NULL;

/* LoadedImage handle */
EFI_HANDLE Xefi_loaded_imgage = NULL;

/* ConIn protocols' GUIDs */
CONST EFI_GUID Xguid_prtcl_stxt_in = EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID;
CONST EFI_GUID Xguid_prtcl_stxt_in_ex = EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;
CONST EFI_GUID Xguid_prtcl_sptr = EFI_SIMPLE_POINTER_PROTOCOL_GUID;
CONST EFI_GUID Xguid_prtcl_abs_ptr = EFI_ABSOLUTE_POINTER_PROTOCOL_GUID;

/* ConOut protocols' GUIDs */
CONST EFI_GUID Xguid_prtcl_stxt_out = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
CONST EFI_GUID Xguid_prtcl_con_ctrl = EFI_CONSOLE_CONTROL_PROTOCOL_GUID;
CONST EFI_GUID Xguid_ptrcl_uga_draw = EFI_UGA_DRAW_PROTOCOL_GUID;
CONST EFI_GUID Xguid_prtcl_gop = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

/*  Other protocols' GUIDs */
CONST EFI_GUID Xguid_prtcl_sfs = SIMPLE_FILE_SYSTEM_PROTOCOL;

/*
 * Global protocol ptrs
 */
/* ConIn protocols' ptrs */
EFI_SIMPLE_TEXT_INPUT_PROTOCOL *Xprtcl_stxt_in = NULL;
EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *Xprtcl_stxt_in_ex = NULL;
EFI_SIMPLE_POINTER_PROTOCOL *Xprtcl_sptr = NULL;
EFI_ABSOLUTE_POINTER_PROTOCOL *Xprtcl_abs_ptr = NULL;

/* ConOut protocols' ptrs */
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *Xprtcl_stxt_out = NULL;
EFI_CONSOLE_CONTROL_PROTOCOL *Xprtcl_con_ctrl = NULL;
EFI_UGA_DRAW_PROTOCOL *Xprtcl_uga_draw = NULL;
EFI_GRAPHICS_OUTPUT_PROTOCOL *Xprtcl_gop = NULL;

CHAR16 *Xprogname = NULL;

INTN Xkeeplooping = 1;

#define PROBE_CONINHANDLE(efi_protocol_guid, efi_protocol_ptr)          \
                if ((efirc = efi_systbl->BootServices->HandleProtocol(  \
                                efi_systbl->ConsoleInHandle,            \
                                (EFI_GUID*) &efi_protocol_guid,         \
                                (VOID **)&efi_protocol_ptr)) != 0)      \
                        efi_protocol_ptr = NULL;

#define PROBE_CONOUTHANDLE(efi_protocol_guid, efi_protocol_ptr)         \
                if ((efirc = efi_systbl->BootServices->HandleProtocol(  \
                                efi_systbl->ConsoleOutHandle,           \
                                (EFI_GUID *) &efi_protocol_guid,        \
                                (VOID **)&efi_protocol_ptr)) != 0)      \
                        efi_protocol_ptr = NULL;

/*
 *
 */
EFI_STATUS globals_init(EFI_HANDLE loadedimg, EFI_SYSTEM_TABLE *efi_systbl)
{
        EFI_STATUS efirc;

        Xefi_systbl = efi_systbl;
        Xefi_loaded_imgage = loadedimg;

        if (efi_systbl->ConsoleInHandle != NULL) {
                PROBE_CONINHANDLE(Xguid_prtcl_stxt_in, Xprtcl_stxt_in);
                PROBE_CONINHANDLE(Xguid_prtcl_stxt_in_ex, Xprtcl_stxt_in_ex);
                PROBE_CONINHANDLE(Xguid_prtcl_sptr, Xprtcl_sptr);
                PROBE_CONINHANDLE(Xguid_prtcl_abs_ptr, Xprtcl_abs_ptr);
        }

        if (efi_systbl->ConsoleOutHandle != NULL) {
                PROBE_CONOUTHANDLE(Xguid_prtcl_stxt_out, Xprtcl_stxt_out);
                PROBE_CONOUTHANDLE(Xguid_prtcl_con_ctrl, Xprtcl_con_ctrl);
                PROBE_CONOUTHANDLE(Xguid_ptrcl_uga_draw, Xprtcl_uga_draw);
                PROBE_CONOUTHANDLE(Xguid_prtcl_gop, Xprtcl_gop);
        }

        return (EFI_SUCCESS);
}

#undef PROBE_CONINHANDLE
#undef PROBE_CONOUTHANDLE

/*
 * XXX: buggy...
 */
UINTN putc8(const char *fmt, ...)
{

        UINTN back;

        va_list args;

        va_start(args, fmt);

        back = APrint((CHAR8 *) fmt, args);

        va_end(args);

        return (back);
}

/*
 *
 */
VOID cpu_dead_loop(VOID)
{
        volatile UINTN it;

        for (it = 0; it == 0;)
                ;
}
