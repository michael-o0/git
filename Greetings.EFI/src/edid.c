#include "edid.h"

#include "globals.h"

int CheckForValidEdid( EFI_EDID_DISCOVERED_PROTOCOL *Edid )
{
    EDID_DATA_BLOCK *EdidDataBlock = (EDID_DATA_BLOCK *) Edid->Edid;
    UINT8 *edid = (UINT8 *) Edid->Edid;
    INTN i;
    UINTN checksum = 0;
    const UINT8 EdidHeader[] =
    { 0x00 , 0xff , 0xff , 0xff , 0xff , 0xff , 0xff , 0x00 };

    for (i = 0; i < EDID_LENGTH ; i++)
    {
        checksum += edid[i];
    }

    if ( (UINT8) checksum != 0 )
    {
        return (1);
    }

    if ( *edid == 0x00 )
    {
        checksum = 0;
        for (i = 0; i < 8 ; i++)
        {
            if ( *edid++ == EdidHeader[i] )
                checksum++;
        }
        if ( checksum != 8 )
        {
            return (1);
        }
    }

    if ( EdidDataBlock->EdidVersion != 1 || EdidDataBlock->EdidRevision > 4 )
    {
        return (1);
    }

    return (0);
}

VOID efi_edid_query( EFI_GUID uga_or_gop_guid )
{
    EFI_GUID edid_guid = EFI_EDID_DISCOVERED_PROTOCOL_GUID;

    EFI_EDID_DISCOVERED_PROTOCOL
    *edid_discovered = NULL;

    EFI_HANDLE
    *handles = NULL;

    UINTN
    i ,
    handles_count = 0;

    EFI_STATUS efirc;

    DPRINTF( L"efi_edid_query:\r\n" , "" );

    efirc = BS->LocateHandleBuffer ( ByProtocol , &uga_or_gop_guid ,
    NULL , &handles_count , &handles );

    for (i = 0; i < handles_count ; i++)
    {
        efirc = BS->OpenProtocol ( handles[i] , &edid_guid ,
                (VOID **) &edid_discovered , Xefi_loaded_imgage ,
                NULL ,
                EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL );

        if ( EFI_ERROR( efirc ) )
        {
            continue;
        }

        if ( CheckForValidEdid ( edid_discovered ) )
        {
            Print ( L"Not a valid EDID\r\n" );
        }
        else
        {
            Print ( L"Has EDID\r\n" );
        }
    }

    FREE_AND_NULL( handles )
    ;
}

VOID efi_edid(
VOID )
{
//    if ( Xprtcl_uga_draw != NULL )
    {
        efi_edid_query ( Xguid_ptrcl_uga_draw );
    }

    //if ( Xprtcl_gop != NULL )
    {
        efi_edid_query ( Xguid_prtcl_gop );
    }
}
