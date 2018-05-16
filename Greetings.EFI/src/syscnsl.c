#include "syscnsl.h"

/* ConIn */
static EFI_GUID guid_prtcl_stxt_in = EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID;
static EFI_GUID guid_prtcl_stxt_in_ex = EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;
static EFI_GUID guid_prtcl_sptr = EFI_SIMPLE_POINTER_PROTOCOL_GUID;
static EFI_GUID guid_prtcl_abs_ptr = EFI_ABSOLUTE_POINTER_PROTOCOL_GUID;
/* ConOut*/
static EFI_GUID guid_prtcl_stxt_out = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
static EFI_GUID guid_prtcl_con_ctrl = EFI_CONSOLE_CONTROL_PROTOCOL_GUID;
static EFI_GUID guid_ptrcl_uga_draw = EFI_UGA_DRAW_PROTOCOL_GUID;
static EFI_GUID guid_prtcl_gop = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

/* Forward fn() prototypes */
static INTN console_probe(struct console *console);
static INTN console_init(struct console *console, INTN arg);
static INTN console_poll(struct console *console);
static INTN console_poll_ex(struct console *console);
static INTN console_getk(struct console *console, void *trgt);
static INTN console_getk_ex(struct console *console, void *trgt);
static INTN console_wc(struct console *console, INTN chr);
static INTN console_ws(struct console *console, VOID *str);

struct syscnsl_ctx Xsyscnsl_ctx = {
        .buffered = 0, /* !! . */
        .conin_handle = NULL,
        .conout_handle = NULL,
        .bootsrvcstabl = NULL,
        /* conin_handle protocols */
        .prtcl_stxt_in = NULL,
        .prtcl_stxt_in_ex = NULL,
        .prtcl_sptr = NULL,
        .prtcl_abs_ptr = NULL,
        /* conout_handle protocols */
        .prtcl_stxt_out = NULL,
        .prtcl_con_ctrl = NULL,
        .prtcl_uga_draw = NULL,
        .prtcl_gop = NULL};

struct console Xsyscnsl = {
        .c_ctx = &Xsyscnsl_ctx,
        .c_probe = console_probe,
        .c_init = console_init,
        .c_poll = console_poll,
        .c_rc = console_getk,
        .c_wc = console_wc,
        .c_ws = console_ws};

/* Macros specific to console_probe() */
#define PROBE_CON_IN_HANDLE(efi_protocol_guid, efi_protocol_ptr)        \
                if ((efirc = ctx->bootsrvcstabl->HandleProtocol(        \
                                (EFI_HANDLE) ctx->conin_handle,         \
                                &efi_protocol_guid,                     \
                                (VOID **)&efi_protocol_ptr)) != 0)      \
                        efi_protocol_ptr = NULL;

#define PROBE_CON_OUT_HANDLE(efi_protocol_guid, efi_protocol_ptr)       \
                if ((efirc = ctx->bootsrvcstabl->HandleProtocol(        \
                                (EFI_HANDLE) ctx->conout_handle,        \
                                &efi_protocol_guid,                     \
                                (VOID **)&efi_protocol_ptr)) != 0)      \
                        efi_protocol_ptr = NULL;
/*
 * console_probe
 */
static INTN console_probe(struct console *console)
{
        INTN rc = 0;
        EFI_STATUS efirc;
        struct syscnsl_ctx *ctx;

        if ((console == NULL) || (console->c_ctx == NULL))
                return (1);

        ctx = (struct syscnsl_ctx *) console->c_ctx;

        if (ctx->conin_handle != NULL) {
                PROBE_CON_IN_HANDLE(guid_prtcl_stxt_in, ctx->prtcl_stxt_in);
                PROBE_CON_IN_HANDLE(guid_prtcl_stxt_in_ex,
                        ctx->prtcl_stxt_in_ex);
                PROBE_CON_IN_HANDLE(guid_prtcl_sptr, ctx->prtcl_sptr);
                PROBE_CON_IN_HANDLE(guid_prtcl_abs_ptr, ctx->prtcl_abs_ptr);
        } else {
                rc++;
        }

        if (ctx->conout_handle != NULL) {
                PROBE_CON_OUT_HANDLE(guid_prtcl_stxt_out, ctx->prtcl_stxt_out);
                PROBE_CON_OUT_HANDLE(guid_prtcl_con_ctrl, ctx->prtcl_con_ctrl);
                PROBE_CON_OUT_HANDLE(guid_ptrcl_uga_draw, ctx->prtcl_uga_draw);
                PROBE_CON_OUT_HANDLE(guid_prtcl_gop, ctx->prtcl_gop);
        } else {
                rc++;
        }

        if ((ctx->prtcl_stxt_in_ex) != NULL) {
                console->c_poll = console_poll_ex;
                console->c_rc = console_getk_ex;
        }

        return (rc);
}

#undef PROBE_CON_IN_HANDLE
#undef PROBE_CON_OUT_HANDLE

static INTN console_init(struct console *console, INTN arg)
{
        struct syscnsl_ctx *ctx;

        if ((console == NULL) || (console->c_ctx == NULL))
                goto out;

        ctx = (struct syscnsl_ctx *) console->c_ctx;

        if (ctx->prtcl_stxt_in != NULL) {
                ctx->prtcl_stxt_in->Reset(ctx->prtcl_stxt_in,
                        arg != 0 ? TRUE : FALSE);
                return (0);
        }

        out:
        return (1);
}

