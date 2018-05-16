#include "commands.h"
#include "parser.h"
#include "syscnsl.h"
#include "edid.h"
#include "globals.h"

static CHAR16 *prompt = L"efi-boot$ ";

/*
 EFI_STATUS efi_cons_protocol_probe_simple_txt_in(VOID);
 EFI_STATUS efi_cons_protocol_probe_simple_txt_in_ex(VOID);
 EFI_STATUS efi_cons_protocol_probe_simple_pointer(VOID);
 EFI_STATUS efi_cons_protocol_probe_absolute_pointer(VOID);
 EFI_STATUS efi_cons_protocol_probe_simple_txt_out(VOID);
 EFI_STATUS efi_cons_protocol_probe_console_control(VOID);
 EFI_STATUS efi_cons_protocol_probe_uga_draw(VOID);
 EFI_STATUS efi_cons_protocol_probe_graphics_output(VOID);
 */

EFI_STATUS PrintGOP(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop);

/*
 * Forward fn() prototypes
 */
static EFI_STATUS cmd_exit(INTN argc, CHAR16 **argv);
static EFI_STATUS cmd_prot(INTN argc, CHAR16 **argv);
static EFI_STATUS cmd_env(INTN argc, CHAR16 **argv);
static EFI_STATUS cmd_gop(INTN argc, CHAR16 **argv);

static struct cmd cmds[] = {
        {
                .name = L"exit",
                .desc = L"Exit",
                .func = cmd_exit
        },
        {
                .name = L"prot",
                .desc = L"print out supported protocols",
                .func = cmd_prot
        },
        {
                .name = L"efi-env",
                .desc = L"print out efi vars",
                .func = cmd_env
        },
        {
                .name = L"efi-gop",
                .desc = L"EFI Graphics Output Protocol info/get/set",
                .func = cmd_gop
        }};

static CONST INTN cmdscount = (sizeof(cmds) / sizeof(struct cmd));

/*
 *
 */
INTN commands(CHAR16 *charbuf)
{
        INTN it;
        INTN rc = 0;
        INTN argc = 0;
        CHAR16 **argv = NULL;
        BOOT_CMD_FCN *cmdfn = NULL;

        Print(L"\r\n");

        if (parse(charbuf, &argc, &argv)) {
                Print(L"efi-boot: error parsing %s\r\n", charbuf);
                rc = 1;
                goto done;
        }

        if (argc < 1) {
                rc = 1;
                goto done;
        }

        for (it = 0; it < cmdscount; it++) {
                if ((cmds[it].name != NULL)
                        && (!StrCmp(argv[0], cmds[it].name))) {
                        if (cmds[it].func != NULL) {
                                cmdfn = cmds[it].func;
                        }
                }
        }

        if (cmdfn != NULL) {
                Xprogname = argv[0];
                (cmdfn)(argc, argv);
                Xprogname = NULL;
        } else {
                rc = 1;
                Print(L"efi-boot: %s: command not found\r\n", argv[0]);
        }

done:
        Print(prompt);
        FreePool(argv);
        return (rc);
}

/*
 *
 */
EFI_STATUS cmd_prompt(INTN argc, CHAR16 **argv)
{
        (VOID) argc;
        (VOID) argv;

        Print(prompt);

        return (EFI_SUCCESS);
}

/*
 *
 */
static EFI_STATUS cmd_exit(INTN argc, CHAR16 **argv)
{
        ( VOID ) argc;
        ( VOID ) argv;

        parser_exit(1);

        Xkeeplooping = 0;

        return (EFI_SUCCESS);
}

/*

 */
