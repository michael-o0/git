#if 0
#include "efi_protocols/UsbIo.h"
#include "ub_wides.h"

#include <efi/efi.h>
#include <efi/efilib.h>

#define CLASS_HID 3
#define SUBCLASS_BOOT 1
#define PROTOCOL_MOUSE 2

BOOLEAN IsUsbMouse( __in EFI_USB_IO_PROTOCOL * UsbIo )
{
    EFI_USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
    EFI_STATUS efirc;

    //
    // Get the default interface descriptor
    //
    efirc = UsbIo->UsbGetInterfaceDescriptor ( UsbIo , &InterfaceDescriptor );

    if ( EFI_ERROR( efirc ) )
    {
        return (FALSE);
    }

    if ( (CLASS_HID == InterfaceDescriptor.InterfaceClass)
            && (SUBCLASS_BOOT == InterfaceDescriptor.InterfaceSubClass)
            && (PROTOCOL_MOUSE == InterfaceDescriptor.InterfaceProtocol) )
    {
        return (TRUE);
    }

    return (FALSE);
}

EFI_STATUS DriverLoadUsbMouse(
__in VOID
)
{
    EFI_DEVICE_PATH *dvcpath = NULL;

    EFI_GUID UsbIoGuid = EFI_USB_IO_PROTOCOL_GUID;

    EFI_HANDLE * handles = NULL;
    EFI_HANDLE driver_handle;

    UINTN handles_count;
    UINTN i;

    EFI_STATUS efirc;

    /*  Get all SimpleFileSystemProtocol handles */
    efirc = BS->LocateHandleBuffer ( ByProtocol , &Xguid_prtcl_sfs ,
    NULL , &handles_count , &handles );

    if ( EFI_ERROR( efirc ) )
    {
        DPRINTF( L"Err getting sfsp handles\r\n" , "" );
        goto out;
    }

    DPRINTF( L"Inf got %d sfsp handles\r\n" , handles_count );

    for (i = 0; i < handles_count ; i++)
    {
        /*  Look for the given File given a FileName */
        dvcpath = FileDevicePath ( handles[i] ,
                L"\\EFI\\PM\\DXE\\USB\\MOUSEX64.EFI" );

        if ( dvcpath == NULL )
        {
            continue; /*  Not here, continue */
        }

        DPRINTF( L"Inf Potentian driver\r\n" , "" );

        /*  Attempt to load the driver */
        efirc = BS->LoadImage ( FALSE , Ximg , dvcpath , NULL , 0 ,
                &driver_handle );

        EB_FREE( dvcpath )
        ;

        if ( EFI_ERROR( efirc ) )
        {
            DPRINTF( L"Err could not load driver\r\n" , "" );
            continue; /*  Possibly not the right driver */
        }

        DPRINTF( L"Inf Potentian start driver\r\n" , "" );

        /*  Attempt to start the driver */
        efirc = BS->StartImage ( driver_handle , NULL , NULL );

        if ( EFI_ERROR( efirc ) )
        {
            DPRINTF( L"Err could not start driver\r\n" , "" );
            goto out;
        }

        break; /*  We got one,  */
    }

    EB_FREE( handles )
    ;

    if ( i >= handles_count )
    {
        DPRINTF( L"Err did not find file(s)\r\n" , "" );
        efirc = EFI_NOT_FOUND;
        goto out;
    }

    /*  Get all UsbIoProtocol handles */
    efirc = BS->LocateHandleBuffer ( ByProtocol , &UsbIoGuid ,
    NULL , &handles_count , &handles );

    if ( EFI_ERROR( efirc ) )
    {
        DPRINTF( L"Err getting usbio handles\r\n" , "" );
        goto out;
    }

    for (i = 0; i < handles_count ; i++)
    {
        if ( IsUsbMouse ( handles[i] ) )
        {
            efirc = BS->ConnectController ( handles[i] , NULL , NULL , TRUE );
            DPRINTF( L"Inf possibly got one\r\n" , "" );
        }
    }

    out:
    EB_FREE( handles )
    ;
    return (efirc);
}

/* */
EFI_STATUS rzr_efi_shell_command_efi_mse( __in INTN argc, __in CHAR16 ** argv )
{

    (VOID) argc;
    (VOID) argv;

    DPRINTF( L"%s: Please wait..\r\n" , Xprogname );

    DriverLoadUsbMouse ( );

    return (0);
}
#endif
