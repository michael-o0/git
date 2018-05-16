#include "env.h"
#include "globals.h"

struct
{
        struct env_efivariable **ptr;
        UINTN count;
        UINTN len;

} efivars = {NULL, 0, 0};

/*
 *
 */
INTN env_init(VOID)
{

        CHAR16 *varname = NULL;
        UINTN namelen = 0;
        UINTN varsidx = 0;
        EFI_GUID varguid = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};
        struct env_efivariable **p = NULL;
        UINTN varslen = 0;
        EFI_STATUS efirc;

        if (efivars.ptr != NULL) {
                env_exit(1);
        }

        efivars.count = 5;
        efivars.len = (sizeof(struct env_efivariable *) * efivars.count);
        efivars.ptr = AllocateZeroPool(efivars.len);

        varname = AllocateZeroPool(sizeof(CHAR16));

        do {
                if (varsidx == efivars.count) {
                        varslen = efivars.len
                                + (sizeof(struct env_efivariable *) * 5);
                        p = AllocateZeroPool(varslen);
                        BS->CopyMem(p, efivars.ptr, efivars.len);
                        BS->FreePool(efivars.ptr);
                        efivars.ptr = p;
                        p = NULL;
                        efivars.len = varslen;
                        efivars.count += 5;
                }

                efirc = RT->GetNextVariableName(&namelen, varname, &varguid);

                if ( EFI_BUFFER_TOO_SMALL == efirc) {
                        FREE_AND_NULL( varname )
                        ;
                        varname = AllocateZeroPool(namelen);
                        if (varname != NULL) {
                                efirc = RT->GetNextVariableName(&namelen,
                                        varname, &varguid);
                        }
                }

                if (!efirc) {
                        efirc = RT->GetVariable(varname, &varguid,
                                &efivars.ptr[varsidx]->attr,
                                &efivars.ptr[varsidx]->data_len,
                                efivars.ptr[varsidx]->data);
                        if ( EFI_BUFFER_TOO_SMALL == efirc) {
                                FREE_AND_NULL( efivars.ptr[varsidx]->data )
                                ;
                                efivars.ptr[varsidx]->data = AllocatePool(
                                        efivars.ptr[varsidx]->data_len);
                                if ((efivars.ptr[varsidx]->data) != NULL) {
                                        efirc =
                                                RT->GetVariable(varname, &varguid,
                                                        &efivars.ptr[varsidx]->attr,
                                                        &efivars.ptr[varsidx]->data_len,
                                                        efivars.ptr[varsidx]->data);
                                }
                        }
                        if (!efirc) {
                                efivars.ptr[varsidx]->name = AllocatePool(
                                        namelen);
                                StrCpy(efivars.ptr[varsidx]->name, varname);
                                efivars.ptr[varsidx]->guid = varguid;
                                /*Print(L"%s, %x, %d, %d\r\n",
                                 ub_env.efi_vars[count]->name,
                                 ub_env.efi_vars[count]->guid,
                                 ub_env.efi_vars[count]->attr,
                                 ub_env.efi_vars[count]->data_len);
                                 */varsidx++;
                        } else {
                                efivars.ptr[varsidx]->data_len = 0;
                                FREE_AND_NULL( efivars.ptr[varsidx]->data )
                                ;
                        }
                } else {
                        FREE_AND_NULL( varname )
                        ;
                }
        } while (efirc);

        FREE_AND_NULL( varname )
        ;

        return (0);
}

/*

 */
INTN env_exit(INTN dealloc)
{

        UINTN it;

        if ((efivars.ptr) != NULL) {
                for (it = 0; it < efivars.count; it++) {
                        FREE_AND_NULL( efivars.ptr[it]->name )
                        ;
                        FREE_AND_NULL( efivars.ptr[it]->data )
                        ;
                        FREE_AND_NULL( efivars.ptr[it] )
                        ;
                }
        }

        if (dealloc) {
                efivars.count = 0;
                FREE_AND_NULL( efivars.ptr )
                ;
        }

        return (0);
}
