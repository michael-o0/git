#ifndef _EBSS_SCREEN_
#define _EBSS_SCREEN_

//#include <efi/efi.h>
//#include <efi/efilib.h>

//#include "ub_wides.h"
//#include "ConIO/ub_console.h"

struct ScreenElement {
  INTN x;
  INTN y;
  INTN z;
  INTN w;
  INTN h;
  VOID * p;
};

struct ClickArea {
  INTN x;
  INTN y;
  INTN z;
  INTN w;
  INTN h;
  // Action
  // DAction
  // RAction
};

struct VisualPointer {
  INTN x;
  INTN y;
  INTN w;
  INTN h;
  // Image
  // Prev_Image //underneath
};

struct Screen {
  struct {
    struct ScreenElement * next;
    struct ScreenElement this;
  } Elements;
  struct {
    struct ClickArea * next;
    struct ClickArea this;
  } ClickAreas;
  struct VisualPointer VisualPointer;
  INTN
    (* AddElement)
  (
    struct ScreenElement *
  );
  INTN
    (* AddCreateElement)
  (
    INTN x,
    INTN y,
    INTN z,
    INTN w,
    INTN h,
    VOID * p
  );
  INTN
    (* AddClickArea)
  (
    struct ClickArea *
  );
  // Draw();
};
extern struct Screen MScreen;

EFI_STATUS screen_init(EFI_HANDLE ubhand, EFI_SYSTEM_TABLE *systab);

#endif /* not _EBSS_SCREEN_ */

// InitializeListHead
// InsertTailList
// RemoveEntryList
// GetNextNode

// Overlap fnh