static EFI_STATUS cmd_env(INTN argc, CHAR16 **argv)
{
        CHAR16 var[EFI_MAXIMUM_VARIABLE_SIZE], err_str[256];

        UINTN var_len, count = 0;

        EFI_GUID var_guid = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};

        EFI_STATUS efirc;

        (VOID) argc;

        efirc = RT->GetNextVariableName(&var_len, NULL, NULL);

        var[0] = '\0';

        do {
                var_len = EFI_MAXIMUM_VARIABLE_SIZE;

                efirc = RT->GetNextVariableName(&var_len, var, &var_guid);

                if (!efirc) {
                        Print(L"%u N:%s L:%u G:%x\r\n", count++, var, var_len,
                                var_guid);
                } else if (efirc != EFI_NOT_FOUND) {
                        StatusToString(&err_str[0], efirc);
                        Print(L"%s: error: %s\r\n", argv[0], &err_str);
                }

        } while (efirc != EFI_NOT_FOUND);

        return (EFI_SUCCESS);
}

/* */
static EFI_STATUS cmd_gop(INTN argc, CHAR16 **argv)
{
        (VOID) argc;

        if (Xprtcl_gop == NULL) {
                Print(L"%s: GOP not supported.\r\n", argv[0]);
                return (0);
        }

        PrintGOP(Xprtcl_gop);

        /*ub_console.protocols.con_ctrl.current = NULL;

         efirc = BS->HandleProtocol(RZR_EFI_CONOUT_HANDLE,
         &rzr_EfiGraphicsOutputProtocolGuid,
         (VOID **)&ub_console.protocols.con_ctrl.current);

         if (!EFI_ERROR(efirc))
         {
         ub_console.flags |= EFI_CONSOLE_DOES_CCP;
         }
         else
         {
         ub_console.protocols.con_ctrl.current = NULL;
         }*/
        return (EFI_SUCCESS);
}

/* */

INTN memcmp(const VOID *s1, const VOID *s2, UINTN n)
{
        const unsigned char *c1 = s1, *c2 = s2;
        INTN d = 0;

        if (!s1 && !s2) return (0);
        if (s1 && !s2) return (1);
        if (!s1 && s2) return (-1);

        while (n--) {
                d = (INTN) *c1++ - (INTN) *c2++;
                if (d) break;
        }

        return (d);
}

#if 0
EFI_STATUS
efi_cons_protocol_probe_simple_txt_out(VOID)
{
        EFI_STATUS efirc = EFI_SUCCESS;

        ub_console.protocols.simple_txt_out.current = NULL;

        efirc = (BS->HandleProtocol)(RZR_EFI_CONOUT_HANDLE,
                &estopg,
                ( VOID ** ) &ub_console.protocols.simple_txt_out.current);

        if (!EFI_ERROR(efirc))
        {
                ub_console.flags |= EFI_CONSOLE_DOES_STOP;
        }
        else
        {
                ub_console.protocols.simple_txt_out.current = NULL;

        }

        ub_console.protocols.simple_txt_out.handles.buffp = NULL;
        ub_console.protocols.simple_txt_out.handles.count = 0;

        efirc = (BS->LocateHandleBuffer)(ByProtocol,
                &estopg,
                NULL,
                &ub_console.protocols.simple_txt_out.handles.count,
                &ub_console.protocols.simple_txt_out.handles.buffp);

        if (EFI_ERROR(efirc))
        {
                if (NULL != ub_console.protocols.simple_txt_out.handles.buffp)
                {
                        (BS->FreePool)(ub_console.protocols.simple_txt_out.handles.buffp);
                }
                ub_console.protocols.simple_txt_out.handles.buffp = NULL;
                ub_console.protocols.simple_txt_out.handles.count = 0;
        }

        return ( efirc );
}

EFI_STATUS
efi_cons_protocol_probe_console_control(VOID)
{
        EFI_STATUS efirc = EFI_SUCCESS;

        ub_console.protocols.con_ctrl.current = NULL;

        efirc = (BS->HandleProtocol)(RZR_EFI_CONOUT_HANDLE,
                &eccpg,
                ( VOID ** ) &ub_console.protocols.con_ctrl.current);

        if (!EFI_ERROR(efirc))
        {
                ub_console.flags |= EFI_CONSOLE_DOES_CCP;
        }
        else
        {
                ub_console.protocols.con_ctrl.current = NULL;
        }

        ub_console.protocols.con_ctrl.handles.buffp = NULL;
        ub_console.protocols.con_ctrl.handles.count = 0;

        efirc = (BS->LocateHandleBuffer)(ByProtocol,
                &eccpg,
                NULL,
                &ub_console.protocols.con_ctrl.handles.count,
                &ub_console.protocols.con_ctrl.handles.buffp);

        if (EFI_ERROR(efirc))
        {
                if (NULL != ub_console.protocols.con_ctrl.handles.buffp)
                {
                        (BS->FreePool)(ub_console.protocols.con_ctrl.handles.buffp);
                }
                ub_console.protocols.con_ctrl.handles.buffp = NULL;
                ub_console.protocols.con_ctrl.handles.count = 0;
        }

        return ( efirc );
}

