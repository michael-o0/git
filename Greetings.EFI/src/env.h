#ifndef UB_ENV_H
#define UB_ENV_H

#include <efi/efi.h>
#include <efi/efilib.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

struct env_variable
{
        CHAR16 *name;
        CHAR16 *val;

};

struct env_efivariable
{
        CHAR16 *name;

        EFI_GUID guid;

        UINT32 attr;

        UINTN data_len;

        UINT8 *data;
};

INTN env_init(VOID);
INTN env_exit(INTN dealloc);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !UB_ENV_H */
