#include "commands.h"
#include "syscnsl.h"
#include "globals.h"

#define INPCHAR_TYPE CHAR16
#define INPCHAR_LEN (sizeof(INPCHAR_TYPE))
#define INPBUF_LEN ((INPCHAR_LEN) * 2048)

static INPCHAR_TYPE *inpbuffer;
static INPCHAR_TYPE *iboffset;

enum events_types
{
        EVNTTYPE_KEYPRESS = 1, EVNTTYPE_SPTR_PRTCL, EVNTTYPE_PERIODIC
};

struct evnt_ctx
{
        enum events_types event_type;
        VOID *evntfunc; /* i.e. events_input_? */
        VOID *evntfunc_ctx; /* i.e. *prtcl_stxt_in */
};

/*
 *
 */
static INTN events_input_getk(VOID *prtcl, VOID *trgt)
{
        UINTN eeid;
        EFI_STATUS efirc;
        EFI_INPUT_KEY ikey;
        EFI_SIMPLE_TEXT_INPUT_PROTOCOL *prtcl_stxt_in;
        EFI_KEY_DATA *ekd;

        if ((prtcl == NULL) || (trgt == NULL))
                return (-1);

        prtcl_stxt_in = (EFI_SIMPLE_TEXT_INPUT_PROTOCOL *) prtcl;
        ekd = (EFI_KEY_DATA *) trgt;

        efirc = prtcl_stxt_in->ReadKeyStroke(prtcl_stxt_in, &ikey);

        while (efirc == EFI_NOT_READY) {
                if ((prtcl_stxt_in->WaitForKey) != NULL) {
                        Xefi_systbl->BootServices->WaitForEvent(1,
                                &prtcl_stxt_in->WaitForKey, &eeid);
                }
                efirc = prtcl_stxt_in->ReadKeyStroke(prtcl_stxt_in, &ikey);
        }

        ekd->KeyState.KeyShiftState = 0;
        ekd->KeyState.KeyToggleState = 0;
        ekd->Key = ikey;

        return (0);
}

static INTN events_input_getk_ex(VOID *prtcl, VOID *trgt)
{
        UINTN eeid;
        EFI_STATUS efirc;
        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *prtcl_stxt_in_ex;
        EFI_KEY_DATA *ekd;

        if ((prtcl == NULL) || (trgt == NULL))
                return (-1);

        prtcl_stxt_in_ex = (EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *) prtcl;
        ekd = (EFI_KEY_DATA *) trgt;

        efirc = prtcl_stxt_in_ex->ReadKeyStrokeEx(prtcl_stxt_in_ex, ekd);

        while (efirc == EFI_NOT_READY) {
                if ((prtcl_stxt_in_ex->WaitForKeyEx) != NULL) {
                        Xefi_systbl->BootServices->WaitForEvent(1,
                                &prtcl_stxt_in_ex->WaitForKeyEx, &eeid);
                }
                efirc = prtcl_stxt_in_ex->ReadKeyStrokeEx(prtcl_stxt_in_ex,
                        ekd);
        }

        return (0);
}

/*
 *
 */
INTN events_event_keypress(struct evnt_ctx *evnt_ctx)
{
        EFI_KEY_DATA ekd;
        INTN (*getk)(VOID *prtcl, VOID *trgt);

        if ((evnt_ctx->evntfunc == NULL) || (evnt_ctx->evntfunc_ctx == NULL))
                return (-1);

        getk = evnt_ctx->evntfunc;

        getk(evnt_ctx->evntfunc_ctx, &ekd);

        if (ekd.Key.UnicodeChar == 0)
                return (0);

        switch (ekd.Key.UnicodeChar)
        {
        case L'\r':
                /* FALLTHROUGH */
        case L'\n':
                *iboffset = L'\0';
                commands(inpbuffer);
                ZeroMem(inpbuffer, INPBUF_LEN);
                iboffset = inpbuffer;
                break;
        case L'\b':
                if (iboffset > inpbuffer) {
                        *iboffset = L'\0';
                        iboffset--;
                        Print(L"\b");
                }
                break;
        default:
                if (iboffset < (inpbuffer + INPBUF_LEN)) {
                        *iboffset = ekd.Key.UnicodeChar;
                        iboffset++;
                        Print(L"%c", ekd.Key.UnicodeChar);
                }
        }

        return (0);
}

