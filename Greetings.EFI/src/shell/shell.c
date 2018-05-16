#if 0
#include "../headers/shell/shell.h"
/*
#include "ub_console.h"
#include "ub_env.h"
#include "ub_fs.h"
#include "ub_parser.h"
#include "ub_commands.h"
#include "ub_wides.h"

#include <efi/efi.h>
#include <efi/efilib.h>
*/
INTN Xub_shell_running = 1;

extern const INTN Xshell_cmd_count;
extern struct ub_cmd Xshell_cmds[];
struct gstream
{
    void *contents;
    uint64_t type, length, wpos;
} shell_input;
CHAR16 *prompt = L"efi-boot$ ";

/*
 * Forward prototypes
 */
INTN _do_input( VOID );
INTN _do_pointer_poll( VOID );
INTN _parse_input( VOID );
INTN _dmy_to_day_of_week( INTN /* day */, INTN /* month */, INTN /* year */);

INTN ub_shell_run( VOID )
{
    CHAR16 *wdays[7] =
    { L"Sun" , L"Mon" , L"Tue" , L"Wed" , L"Thu" , L"Fri" , L"Sat" };

    CHAR16 *mnths[12] =
    { L"Jan" , L"Feb" , L"Mar" , L"Apr" , L"May" , L"Jun" , L"Jul" , L"Aug" ,
            L"Sep" , L"Oct" , L"Nov" , L"Dec" };

    INTN wday_idx = 0; /* EFI_TIME.Day is 1<->7, init to 0 to cause a check on first run */
    INTN curr_day = 0;

    enum
    {
        EVTIDX_KEYPRESS = 0,
        EVTIDX_PTRDVC_INPUT,
        EVTIDX_PERIODIC,
        /* KEEP LAST */
        EVTIDX_COUNT
    };

    EFI_EVENT evts[EVTIDX_COUNT] = { NULL };

    UINTN evt_idx, curx, cury;

    EFI_TIME tm;

    EFI_STATUS efirc;

    //      BS->SetMem(evts, 0, sizeof(EFI_EVENT) * EVTS_COUNT);

    if ( !(efirc = BS->CreateEvent ( EVT_TIMER , 0 , NULL , NULL ,
            &evts[EVTIDX_PERIODIC] )) )
    {
        BS->SetTimer ( evts[EVTIDX_PERIODIC] , TimerPeriodic , 10000000 );
    }
    else
    {
        evts[EVTIDX_PERIODIC] = NULL;
    }

    if ( (Xprtcl_stxt_in_ex != NULL)
            && ((Xprtcl_stxt_in_ex->WaitForKeyEx) != NULL) )
    {
        evts[EVTIDX_KEYPRESS] = Xprtcl_stxt_in_ex->WaitForKeyEx;
    }
    else if ( (ST->ConIn->WaitForKey) != NULL )
    {
        evts[EVTIDX_KEYPRESS] = ST->ConIn->WaitForKey;
    }

    if ( (Xprtcl_sptr != NULL) && ((Xprtcl_sptr->WaitForInput) != NULL) )
    {
        evts[EVTIDX_PTRDVC_INPUT] = Xprtcl_sptr->WaitForInput;
    }

    // Thread?

    /*Print(L"%s BOOTSYS, r%u\r\n",
     ST->FirmwareVendor,
     ST->FirmwareRevision);
     */

    ST->ConOut->SetAttribute ( ST->ConOut ,
    EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY );

    ST->ConOut->ClearScreen ( ST->ConOut );
    /*
     Print(L"# EfiShell");

     ST->ConOut->SetAttribute(ST->ConOut, EFI_RED | EFI_BACKGROUND_RED);
     PrintAt(0, 1, L" ");
     ST->ConOut->SetAttribute(ST->ConOut, EFI_YELLOW | (EFI_BACKGROUND_GREEN | EFI_BACKGROUND_RED));
     Print(L" ");
     ST->ConOut->SetAttribute(ST->ConOut, EFI_GREEN | EFI_BACKGROUND_GREEN);
     Print(L" ");

     ST->ConOut->SetAttribute(ST->ConOut, EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY);

     PrintAt(36, 1, L"Hello\r\n$ ");
     */
    Print ( prompt );

#if 1
    EFI_UGA_PIXEL EfiColors[16] =
    {
    { 0x00 , 0x00 , 0x00 , 0x00 } ,  // BLACK
            { 0x98 , 0x00 , 0x00 , 0x00 } ,  // BLUE
            { 0x00 , 0x98 , 0x00 , 0x00 } ,  // GREEN
            { 0x98 , 0x98 , 0x00 , 0x00 } ,  // CYAN
            { 0x00 , 0x00 , 0x98 , 0x00 } ,  // RED
            { 0x98 , 0x00 , 0x98 , 0x00 } ,  // MAGENTA
            { 0x00 , 0x98 , 0x98 , 0x00 } ,  // BROWN
            { 0x98 , 0x98 , 0x98 , 0x00 } ,  // LIGHTGRAY
            { 0x30 , 0x30 , 0x30 , 0x00 } ,  // DARKGRAY - BRIGHT BLACK
            { 0xff , 0x00 , 0x00 , 0x00 } ,  // LIGHTBLUE - ?
            { 0x00 , 0xff , 0x00 , 0x00 } ,  // LIGHTGREEN - ?
            { 0xff , 0xff , 0x00 , 0x00 } ,  // LIGHTCYAN
            { 0x00 , 0x00 , 0xff , 0x00 } ,  // LIGHTRED
            { 0xff , 0x00 , 0xff , 0x00 } ,  // LIGHTMAGENTA
            { 0x00 , 0xff , 0xff , 0x00 } ,  // LIGHTBROWN
            { 0xff , 0xff , 0xff , 0x00 }  // WHITE
    };
    //Xprtcl_con_ctrl->SetMode (Xprtcl_con_ctrl, EfiConsoleControlScreenGraphics);
    //ST->ConOut->ClearScreen(ST->ConOut);
    if ( Xprtcl_gop )
    {
        //Xprtcl_gop->SetMode(Xprtcl_gop, 12);

        Xprtcl_gop->Blt ( Xprtcl_gop ,
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) &EfiColors[15] ,
                EfiBltVideoFill , 0 , // src x
                0 , // src y
                0 , // dst x
                0 , // dst y
                10 , 10 , 0 * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) );
    }
