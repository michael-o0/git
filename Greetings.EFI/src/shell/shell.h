#ifndef UB_SHELL_H
#define UB_SHELL_H

#include <efi/efi.h>
#include <efi/efilib.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern INTN Xub_shell_running ;

INTN ub_shell_run ( VOID ) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ifndef UB_SHELL_H */
