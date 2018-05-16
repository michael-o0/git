#include "../globals.h"
#include "../syscnsl.h"
#include "conout_wrapper.h"

#if 0
#include "external/freetype-2.8.1/include/ft2build.h"
#include FT_FREETYPE_H
#include "freetype/ftmodapi.h"

int strcmp( const char * s1, const char * s2 )
{
        while ( ( *s1 ) && ( *s1 == *s2 ) )
        {
                ++s1;
                ++s2;
        }
        return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
}
#endif // 0

EFI_STATUS screen_init(EFI_HANDLE ubhand, EFI_SYSTEM_TABLE *systab)
{

        ConOutWrapperInit(ubhand, systab);
        //ConOutWrapperExit(ubhand, systab);

#if 0
        void* _Alloc_Func(FT_Memory memory, long size)
        {
                return AllocatePool(size);
        }

        void _Free_Func(FT_Memory memory, void *block)
        {
                FreePool(block);
        }

        void* _Realloc_Func(FT_Memory memory, long cur_size, long new_size, void *block)
        {
                return ReallocatePool(block, cur_size, new_size);
        }

        FT_Memory
        ftmem;

        FT_Library
        *library = NULL;

        FT_Face
        face = NULL;

        ftmem->alloc = _Alloc_Func;
        ftmem->free = _Free_Func;
        ftmem->realloc = _Realloc_Func;

        FT_New_Library(ftmem, library);

#endif
        /*
         UINTN i, max_dim, best_mode, cols, rows;
         EFI_STATUS efirc;

         DPRINTF(L"screen_init\r\n","");

         max_dim = best_mode = 0;
         for (i = 0; ; i++)
         {
         efirc = ST->ConOut->QueryMode(ST->ConOut, i, &cols, &rows);
         if (EFI_ERROR(efirc))
         {
         DPRINTF(L"screen_init ERR\r\n","");
         break;
         }
         if (cols * rows > max_dim)
         {
         DPRINTF(L"screen_init New MaxMode %d, %dx%d\r\n", i, cols, rows);

         max_dim = cols * rows;
         best_mode = i;
         }
         }
         cpu_dead_loop();
         if (max_dim > 0)
         ST->ConOut->SetMode(ST->ConOut, best_mode);
         ST->ConOut->EnableCursor(ST->ConOut, TRUE);
         ST->ConOut->ClearScreen(ST->ConOut);
         */
        return (0);
}