EFI_STATUS
efi_cons_protocol_probe_uga_draw(VOID)
{
        EFI_STATUS efirc = EFI_SUCCESS;

        ub_console.protocols.uga.current = NULL;

        efirc = (BS->HandleProtocol)(RZR_EFI_CONOUT_HANDLE,
                &eudpg,
                ( VOID ** ) &ub_console.protocols.uga.current);

        if (!EFI_ERROR(efirc))
        {
                ub_console.flags |= EFI_CONSOLE_DOES_UGA;
        }
        else
        {
                ub_console.protocols.uga.current = NULL;
        }

        ub_console.protocols.uga.handles.buffp = NULL;
        ub_console.protocols.uga.handles.count = 0;

        efirc = (BS->LocateHandleBuffer)(ByProtocol,
                &eudpg,
                NULL,
                &ub_console.protocols.uga.handles.count,
                &ub_console.protocols.uga.handles.buffp);

        if (EFI_ERROR(efirc))
        {
                if (NULL != ub_console.protocols.uga.handles.buffp)
                {
                        (BS->FreePool)(ub_console.protocols.uga.handles.buffp);
                }
                ub_console.protocols.uga.handles.buffp = NULL;
                ub_console.protocols.uga.handles.count = 0;
        }

        return ( efirc );
}

EFI_STATUS
efi_cons_protocol_probe_graphics_output(VOID)
{
        EFI_STATUS efirc = EFI_SUCCESS;

        ub_console.protocols.gop.current = NULL;

        efirc = (BS->HandleProtocol)(RZR_EFI_CONOUT_HANDLE,
                &egopg,
                ( VOID ** ) &ub_console.protocols.gop.current);

        if (!EFI_ERROR(efirc ))
        {
                ub_console.flags |= EFI_CONSOLE_DOES_GOP;
        }
        else
        {
                ub_console.protocols.gop.current = NULL;
        }

        ub_console.protocols.gop.handles.buffp = NULL;
        ub_console.protocols.gop.handles.count = 0;

        efirc = (BS->LocateHandleBuffer)(ByProtocol,
                &egopg,
                NULL,
                &ub_console.protocols.gop.handles.count,
                &ub_console.protocols.gop.handles.buffp);
        if ( EFI_ERROR ( efirc ) )
        {
                if ( ub_console.protocols.gop.handles.buffp != NULL )
                {
                        (BS->FreePool)(ub_console.protocols.gop.handles.buffp);
                }
                ub_console.protocols.gop.handles.buffp = NULL;
                ub_console.protocols.gop.handles.count = 0;
        }

        return ( efirc );
}

