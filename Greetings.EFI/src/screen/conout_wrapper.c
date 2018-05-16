#include "../globals.h"
#include "../syscnsl.h"

EFI_GUID guid_prtcl_stxt_out = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;

EFI_STATUS
reset(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, BOOLEAN ExtendedVerification);
EFI_STATUS
outputstr(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *WString);
EFI_STATUS
teststr(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 * WString);
EFI_STATUS
querymode(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN ModeNumber,
        UINTN * Columns, OUT UINTN * Rows);
EFI_STATUS
setmode(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN ModeNumber);
EFI_STATUS
setattr(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN Attribute);
EFI_STATUS
clear(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
EFI_STATUS
setcaretpos(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL * This, UINTN Column, UINTN Row);
EFI_STATUS
togglecaret(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL * This, BOOLEAN Visible);

EFI_HANDLE orig_conouthand;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *orig_conout = NULL;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL cust_conout = {
        .Reset = reset,
        .OutputString = outputstr,
        .TestString = teststr,
        .QueryMode = querymode,
        .SetMode = setmode,
        .SetAttribute = setattr,
        .ClearScreen = clear,
        .SetCursorPosition = setcaretpos,
        .EnableCursor = togglecaret};

EFI_STATUS reset(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
        BOOLEAN ExtendedVerification)
{
        return (0);
}

EFI_STATUS outputstr(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *WString)
{
        return (0);
}

EFI_STATUS teststr(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *WString)
{
        return (0);
}

EFI_STATUS querymode(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN ModeNumber,
        UINTN *Columns, UINTN *Rows)
{
        return (0);
}

EFI_STATUS setmode(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN ModeNumber)
{
        return (0);
}

EFI_STATUS setattr(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN Attribute)
{
        return (0);
}

EFI_STATUS clear(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This)
{
        return (0);
}

EFI_STATUS setcaretpos(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN Column,
        UINTN Row)
{
        return (0);
}

EFI_STATUS togglecaret(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, BOOLEAN Visible)
{
        return (0);
}

/*
 * Replace ConOut with a custom implementation
 */
EFI_STATUS ConOutWrapperInit(EFI_HANDLE imghandle, EFI_SYSTEM_TABLE *systab)
{
        EFI_STATUS efirc;

        orig_conouthand = systab->ConsoleOutHandle;
        orig_conout = systab->ConOut;

        efirc = systab->BootServices->InstallProtocolInterface(&imghandle,
                &guid_prtcl_stxt_out, EFI_NATIVE_INTERFACE,
                &cust_conout);

        if (EFI_ERROR(efirc)) {
                return (efirc);
        }

        cust_conout.Mode = systab->ConOut->Mode;

        systab->ConsoleOutHandle = imghandle;
        systab->ConOut = &cust_conout;

        systab->Hdr.CRC32 = 0;
        systab->BootServices->CalculateCrc32(&systab->Hdr,
                systab->Hdr.HeaderSize, &systab->Hdr.CRC32);

        return (efirc);
}

/*
 * Restore ConOut
 */
EFI_STATUS ConOutWrapperExit(EFI_HANDLE imghandle, EFI_SYSTEM_TABLE *systab)
{
        systab->ConsoleOutHandle = orig_conouthand;
        systab->ConOut = orig_conout;

        systab->Hdr.CRC32 = 0;
        systab->BootServices->CalculateCrc32(&systab->Hdr,
                systab->Hdr.HeaderSize, &systab->Hdr.CRC32);

        return (systab->BootServices->UninstallProtocolInterface(&imghandle,
                &guid_prtcl_stxt_out, &cust_conout));
}
