/*
 * Needs to be able to be shutdown either via kbd
 * or pointer input at most any time.
 * Being able to handle loss of power at any time.
 */

#include "events.h"
#include "fsaccess.h"
#include "prefs.h"
#include "screen/screen.h"
#include "syscnsl.h"
#include "globals.h"

#define libgnuefi_init(x, y)    InitializeLib(x, y);

/*
 * main
 * initialize things/ball rollin
 */
EFI_STATUS efi_main(EFI_HANDLE loadedimg, EFI_SYSTEM_TABLE *efi_systbl)
{
        /* XXX: ? CheckCrc(0, &systab->Hdr); */

        libgnuefi_init(loadedimg, efi_systbl);

        globals_init(loadedimg, efi_systbl);
        //env_init();
        //fsaccess_init(loadedimg, efi_systbl);
        //prefs_init();

        Xsyscnsl_ctx.conin_handle = efi_systbl->ConsoleInHandle;
        Xsyscnsl_ctx.conout_handle = efi_systbl->ConsoleOutHandle;
        Xsyscnsl_ctx.bootsrvcstabl = efi_systbl->BootServices;

        /*do
         {*/
        if (Xsyscnsl.c_probe(&Xsyscnsl)
        /* && ConInHandle == NULL && pause_on_missing_ConIn */) {
                // TODO
                // print msg
                // loop
        }
        /*} while (ConInHandle == NULL && !proceed);*/

        /* Flush out any keystrokes */
        Xsyscnsl.c_init(&Xsyscnsl, FALSE);

        /*
         * RegisterKeystrokeNotify for the/any keystrokes we really care about,
         * then later in prefs_init() any custom ones
         */

        /* Screen */
        //screen_init(loadedimg, efi_systbl);

        /* Turn off the WatchdogTimer */
        efi_systbl->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

        return (events(efi_systbl));
}
