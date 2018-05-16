#ifndef UB_COMMANDS_H
#define UB_COMMANDS_H

#include <efi/efi.h>
#include <efi/efilib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef EFI_STATUS (BOOT_CMD_FCN)(INTN argc, CHAR16 **argv);

struct cmd
{
    CONST CHAR16 *name;
    CONST CHAR16 *desc;
    BOOT_CMD_FCN *func;
};

INTN commands(CHAR16 *charbuf);
EFI_STATUS cmd_prompt(INTN argc, CHAR16 **argv);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* not UB_COMMANDS_H */

/* IDEA: HotKeys can be done with Xprtcl_stxt_in_ex->RegisterKeyNotify */