EFI_STATUS
efi_cons_protocol_probe_simple_pointer(VOID)
{
        EFI_STATUS efirc = EFI_SUCCESS;

        ub_console.protocols.simple_pointer.current = NULL;

        efirc = (BS->HandleProtocol)(RZR_EFI_CONIN_HANDLE,
                &esppg,
                ( VOID ** ) &ub_console.protocols.simple_pointer.current);

        if (!EFI_ERROR(efirc))
        {
                ub_console.flags |= EFI_CONSOLE_DOES_SPP;
        }
        else
        {
                ub_console.protocols.simple_pointer.current = NULL;

        }

        ub_console.protocols.simple_pointer.handles.buffp = NULL;
        ub_console.protocols.simple_pointer.handles.count = 0;

        efirc = (BS->LocateHandleBuffer)(ByProtocol,
                &esppg,
                NULL,
                &ub_console.protocols.simple_pointer.handles.count,
                &ub_console.protocols.simple_pointer.handles.buffp);
        if ( EFI_ERROR ( efirc ) )
        {
                if ( ub_console.protocols.simple_pointer.handles.buffp != NULL )
                {
                        BS->FreePool(ub_console.protocols.simple_pointer.handles.buffp);
                }
                ub_console.protocols.simple_pointer.handles.buffp = NULL;
                ub_console.protocols.simple_pointer.handles.count = 0;
        }
        else
        {
                efirc = (BS->HandleProtocol)(ub_console.protocols.simple_pointer.handles.buffp[0],
                        &esppg,
                        ( VOID ** ) &ub_console.protocols.simple_pointer.current);
                if (EFI_ERROR(efirc))
                {
                        ub_console.protocols.simple_pointer.current = NULL;
                }
        }

        return ( efirc );
}

EFI_STATUS
efi_cons_protocol_probe_absolute_pointer(VOID)
{
        EFI_STATUS efirc = EFI_SUCCESS;

        ub_console.protocols.absolute_pointer.current = NULL;
        ub_console.protocols.absolute_pointer.handles.buffp = NULL;
        ub_console.protocols.absolute_pointer.handles.count = 0;

        efirc = (BS->LocateHandleBuffer)(ByProtocol,
                &eappg,
                NULL,
                &ub_console.protocols.absolute_pointer.handles.count,
                &ub_console.protocols.absolute_pointer.handles.buffp);
        if ( EFI_ERROR ( efirc ) )
        {
                if ( ub_console.protocols.absolute_pointer.handles.buffp != NULL )
                {
                        (BS->FreePool)(ub_console.protocols.absolute_pointer.handles.buffp);
                }
                ub_console.protocols.absolute_pointer.handles.buffp = NULL;
                ub_console.protocols.absolute_pointer.handles.count = 0;
        }
        else
        {
                efirc = (BS->HandleProtocol)(ub_console.protocols.absolute_pointer.handles.buffp[0],
                        &eappg,
                        ( VOID ** ) &ub_console.protocols.absolute_pointer.current);
                if ( EFI_ERROR ( efirc ) )
                {
                        ub_console.protocols.absolute_pointer.current = NULL;
                }
        }

        return ( efirc );
}
#endif /* if 0/1 */

EFI_STATUS PrintCCP(EFI_CONSOLE_CONTROL_PROTOCOL *efi_conctrl_protocol_p)
{
        EFI_STATUS efi_res = EFI_SUCCESS;

        EFI_CONSOLE_CONTROL_SCREEN_MODE ccp_mode;

        BOOLEAN uga_present, StdInLocked;

        efi_res = efi_conctrl_protocol_p->GetMode(efi_conctrl_protocol_p,
                &ccp_mode, &uga_present, &StdInLocked);

        if (EFI_ERROR(efi_res)) {
                Print(L"CCP ERR(%d) Could not get mode information on %xh\r\n",
                        efi_res, efi_conctrl_protocol_p);
                return (efi_res);
        }

        Print(L"Screen mode: ");

        switch (ccp_mode)
        {
        case EfiConsoleControlScreenText:
                Print(L"Text");
                break;
        case EfiConsoleControlScreenGraphics:
                Print(L"Graphics");
                break;
        case EfiConsoleControlScreenMaxValue:
                Print(L"MaxValue");
                break;
        }

        Print(L"\r\n");

        putc8("UGA Present: ");

        if (uga_present) {
                Print(L"Yes");
        } else {
                Print(L"No");
        }

        putc8("\r\n");

        return (EFI_SUCCESS);
}