#endif /* 0 */

    shell_input.length = 4096;
    shell_input.wpos = 0;
    shell_input.contents = AllocateZeroPool ( shell_input.length );

    do
    {
        if ( (efirc = BS->WaitForEvent ( /*EVTS_COUNT*/2 , evts , &evt_idx )) )
        {
            //break;
        }

        switch ( evt_idx )
        {
        case EVTIDX_KEYPRESS :
            _do_input ( );
            break;
        case EVTIDX_PTRDVC_INPUT :
            _do_pointer_poll ( );
            break;
        case EVTIDX_PERIODIC :
            if ( !(efirc = RT->GetTime ( &tm , NULL )) )
            {
                if ( curr_day != tm.Day )
                {
                    curr_day = tm.Day;
                    wday_idx = _dmy_to_day_of_week ( tm.Day , tm.Month ,
                            tm.Year );
                }
                EB_CON_OUT_GET_XY( curx , cury );
                PrintAt ( 61 , 0 , L"%s %s %02d %02d:%02d" , wdays[wday_idx] ,
                        mnths[tm.Month - 1] , tm.Day , tm.Hour , tm.Minute );
                EB_CON_OUT_SET_XY( curx , cury );
            }
        }
    } while ( Xub_shell_running );

    ub_env_exit ( 1 );

    EB_FREE( shell_input.contents )
    ;

    if ( evts[EVTIDX_PERIODIC] )
    {
        efirc = BS->CloseEvent ( evts[EVTIDX_PERIODIC] );
    }

    return (efirc);
}

/*

 */
INTN
_do_input(VOID)
{
    CHAR16 *p;
    EFI_KEY_DATA ekd;

    p = (CHAR16 *) shell_input.contents;

    ub_console.rc(&ub_console , &ekd);

    if ( ekd.Key.UnicodeChar == 0 )
    {
        return ( 0 );
    }

    switch ( ekd.Key.UnicodeChar )
    {
    case L'\r' :
        /* FALLTHROUGH */
    case L'\n' :
        p[shell_input.wpos] = L'\0';
        _parse_input( );
        ZeroMem(shell_input.contents , shell_input.length);
        shell_input.wpos = 0;
        Print(prompt);
        break;
    case L'\b' :
        if ( shell_input.wpos > 0 )
        {
            p[shell_input.wpos] = L'\0';
            shell_input.wpos--;
            Print(L"\b");
        }
        break;
    default :
        if ( shell_input.wpos < (shell_input.length - 2) )
        {
            p[shell_input.wpos++] = ekd.Key.UnicodeChar;
            Print(L"%c" , ekd.Key.UnicodeChar);
        }
    }

    return ( 0 );
}

/*

 */
