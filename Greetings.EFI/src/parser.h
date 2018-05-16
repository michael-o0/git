#ifndef UB_PARSER_H
#define UB_PARSER_H

#include <efi/efi.h>
#include <efi/efilib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

INTN parser_init(VOID);
INTN parser_exit(INTN dealloc);
INTN parse(CHAR16 *str, INTN *argc, CHAR16 ***argv);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ifndef UB_PARSER_H */