EFI_STATUS PrintUGA(EFI_UGA_DRAW_PROTOCOL *efi_uga_draw_protocol_p)
{
        EFI_STATUS efi_res = EFI_SUCCESS;
        UINT32 HorzResolution = 0;
        UINT32 VertResolution = 0;
        UINT32 ColorDepth = 0;
        UINT32 RefreshRate = 0;

        efi_res = efi_uga_draw_protocol_p->GetMode(efi_uga_draw_protocol_p,
                &HorzResolution, &VertResolution, &ColorDepth, &RefreshRate);

        if (EFI_ERROR(efi_res)) {
                Print(L"ERROR: UGA GetMode failed [%d]\n", efi_res);
        } else {
                Print(L"Horizontal Resolution: %d\n", HorzResolution);
                Print(L"Vertical Resolution: %d\n", VertResolution);
                Print(L"Color Depth: %d\n", ColorDepth);
                Print(L"Refresh Rate: %d\n", RefreshRate);
                Print(L"\n");
        }

        return (efi_res);
}

EFI_STATUS PrintGOP(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop)
{
        UINT32 i, imax;
        EFI_STATUS efi_res;

        imax = gop->Mode->MaxMode;

        Print(L"GOP reports MaxMode %d\n", imax);

        for (i = 0; i < imax; i++) {
                EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
                UINTN SizeOfInfo;

                efi_res = gop->QueryMode(gop, i, &SizeOfInfo, &Info);
                if ( EFI_ERROR(efi_res) && efi_res == EFI_NOT_STARTED) {
                        gop->SetMode(gop, gop->Mode->Mode);
                        efi_res = gop->QueryMode(gop, i, &SizeOfInfo, &Info);
                }

                if (EFI_ERROR(efi_res)) {
                        Print(L"ERROR: Bad response from QueryMode: %d\n",
                                efi_res);
                        continue;
                }
                Print(L"%c%d: %dx%d ",
                        memcmp(Info, gop->Mode->Info, sizeof(*Info)) == 0 ?
                                '*' : ' ', i, Info->HorizontalResolution,
                        Info->VerticalResolution);
                switch (Info->PixelFormat)
                {
                case PixelRedGreenBlueReserved8BitPerColor:
                        Print(L"RGBRerserved");
                        break;
                case PixelBlueGreenRedReserved8BitPerColor:
                        Print(L"BGRReserved");
                        break;
                case PixelBitMask:
                        Print(L"Red:%08x Green:%08x Blue:%08x Reserved:%08x",
                                Info->PixelInformation.RedMask,
                                Info->PixelInformation.GreenMask,
                                Info->PixelInformation.BlueMask,
                                Info->PixelInformation.ReservedMask);
                        break;
                case PixelBltOnly:
                        Print(L"(blt only)");
                        break;
                default:
                        Print(L"(Invalid pixel format)");
                        break;
                }
                Print(L" Pixels %d\n", Info->PixelsPerScanLine);
        }
        Print(L"\n");

        return (EFI_SUCCESS);
}

/*

 */
EFI_STATUS cmd_prot(INTN argc, CHAR16 **argv)
{
        (VOID) argc;
        (VOID) argv;

        if (Xsyscnsl_ctx.prtcl_stxt_in != NULL)
                ST->ConOut->OutputString(ST->ConOut, L"stxt_in\r\n");

        if (Xsyscnsl_ctx.prtcl_stxt_in_ex != NULL)
                ST->ConOut->OutputString(ST->ConOut, L"stxt_in_ex\r\n");

        if (Xsyscnsl_ctx.prtcl_sptr != NULL)
                ST->ConOut->OutputString(ST->ConOut, L"sptr\r\n");

        if (Xsyscnsl_ctx.prtcl_abs_ptr != NULL)
                ST->ConOut->OutputString(ST->ConOut, L"abs_ptr\r\n");

        if (Xsyscnsl_ctx.prtcl_stxt_out != NULL)
                ST->ConOut->OutputString(ST->ConOut, L"stxt_out\r\n");

        if (Xsyscnsl_ctx.prtcl_con_ctrl != NULL)
                ST->ConOut->OutputString(ST->ConOut, L"con_ctrl\r\n");

        if (Xsyscnsl_ctx.prtcl_uga_draw != NULL)
                ST->ConOut->OutputString(ST->ConOut, L"uga_draw\r\n");

        if (Xsyscnsl_ctx.prtcl_gop != NULL)
                ST->ConOut->OutputString(ST->ConOut, L"gop\r\n");

        return (EFI_SUCCESS);
}