/*
 *
 */
EFI_STATUS events(EFI_SYSTEM_TABLE *efi_systbl)
{
        EFI_GUID guid_prtcl_stxt_in_ex = EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;
        EFI_GUID guid_prtcl_stxt_in = EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID;
        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *prtcl_stxt_in_ex;
        EFI_SIMPLE_TEXT_INPUT_PROTOCOL *prtcl_stxt_in;

        struct evnt_ctx *events_ctx = NULL;
        EFI_EVENT *events = NULL;
        EFI_HANDLE *stxt_in_hndlbuf = NULL;
        EFI_HANDLE *stxt_in_ex_hndlbuf = NULL;
        UINTN stxt_in_hndlscount;
        UINTN stxt_in_ex_hndlscount;
        UINTN eventscount;
        UINTN eventslen;
        UINTN it;
        UINTN j;
        UINTN k;
        UINTN dups;
        UINTN evntidx;
        EFI_STATUS efirc;

        inpbuffer = AllocateZeroPool(INPBUF_LEN);

        if (inpbuffer == NULL)
                return (EFI_OUT_OF_RESOURCES);

        iboffset = inpbuffer;

        efirc = efi_systbl->BootServices->LocateHandleBuffer(ByProtocol,
                &guid_prtcl_stxt_in_ex, NULL, &stxt_in_ex_hndlscount,
                &stxt_in_ex_hndlbuf);

        if (EFI_ERROR(efirc))
                stxt_in_ex_hndlscount = 0;

        efirc = efi_systbl->BootServices->LocateHandleBuffer(ByProtocol,
                &guid_prtcl_stxt_in, NULL, &stxt_in_hndlscount,
                &stxt_in_hndlbuf);

        if (EFI_ERROR(efirc))
                stxt_in_hndlscount = 0;

        if (!stxt_in_ex_hndlscount && !stxt_in_hndlscount) {
                /* take into account prefs */
        }

        dups = 0;

        for (j = 0; j < stxt_in_ex_hndlscount; j++) {
                for (k = 0; k < stxt_in_hndlscount; k++) {
                        if (stxt_in_ex_hndlbuf[j] == stxt_in_hndlbuf[k]) {
                                dups++;
                                stxt_in_hndlbuf[k] = NULL;
                                continue;
                        }
                }
        }

        eventscount = stxt_in_ex_hndlscount + (stxt_in_hndlscount - dups);
        eventslen = eventscount + 2; /* timer, pointer */
        events = AllocateZeroPool(sizeof(EFI_EVENT) * eventslen);
        events_ctx = AllocateZeroPool(sizeof(struct evnt_ctx) * eventslen);

        for (j = 0; j < stxt_in_ex_hndlscount; j++) {
                efirc = efi_systbl->BootServices->HandleProtocol(
                        stxt_in_ex_hndlbuf[j], &guid_prtcl_stxt_in_ex,
                        (VOID **) &prtcl_stxt_in_ex);
                if (EFI_ERROR(efirc))
                        continue;
                events_ctx[j].event_type = EVNTTYPE_KEYPRESS;
                events_ctx[j].evntfunc = &events_input_getk_ex;
                events_ctx[j].evntfunc_ctx = prtcl_stxt_in_ex;
                events[j] = prtcl_stxt_in_ex->WaitForKeyEx;
        }

        if (eventscount > stxt_in_ex_hndlscount) {
                it = stxt_in_ex_hndlscount;
                for (k = 0; k < stxt_in_hndlscount; k++) {
                        if (stxt_in_hndlbuf[k] != NULL) {
                                efirc =
                                        efi_systbl->BootServices->HandleProtocol(
                                                stxt_in_hndlbuf[k],
                                                &guid_prtcl_stxt_in,
                                                (VOID **) &prtcl_stxt_in);
                                if (EFI_ERROR(efirc))
                                        continue;
                                events_ctx[it].event_type = EVNTTYPE_KEYPRESS;
                                events_ctx[it].evntfunc = &events_input_getk;
                                events_ctx[it].evntfunc_ctx = prtcl_stxt_in;
                                events[it] = prtcl_stxt_in->WaitForKey;
                                it++;
                        }
                }
        }



        Print(L"prtcl_stxt_in_ex handles: %u ; ", stxt_in_ex_hndlscount);

        for (it = 0; it < stxt_in_ex_hndlscount; it++) {
                Print(L"%X ", stxt_in_ex_hndlbuf[it]);
        }
        Print(L"\r\n");

        Print(L"unique prtcl_stxt_in %u\r\n", stxt_in_hndlscount - dups);
        Print(L"prtcl_stxt_in handles: %u ; ", stxt_in_hndlscount);

        for (it = 0; it < stxt_in_hndlscount; it++) {
                Print(L"%X ", stxt_in_hndlbuf[it]);
        }
        Print(L"\r\n");

        for (it = 0; it < eventscount; it++) {
        Print(L"%u %X %X %X\r\n",
                it,
                events_ctx[it].evntfunc_ctx,
                        events[it], events_ctx[it].evntfunc);
        }



        cmd_prompt(0, NULL);

        /* Main Loop / Event Dispatcher */
        do {
                if ((efirc = efi_systbl->BootServices->WaitForEvent(eventscount,
                        events, &evntidx))) {
                        //break;
                }

                switch (events_ctx[evntidx].event_type)
                {
                case EVNTTYPE_KEYPRESS:
                        events_event_keypress(&events_ctx[evntidx]);
                        break;
                case EVNTTYPE_SPTR_PRTCL:
                        break;
                case EVNTTYPE_PERIODIC:
                        break;
                }
        } while (Xkeeplooping);

#if 0
        /*
         * Events setup
         */
        if (!(efirc = efi_systbl->BootServices->CreateEvent(EVT_TIMER, 0, NULL,
        NULL, &evnts[EVNTS_IDX_PERIODIC]))) {
                efi_systbl->BootServices->SetTimer(evnts[EVNTS_IDX_PERIODIC],
                        TimerPeriodic, 10000000);
        } else {
                evnts[EVNTS_IDX_PERIODIC] = NULL;
        }

        if ((Xsyscnsl_ctx.prtcl_stxt_in_ex != NULL)
                && ((Xsyscnsl_ctx.prtcl_stxt_in_ex->WaitForKeyEx) != NULL)) {
                evnts[EVNTS_IDX_KEYPRESS] =
                        Xsyscnsl_ctx.prtcl_stxt_in_ex->WaitForKeyEx;
        } else if ((Xsyscnsl_ctx.prtcl_stxt_in->WaitForKey) != NULL) {
                evnts[EVNTS_IDX_KEYPRESS] =
                        Xsyscnsl_ctx.prtcl_stxt_in->WaitForKey;
        }

        if ((Xsyscnsl_ctx.prtcl_sptr != NULL)
                && ((Xsyscnsl_ctx.prtcl_sptr->WaitForInput) != NULL)) {
                evnts[EVNTS_IDX_PTRDVC_INPUT] =
                        Xsyscnsl_ctx.prtcl_sptr->WaitForInput;
        }

        /* Main Loop / Event Dispatcher */
        do {
                if ((efirc = efi_systbl->BootServices->WaitForEvent(
                        EVNTS_IDX_COUNT, evnts, &evntidx)))
                        break;

                switch (evntidx)
                {
                case EVNTS_IDX_KEYPRESS:
                        events_event_keypress(&Xsyscnsl);
                        break;
                case EVNTS_IDX_PTRDVC_INPUT:
                        break;
                case EVNTS_IDX_PERIODIC:
                        break;
                }
        } while (Xkeeplooping);
#endif

        /* CloseEvent(..)...*/
        FreePool(events_ctx);
        FreePool(events);
        FreePool(stxt_in_hndlbuf);
        FreePool(stxt_in_ex_hndlbuf);
        FreePool(inpbuffer);

        return (EFI_SUCCESS);
}
