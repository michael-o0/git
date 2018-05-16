#ifndef SYSCNSL_H
#define SYSCNSL_H

#include "console.h"

struct syscnsl_ctx
{
        /* poll() */
        INTN buffered;
        EFI_KEY_DATA buf;

        EFI_HANDLE *conin_handle;
        EFI_HANDLE *conout_handle;
        EFI_BOOT_SERVICES *bootsrvcstabl;

        /* conin_handle protocols */
        /*EFI_SIMPLE_TEXT_INPUT_PROTOCOL*/
        SIMPLE_INPUT_INTERFACE *prtcl_stxt_in;
        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *prtcl_stxt_in_ex;
        EFI_SIMPLE_POINTER_PROTOCOL *prtcl_sptr;
        EFI_ABSOLUTE_POINTER_PROTOCOL *prtcl_abs_ptr;
        /* conout_handle protocols */
        /*EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*/
        SIMPLE_TEXT_OUTPUT_INTERFACE *prtcl_stxt_out;
        EFI_CONSOLE_CONTROL_PROTOCOL *prtcl_con_ctrl;
        EFI_UGA_DRAW_PROTOCOL *prtcl_uga_draw;
        EFI_GRAPHICS_OUTPUT_PROTOCOL *prtcl_gop;

        /*
         EFI_EVENT
         WaitForString;

         CHAR16
         (* GetString)(
         __in VOID);
         */
};

extern struct syscnsl_ctx Xsyscnsl_ctx;
extern struct console Xsyscnsl;

#endif /* !SYSCNSL_H */