/*
 * console_poll
 */
static INTN console_poll(struct console *console)
{
        EFI_STATUS efirc;
        EFI_INPUT_KEY ikey;
        struct syscnsl_ctx *ctx;

        if ((console == NULL) || (console->c_ctx == NULL))
                return (-1);

        ctx = (struct syscnsl_ctx *) console->c_ctx;

        if (ctx->buffered)
                return (ctx->buffered);

        if (ctx->prtcl_stxt_in == NULL)
                return (-1);

        efirc = ctx->prtcl_stxt_in->ReadKeyStroke(ctx->prtcl_stxt_in, &ikey);

        if (efirc == EFI_SUCCESS) {
                ctx->buf.KeyState.KeyShiftState = 0;
                ctx->buf.KeyState.KeyToggleState = 0;
                ctx->buf.Key = ikey;
                ctx->buffered = 1;
        }

        return (ctx->buffered);
}

/*
 * console_poll_ex
 */
static INTN console_poll_ex(struct console *console)
{
        EFI_STATUS efirc;
        EFI_KEY_DATA ekd;
        struct syscnsl_ctx *ctx;

        if ((console == NULL) || (console->c_ctx == NULL)) {
                return (-1);
        }

        ctx = (struct syscnsl_ctx *) console->c_ctx;

        if (ctx->buffered) {
                return (ctx->buffered);
        }

        if (ctx->prtcl_stxt_in_ex == NULL) {
                return (-1);
        }

        efirc = ctx->prtcl_stxt_in_ex->ReadKeyStrokeEx(ctx->prtcl_stxt_in_ex,
                &ekd);

        if (efirc == EFI_SUCCESS) {
                ctx->buffered = 1;
                ctx->buf = ekd;
        }

        return (ctx->buffered);
}

/*
 * console_getk
 */
static INTN console_getk(struct console *console, void *trgt)
{
        UINTN eeid;
        EFI_STATUS efirc;
        EFI_INPUT_KEY ikey;
        struct syscnsl_ctx *ctx;
        EFI_KEY_DATA *ekd;

        if ((console == NULL) || (console->c_ctx == NULL)) {
                return (-1);
        }

        ctx = (struct syscnsl_ctx *) console->c_ctx;
        ekd = (EFI_KEY_DATA *) trgt;

        if (ctx->buffered) {
                ctx->buffered = 0;
                *ekd = ctx->buf;
        } else {
                efirc = ctx->prtcl_stxt_in->ReadKeyStroke(ctx->prtcl_stxt_in,
                        &ikey);

                while (efirc == EFI_NOT_READY) {
                        if ((ctx->prtcl_stxt_in->WaitForKey) != NULL) {
                                ctx->bootsrvcstabl->WaitForEvent(1,
                                        &ctx->prtcl_stxt_in->WaitForKey, &eeid);
                        }
                        efirc = ctx->prtcl_stxt_in->ReadKeyStroke(
                                ctx->prtcl_stxt_in, &ikey);
                }

                ekd->KeyState.KeyShiftState = 0;
                ekd->KeyState.KeyToggleState = 0;
                ekd->Key = ikey;
        }

        return (0);
}

/*
 * console_getk_ex
 */
static INTN console_getk_ex(struct console *console, void *trgt)
{
        UINTN eeid;
        EFI_STATUS efirc;
        struct syscnsl_ctx *ctx;
        EFI_KEY_DATA *ekd;

        if ((console == NULL) || (console->c_ctx == NULL)) {
                return (-1);
        }

        ctx = (struct syscnsl_ctx *) console->c_ctx;
        ekd = (EFI_KEY_DATA *) trgt;

        if (ctx->buffered) {
                ctx->buffered = 0;
                *ekd = ctx->buf;
        } else {
                efirc = ctx->prtcl_stxt_in_ex->ReadKeyStrokeEx(
                        ctx->prtcl_stxt_in_ex, ekd);

                while (efirc == EFI_NOT_READY) {
                        if ((ctx->prtcl_stxt_in_ex->WaitForKeyEx) != NULL) {
                                ctx->bootsrvcstabl->WaitForEvent(1,
                                        &ctx->prtcl_stxt_in_ex->WaitForKeyEx,
                                        &eeid);
                        }
                        efirc = ctx->prtcl_stxt_in_ex->ReadKeyStrokeEx(
                                ctx->prtcl_stxt_in_ex, ekd);
                }
        }

        return (0);
}

static INTN console_wc(struct console *console, INTN chr)
{
        CHAR16 buf[2];
        struct syscnsl_ctx *ctx;

        ctx = (struct syscnsl_ctx *) console->c_ctx;

        buf[0] = (CHAR16) chr;
        buf[1] = 0;

        ctx->prtcl_stxt_out->OutputString(ctx->prtcl_stxt_out, buf);

        return (0);
}

static INTN console_ws(struct console *console, VOID *str)
{
        struct syscnsl_ctx *ctx;

        ctx = (struct syscnsl_ctx *) console->c_ctx;

        ctx->prtcl_stxt_out->OutputString(ctx->prtcl_stxt_out, str);

        return (0);
}