#if 0
/*
 Print (L" TXT(%d)", ub_console.protocols.simple_txt_out.handles.count );
 Print (L" CCP(%d)", ub_console.protocols.con_ctrl.handles.count );
 Print (L" UGA(%d)", ub_console.protocols.uga.handles.count );
 Print (L" GOP(%d)", ub_console.protocols.gop.handles.count );
 Print (L" SPP(%d)", ub_console.protocols.simple_pointer.handles.count );
 Print (L" APP(%d)", ub_console.protocols.absolute_pointer.handles.count );
 ST->ConOut->OutputString ( ST->ConOut, L"\r\n" );

 if ( NULL != ub_console.Xprtcl_abs_ptr )
 {
 EFI_STATUS  efi_res = EFI_SUCCESS;
 EFI_ABSOLUTE_POINTER_STATE  app_state;


 }

 if (NULL != ub_console.Xprtcl_sptr)
 {
 EFI_STATUS  efi_res = EFI_SUCCESS;
 EFI_SIMPLE_POINTER_MODE     *spp_mode;
 EFI_SIMPLE_POINTER_STATE    spp_state;

 spp_mode = ub_console.Xprtcl_sptr->Mode;
 DPRINTF(L"SPP:\n","");
 DPRINTF(L" - ResolutionX=%ld\n", spp_mode->ResolutionX);
 DPRINTF(L" - ResolutionY=%ld\n", spp_mode->ResolutionY);
 DPRINTF(L" - ResolutionZ=%ld\n", spp_mode->ResolutionZ);
 DPRINTF(L" - Left button %a\n", spp_mode->LeftButton?L"YES":L"NO");
 DPRINTF(L" - Right button %a\n", spp_mode->RightButton?L"YES":L"NO");
 DPRINTF(L" - WaitForInput %a\n", ub_console.Xprtcl_sptr->WaitForInput?L"YES":L"NO");
 }
 */

UINT32 header_crc = 0, crc = 0;

/** Verify EfiSystemTable
 */
if ( EfiSystemTable->Hdr.HeaderSize == 0 )
{
        return ( 1 );
}
header_crc = EfiSystemTable->Hdr.CRC32;
EfiSystemTable->Hdr.CRC32 = 0;
//crc = CalculateCheckSum32 ( ( UINT32 * ) EfiSystemTable, EfiSystemTable->Hdr.HeaderSize );
EfiBootServices->CalculateCrc32 ( EfiSystemTable, EfiSystemTable->Hdr.HeaderSize, &crc );

if ( crc != header_crc )
{
        return ( 1 );
}

/** Verify EfiBootServices
 */
header_crc = EfiBootServices->Hdr.CRC32;
EfiBootServices->Hdr.CRC32 = 0;
EfiBootServices->CalculateCrc32 ( EfiBootServices, EfiBootServices->Hdr.HeaderSize, &crc );

if ( crc != header_crc )
{
        return ( 1 );
}

/** Verify EfiRuntimeServices
 */
header_crc = EfiRuntimeServices->Hdr.CRC32;
EfiRuntimeServices->Hdr.CRC32 = 0;
EfiBootServices->CalculateCrc32 ( EfiRuntimeServices, EfiRuntimeServices->Hdr.HeaderSize, &crc );

if ( crc != header_crc )
{
        return ( 1 );
}

EfiSystemTable = efi_system_table_p;
EfiBootServices = EfiSystemTable->BootServices;
EfiRuntimeServices = EfiSystemTable->RuntimeServices;

EfiApp = efi_image_h;

header_crc = EfiSystemTable->Hdr.CRC32;
EfiSystemTable->Hdr.CRC32 = 0;
EfiBootServices->CalculateCrc32 ( EfiSystemTable, EfiSystemTable->Hdr.HeaderSize, &crc );

if ( crc != header_crc )
{
        return ( -1 );
}
//__asm__ ( "mov %%rax, %%rbx; cli; hlt" : : "a" ( header_crc ) );
#endif
