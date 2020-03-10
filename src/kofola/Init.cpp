//------------------------------------------------------------------------------------------------
// version 0.0.2
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>
#include "3d_all.h"

#include "Berusky3d_kofola_interface.h"
#include "init.h"


//------------------------------------------------------------------------------------------------
// Load items into memory
//------------------------------------------------------------------------------------------------
int lsi_Load_Items(LEVELINFO * p_Level)
{
  char dir[MAX_FILENAME], odir[MAX_FILENAME];
  struct dirent **files;
  int Count, m;
  SECONDDATADESC sec;

  if (getcwd(odir, MAX_FILENAME) == NULL)
    return 0;
  strcpy(dir, PRVKY_DIR);
  if (chdir(dir))
    return 0;

  file_filter_mask("*.itm");
  Count = scandir(dir, &files, &file_filter, NULL);
  if (Count < 0) {
    /* GCC warns when we don't check the return value of chdir(). For
       some reason, casting to (void) doesn't work. */
    if (chdir(odir))
      return 0;
    return 0;
  }

  p_Level->Count_Of_Objects = Count;
  p_Level->Object = (OBJECTDESC *) mmalloc(Count * sizeof(OBJECTDESC));

  for (m = 0; m < Count; m++) {
    char filename[MAX_FILENAME];
    FILE *file;

    strncpy(filename, files[m]->d_name, MAX_FILENAME - 1);
    free(files[m]);

    file = fopen(filename, "rb");
    if (file) {
      if (fread(p_Level->Object + m, sizeof(OBJECTDESC), 1, file) != 1) {
        kprintf(1, "Unable to read file %s: %s",
                filename, strerror(errno));
        continue;
      }
      fseek(file, 1000, SEEK_SET);
      if (fread(&sec, sizeof(SECONDDATADESC), 1, file) != 1) {
        kprintf(1, "Unable to read file %s: %s",
                filename, strerror(errno));
        continue;
      }
      p_Level->Object[m].Out_File = sec;
      fclose(file);
    }
    else {
      //MessageBox(p_Level->hWnd,text,"Warning",MB_OK);
      kprintf(1, "Unable to open file %s: %s",
              filename, strerror(errno));
    }
  }

  free(files);

  if (chdir(odir))
    return 0;

  return 1;
}

//------------------------------------------------------------------------------------------------
// Releas items from memory
//------------------------------------------------------------------------------------------------
void lsi_Release_Items(LEVELINFO * p_Level)
{
  free((void *) p_Level->Object);
}
