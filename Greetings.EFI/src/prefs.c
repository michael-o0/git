/*
 TODO:
 Preferences:
 Missing ConIn ? : act
 Missing ConOut ? : act
 Time ~ : various
 Fonts : cache a bitmap of current
 load FreeType only when selecting new
 */

#include "globals.h"

EFI_STATUS prefs_init(VOID)
{

        CHAR16 *err_str =
                L"ub_prefs_init: error: The handle provided for receiving "
                L"operator input is invalid; powering system down..";

        /* make this a pref */
        if (Xefi_systbl->ConsoleOutHandle == NULL) {
                goto _exit;
        };

        /* make this a pref */
        if (Xefi_systbl->ConsoleInHandle == NULL) {
                goto _errx;
        };

        return (EFI_SUCCESS);

        _errx:
        /* State */
        Xefi_systbl->ConOut->OutputString(Xefi_systbl->ConOut, err_str);

        /* Give a chance for statement to be read */
        Xefi_systbl->BootServices->Stall( EFI_SECOND * 5);

        /* FALLTHROUGH */
        _exit:
        /* Take action */
        Xefi_systbl->RuntimeServices->ResetSystem(EfiResetShutdown,
        EFI_SUCCESS, 0,
        NULL);

        /* NOTREACHED */
        cpu_dead_loop();

        /* NOTREACHED */
        return (EFI_UNSUPPORTED);
}
