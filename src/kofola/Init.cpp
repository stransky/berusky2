//------------------------------------------------------------------------------------------------
// version 0.0.2
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "3d_all.h"

#include "Berusky3d_kofola_interface.h"
#include "init.h"


//------------------------------------------------------------------------------------------------
// Load items into memory
//------------------------------------------------------------------------------------------------
int lsi_Load_Items(LEVELINFO * p_Level)
{
  char text[MAX_FILENAME], dir[MAX_FILENAME], odir[MAX_FILENAME];
  int Count, m, error_open;
  struct _finddata_t Data;
  size_ptr Done;
  int error;
  FILE *file;
  SECONDDATADESC sec;
  APAK_HANDLE *aHandle;
  
  if (getcwd(odir, MAX_FILENAME) == NULL)
    return 0;
  strcpy(dir, PRVKY_DIR);
  if (chdir(dir))
    return 0;

  strcpy(text, "*.itm");
  Count = 0;
  
  aHandle = apakopen("items.pak", dir, &error_open);

  if (!aHandle) {
    kerror(1, "nepodarilo se otevrit archiv items.pak!!!");    
  }
  aHandle->pActualNode = aHandle->pRootNode->pNextNode;


  Done = afindfirst(aHandle, text, &Data);
  error = Done;

  while (error != -1) {
    Count++;
    error = afindnext(Done, &Data);
  }

  afindclose(Done);

  p_Level->Count_Of_Objects = Count;
  p_Level->Object = (OBJECTDESC *) mmalloc(Count * sizeof(OBJECTDESC));

  Done = afindfirst(aHandle, text, &Data);
  error = Done;

  for (m = 0; m < Count; m++) {
    if (error == -1) {
      //MessageBox(p_Level->hWnd,"Count Of Models is not matching","Warning",MB_OK);
      kprintf(1, "Count Of Models is not matching");
    }
    else {
      file = 0;
      achdir(aHandle, dir);
      file = aopen(aHandle, Data.name, "rb");
      if (file) {
        aread(p_Level->Object + m, sizeof(OBJECTDESC), 1, file);
        aseek(file, 1000, SEEK_SET);
        aread(&sec, sizeof(SECONDDATADESC), 1, file);
        p_Level->Object[m].Out_File = sec;
        aclose(file);
      }
      else {
        sprintf(text, "Unable to find file %s", Data.name);
        //MessageBox(p_Level->hWnd,text,"Warning",MB_OK);
        kprintf(1, text);
      }
    }
    error = afindnext(Done, &Data);
  }

  afindclose(Done);

  apakclose(&aHandle);

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
