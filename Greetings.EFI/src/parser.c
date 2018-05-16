#include "parser.h"

#include "globals.h"

#define    ARG_TYPE CHAR16
#define    ARG_SIZE (sizeof(ARG_TYPE))
#define  ARG_LENGTH ((ARG_SIZE) * 128)
#define  ARGS_COUNT (16)
#define ARGS_LENGTH (sizeof(ARG_TYPE *) * (ARGS_COUNT))

struct
{
        ARG_TYPE *ptr;
        UINTN len;
        UINTN pos;

} buf = {NULL, 0, 0};

struct
{
        ARG_TYPE **buf;
        UINTN len;
        UINTN count;

} aar = {NULL, 0, 0};

INTN parser_init(VOID)
{

        parser_exit(0);

        if (buf.ptr == NULL) {
                buf.len = ARG_LENGTH;
                buf.ptr = AllocateZeroPool(buf.len);
        }

        if (aar.buf == NULL) {
                aar.len = ARGS_LENGTH;
                aar.count = ARGS_COUNT;
                aar.buf = AllocateZeroPool(aar.len);
        }

        return (0);
}

INTN parser_exit(INTN dealloc)
{

        UINTN it;

        if ((aar.buf) != NULL) {
                for (it = 0; it < aar.count; it++) {
                        FREE_AND_NULL( aar.buf[it] )
                        ;
                }
        }

        if (dealloc) {
                FREE_AND_NULL( buf.ptr )
                ;
                FREE_AND_NULL( aar.buf )
                ;
        }

        return (0);
}

INTN _grow_buf(VOID)
{

        UINTN len;

        ARG_TYPE *p;

        len = buf.len + ARG_LENGTH;
        p = AllocateZeroPool(len);

        if (p == NULL) {
                return (1);
        }

        BS->CopyMem(p, buf.ptr, buf.len);
        BS->FreePool(buf.ptr);
        buf.ptr = p;

        buf.len = len;

        return (0);
}

INTN _grow_args(VOID)
{

        UINTN len;

        ARG_TYPE **p;

        len = aar.len + ARGS_LENGTH;
        p = AllocateZeroPool(len);

        if (p == NULL) {
                return (1);
        }

        BS->CopyMem(p, aar.buf, aar.len);
        BS->FreePool(aar.buf);
        aar.buf = p;

        aar.len = len;
        aar.count += ARGS_COUNT;
        return (0);
}

/*
 * parse
 * -> str
 * <- argc
 * <- argv
 */
INTN parse(CHAR16 *p, INTN *argc, CHAR16 ***argv)
{
        UINTN count = 0;

        if (p == NULL) {
                return (1);
        }

        parser_init();

        do {
                // TODO char processing \' \" ..

                if ((L' ' == *p) || (L'\0' == *p)) {
                        if (buf.pos) {
                                if (count >= (aar.count - 1)) {
                                        _grow_args(); // TODO check == 1 needed
                                }
                                buf.ptr[buf.pos] = L'\0';
                                aar.buf[count] = AllocatePool(
                                        buf.pos * ARG_SIZE + ARG_SIZE);
                                BS->CopyMem(aar.buf[count], buf.ptr,
                                        (buf.pos * ARG_SIZE + ARG_SIZE));
                                ZeroMem(buf.ptr, buf.len);
                                count++;
                                buf.pos = 0;
                        }

                        if (L'\0' == *p) {
                                break;
                        } else {
                                p++;
                        }
                } else {
                        if (buf.pos >= (buf.len - ARG_SIZE)) {
                                _grow_buf(); // TODO check == 1 needed
                        }

                        buf.ptr[buf.pos++] = *p++;
                }
        } while (1);

        buf.pos = 0;

        /* NULL terminate */
        aar.buf[count] = NULL;
        *argc = (INTN) count; /* NOTE: not expecting to exceed an int 64bit */
        *argv = AllocatePool((sizeof(ARG_TYPE *) * (count + 1)));
        BS->CopyMem(*argv, aar.buf, (sizeof(ARG_TYPE *) * (count + 1)));

        return (0);
}
