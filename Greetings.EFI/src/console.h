/*
 * console.h
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include "./efi_protocols/ConsoleControl.h"
#include "./efi_protocols/SimpleTextInEx.h"
#include "./efi_protocols/UgaDraw.h"
#include "./cdefs.h"

__BEGIN_DECLS

struct console
{
        void *c_ctx;

        INTN (*c_probe)(IN struct console *console);

        INTN (*c_init)(IN struct console *console, IN INTN arg);

        INTN (*c_poll)(IN struct console *console);

        INTN (*c_rc)(IN struct console *console, OUT VOID *trgt);

        INTN (*c_wc)(IN struct console *console, IN INTN chr);

        INTN (*c_ws)(IN struct console *console, IN VOID *str);

};

__END_DECLS

#endif /* !CONSOLE_H */