INTN
_do_pointer_poll(VOID)
{
    EFI_STATUS efirc;
    EFI_SIMPLE_POINTER_STATE spp_state;

    if ( Xprtcl_sptr != NULL )
    {
        efirc = Xprtcl_sptr->GetState ( Xprtcl_sptr , &spp_state );

        if ( EFI_ERROR( efirc ) )
        {
            Print ( L"\r\nError: simple_pointer.current->GetState\r\n" );
        }
        else
        {
            PrintAt ( 0 , 1 , L"X:%u Y:%u Z:%u L:%u R:%u" ,
                    spp_state.RelativeMovementX , spp_state.RelativeMovementY ,
                    spp_state.RelativeMovementZ , spp_state.LeftButton ,
                    spp_state.RightButton );
        }
    }

    return (0);
}

/*

 */
INTN _parse_input(
VOID
)
{

    INTN i, argc = 0;

    CHAR16 **argv = NULL;

    BOOT_CMD_FCN *cmdfcn = NULL;

    Print ( L"\r\n" );

    if ( ub_parse ( &argc , &argv , shell_input.contents ) )
    {
        Print ( L"efi-boot: error parsing %s\r\n" , shell_input.contents );
        return (1);
    }

    if ( argc < 1 )
    {
        return (0);
    }

    for (i = 0; i < Xshell_cmd_count ; i++)
    {
        if ( (Xshell_cmds[i].name != NULL)
                && (!StrCmp ( argv[0] , Xshell_cmds[i].name )) )
        {
            if ( Xshell_cmds[i].fcn != NULL )
            {
                cmdfcn = Xshell_cmds[i].fcn;
            }
        }
    }
    if ( cmdfcn != NULL )
    {
        Xprogname = argv[0];
        (cmdfcn) ( argc , argv );
        Xprogname = NULL;
        EB_FREE( argv )
        ;
    }
    else
    {
        Print ( L"efi-boot: %s: command not found\r\n" , argv[0] );
    }

    return (0);
}

/* *//*
 INTN
 pointer_event(
 __in VOID
 ) {
 EFI_STATUS efirc = 0;
 //UINTN                 i = 0;
 EFI_SIMPLE_POINTER_PROTOCOL *tmp_spp;
 EFI_SIMPLE_POINTER_STATE    spp_state;

 if (!ub_console.Xprtcl_sptr) { /// TODO: APP
 return(0);
 }

 //    for (i = 0; i < ub_console.protocols.simple_pointer.handles.count; i++)
 {
 tmp_spp = (ub_console.Xprtcl_sptr);
 efirc = (tmp_spp->GetState)(tmp_spp, &spp_state);
 if (!EFI_ERROR(efirc))
 {
 if (!ub_console.pointer.state.LeftButton && spp_state.LeftButton)
 {
 ub_console.pointer.event = EFI_CONSOLE_POINTER_EVENT_BUTTON_PRESSED_LEFT;
 }
 else if (!ub_console.pointer.state.RightButton && spp_state.RightButton)
 {
 ub_console.pointer.event = EFI_CONSOLE_POINTER_EVENT_BUTTON_PRESSED_RIGHT;
 }
 else if (ub_console.pointer.state.LeftButton && !spp_state.LeftButton)
 {
 ub_console.pointer.event = EFI_CONSOLE_POINTER_EVENT_BUTTON_RELEASED_LEFT;
 }
 else if (ub_console.pointer.state.RightButton && !spp_state.RightButton)
 {
 ub_console.pointer.event = EFI_CONSOLE_POINTER_EVENT_BUTTON_RELEASED_RIGHT;
 }
 else if (ub_console.pointer.state.RelativeMovementZ > 0)
 {
 ub_console.pointer.event = EFI_CONSOLE_POINTER_EVENT_SCROLL_DOWN;
 }
 else if (ub_console.pointer.state.RelativeMovementZ < 0)
 {
 ub_console.pointer.event = EFI_CONSOLE_POINTER_EVENT_SCROLL_UP;
 }
 else if (ub_console.pointer.state.RelativeMovementX || ub_console.pointer.state.RelativeMovementY)
 {
 ub_console.pointer.event = EFI_CONSOLE_POINTER_EVENT_REPOSITION;
 }
 else
 {
 ub_console.pointer.event = EFI_CONSOLE_POINTER_EVENT_NONE;
 }
 ub_console.pointer.state = spp_state;
 }

 }

 return ( 0 );

 }
 */
/*
 https://en.wikipedia.org/wiki/
 Determination_of_the_day_of_the_week#Implementation-dependent_methods

 In the C language expressions below, y, m and d are, respectively,
 integer variables representing
 the year (e.g., 1988), month (1-12) and day of the month (1-31).

 Returns 0 = Sunday, 1 = Monday, etc.
 */
INTN _dmy_to_day_of_week( INTN d, INTN m, INTN y )
{
    return ((d += m < 3 ? y-- : y - 2 , 23 * m / 9 + d + 4 + y / 4 - y / 100
            + y / 400) % 7);
}
#endif
