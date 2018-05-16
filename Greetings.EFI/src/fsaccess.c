#include "fsaccess.h"

EFI_STATUS fsaccess_lsdir(EFI_FILE_PROTOCOL *dir);

/*
 *
 */
INTN fsaccess_init(EFI_HANDLE loadedimg, EFI_SYSTEM_TABLE *efi_systbl)
{
        //DiskIoProtocolGUID = EFI_DISK_IO_PROTOCOL_GUID,
        //BlockIoProtocolGUID = EFI_BLOCK_IO_PROTOCOL_GUID,
        //FileInfoGUID = EFI_FILE_INFO_ID,

        EFI_GUID guid_prtcl_dvcpath = EFI_DEVICE_PATH_PROTOCOL_GUID;
        EFI_GUID guid_prtcl_sfs = SIMPLE_FILE_SYSTEM_PROTOCOL;
        EFI_GUID guid_prtcl_fsinfo = EFI_FILE_SYSTEM_INFO_ID;
        UINTN it;
        UINTN handcount;
        EFI_HANDLE *phandles;
        UINTN infolength;
        EFI_STATUS efirc;

        Print(L"fsaccess_init:\r\n");
        /*
        EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
        EFI_DEVICE_PATH *BootPartitionPath = NULL;
        CHAR16 *DevicePathString;

        efirc = systab->BootServices->OpenProtocol(imghandle,
                &gEfiLoadedImageProtocolGuid, (VOID**) &LoadedImage, imghandle,
                NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL);

        BootPartitionPath = DevicePathFromHandle(LoadedImage->DeviceHandle);
        DevicePathString = DevicePathToStr(BootPartitionPath);
        Print(L"Boot disk: %s", DevicePathString);
        systab->BootServices->FreePool(DevicePathString);
        return (0);
         */
        /* Locate all SIMPLE_FILE_SYSTEM_PROTOCOL handles */
        efirc = efi_systbl->BootServices->LocateHandleBuffer(ByProtocol,
                &guid_prtcl_sfs, NULL, &handcount, &phandles);

        if (EFI_ERROR(efirc)) {
                FreePool(phandles);
                return (1);
        }

        Print(L"Handles: %u\r\n", handcount);

        for (it = 0; it < handcount; it++) {
                EFI_DEVICE_PATH_PROTOCOL *dvcpath = NULL;
                EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp = NULL;
                EFI_FILE_SYSTEM_INFO *fsinfo = NULL;
                EFI_FILE_PROTOCOL *root = NULL;
                EFI_FILE_INFO *fileinfo = NULL;
                //EFI_BLOCK_IO_PROTOCOL *blkio = NULL;

                efirc = efi_systbl->BootServices->HandleProtocol(phandles[it],
                        &guid_prtcl_dvcpath, (VOID **) &dvcpath);

                if (EFI_ERROR(efirc)) {
                        continue;
                }

                Print(L"dvcpath: %s\r\n", DevicePathToStr(dvcpath));

                efirc = efi_systbl->BootServices->OpenProtocol(phandles[it],
                        &guid_prtcl_sfs, (VOID **) &sfsp, loadedimg,
                        NULL,
                        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

                if (EFI_ERROR(efirc) || (sfsp == NULL)) {
                        continue;
                }

                efirc = sfsp->OpenVolume(sfsp, &root);

                if (EFI_ERROR(efirc) || (root == NULL)) {
                        continue;
                }

                infolength = 0;

                efirc = root->GetInfo(root, &guid_prtcl_fsinfo, &infolength,
                NULL);

                if (EFI_ERROR(efirc) && (efirc != EFI_BUFFER_TOO_SMALL)) {
                        efirc = root->Close(root);
                        continue;
                }

                fsinfo = AllocatePool(infolength);

                efirc = root->GetInfo(root, &guid_prtcl_fsinfo, &infolength,
                        fsinfo);

                if (!EFI_ERROR(efirc)) {
                        Print(L"%u / %u\r\n", fsinfo->FreeSpace,
                                fsinfo->VolumeSize);
                }

                fsaccess_lsdir(root);

                //      infolength = 2048; /* should fit most FAT32 names ...ahem */
                /*        fileinfo = AllocatePool(infolength);

                 do
                 {
                 ZeroMem(fileinfo , 2048);
                 efirc = root->Read(root , &infolength , fileinfo);
                 if ( !EFI_ERROR(efirc) && *(fileinfo->FileName) != L'\0' )
                 {
                 Print(L"%s %u %u %u %x\r\n" ,
                 fileinfo->FileName ,
                 fileinfo->FileSize,
                 fileinfo->PhysicalSize,
                 fileinfo->Size,
                 fileinfo->Attribute);
                 if ( (fileinfo->Attribute) && EFI_FILE_DIRECTORY )
                 {

                 }
                 }
                 } while ( infolength != 0 );
                 */
                FreePool(fileinfo);
                FreePool(fsinfo);
                root->Close(root);
        }

        FreePool(phandles);

        return (0);
}

/*
 *
 */
EFI_STATUS fsaccess_lsdir(EFI_FILE_PROTOCOL *dir)
{

#define INFO_LEN 4096 /* expected length of a returned fileinfo struct in bytes
                       * XXX: redo/amend w/ AllocatePool ??*/

        EFI_STATUS efirc;
        UINTN infolength; /* stack variable */
        static INTN buf[INFO_LEN], runs = 1; /* XXX: runs ends up being val-1 ??? */
        static EFI_FILE_INFO *fileinfo = (EFI_FILE_INFO *) &buf;
        EFI_FILE_PROTOCOL *subdir = NULL; /* yay for stack variables :) */

        infolength = INFO_LEN;

        do {
                ZeroMem(fileinfo, INFO_LEN);
                efirc = dir->Read(dir, &infolength, fileinfo);
                if (!EFI_ERROR(efirc) && *(fileinfo->FileName) != L'\0') {
                        Print(L"[%X] %s %u %u\r\n", runs, fileinfo->FileName,
                                fileinfo->FileSize, fileinfo->PhysicalSize);

                        if (((fileinfo->Attribute & EFI_FILE_DIRECTORY)
                                == EFI_FILE_DIRECTORY)
                                && StrCmp(fileinfo->FileName, L".")
                                && StrCmp(fileinfo->FileName, L"..")) {
                                efirc = dir->Open(dir, &subdir,
                                        fileinfo->FileName,
                                        EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
                                if (efirc == EFI_SUCCESS) {
                                        runs++;
                                        fsaccess_lsdir(subdir);
                                        subdir->Close(subdir);
                                } else {
                                        Print(
                                                L"[%u] Could not open %s for reading.",
                                                runs, fileinfo->FileName);
                                }
                        }
                }
        } while (infolength != 0);
        runs--;
        return (EFI_SUCCESS);
}

/*
 Print(  L"Block size:    %u      IOAlign:       %u\r\n"
 L"LastBlock      %u      LBPerPhysBlk   %u\r\n"
 L"LogicalPrtn    %u      LwstAlgndLba   %u\r\n"
 L"MediaId        %u      Present        %u\r\n"
 L"OptTxGran      %u      ReadOnly       %u\r\n"
 L"Removable      %u      WrtCache       %u\r\n",
 blkio->Media->BlockSize,
 blkio->Media->IoAlign,
 blkio->Media->LastBlock,
 blkio->Media->LogicalBlocksPerPhysicalBlock,
 blkio->Media->LogicalPartition,
 blkio->Media->LowestAlignedLba,
 blkio->Media->MediaId,
 blkio->Media->MediaPresent,
 blkio->Media->OptimalTransferLengthGranularity,
 blkio->Media->ReadOnly,
 blkio->Media->RemovableMedia,
 blkio->Media->WriteCaching);
 */

#if 0
efirc = systab->BootServices->LocateHandle(ByProtocol, &BlockIoProtocolGUID, NULL, &nparts, handbuf);

nparts /= sizeof(handbuf[0]);
for (i=0; i<nparts; i++)
{
        efirc = systab->BootServices->HandleProtocol(handbuf[i], &guid_prtcl_dvcpath, (void**)&devpath);
        if (EFI_ERROR(efirc))
        continue;

        //DPRINTF(L"devpath: %s\n", efi_devpath_text(devpath));

        //while (!IsDevicePathEnd(NextDevicePathNode(devpath)))
        //  devpath = NextDevicePathNode(devpath);

        efirc = systab->BootServices->HandleProtocol(handbuf[i], &BlockIoProtocolGUID, (void**)&blkio);

        if (EFI_ERROR(efirc))
        continue;

        if (!blkio->Media->LogicalPartition)
        continue;

        efirc = systab->BootServices->HandleProtocol(handbuf[i], &SimpleFileSystemProtocolGUID, (void**)&sfs);

        if (EFI_ERROR(efirc))
        continue;

        EFI_HANDLE loaderhandle;
        EFI_LOADED_IMAGE *loaded_image;

//      void *buffer;
//      size_t bufsize;

        efirc = sfs->OpenVolume(sfs, &fs);

        if (EFI_ERROR(efirc))
        continue;

        DPRINTF(L"we opened volume\r\n","");

        EFI_FILE_PROTOCOL *tmp;
//      efirc = fs->Open(fs, &tmp, L"\\EFI\\BOOT\\BOOTX64.EFI\0", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
        efirc = fs->Open(fs, &tmp, L"\\EFI\\centos\\grubx64.efi\0", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

        if (EFI_ERROR(efirc))
        {
                DPRINTF(L" error %i\r\n", (int)efirc);
                continue;
        }

        DPRINTF(L"we have file\r\n","");

        UINTN bufsize = 0;
        EFI_FILE_INFO *infobuffer;

        efirc = tmp->GetInfo(tmp, &FileInfoGUID, &bufsize, NULL);
        if (efirc != EFI_BUFFER_TOO_SMALL)
        {
                DPRINTF(L"file get info died with %i\r\n", (int)efirc);
                continue;
        }

        DPRINTF(L"size of structure %i\n", (int)bufsize);

        efirc = systab->BootServices->AllocatePool(EfiLoaderData, bufsize, (void**)(&infobuffer));
        if (EFI_ERROR(efirc))
        continue;

        efirc = tmp->GetInfo(tmp, &FileInfoGUID, &bufsize, (void*)infobuffer);
        if (EFI_ERROR(efirc))
        continue;

        bufsize = infobuffer->FileSize;

        efirc = systab->BootServices->FreePool((void*)infobuffer);
        if (EFI_ERROR(efirc))
        continue;

        void *buffer;

        DPRINTF(L" The file size is %i\r\n",(int)bufsize);

        efirc = systab->BootServices->AllocatePool(EfiLoaderData, bufsize, &buffer);
        if (EFI_ERROR(efirc))
        continue;

        efirc = tmp->Read(tmp, &bufsize, buffer);
        if (EFI_ERROR(efirc))
        continue;

        DPRINTF(L" size %i \r\n", (int)bufsize);

        EFI_HANDLE efi_image_handle;
        efirc = systab->BootServices->LoadImage(FALSE, efi_image_handle, devpath, buffer, bufsize, &loaderhandle);

        if (EFI_ERROR(efirc))
        {
                DPRINTF(L"efirc %i \r\n", (int)efirc);
                continue;
        }

        DPRINTF(L" passed 1 ","");

        EFI_GUID LoadedImageGUID = EFI_LOADED_IMAGE_PROTOCOL_GUID;
        efirc = systab->BootServices->HandleProtocol(loaderhandle, &LoadedImageGUID, (void**)&loaded_image);

        if (EFI_ERROR(efirc))
        continue;

        DPRINTF(L" passed 2 ","");

        loaded_image->DeviceHandle = handbuf[i];

        efirc = systab->BootServices->StartImage(loaderhandle, NULL, NULL);
        if (EFI_ERROR(efirc))
        continue;

        DPRINTF(L"doing something here\r\n","");

}

EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
EFI_DEVICE_PATH *BootPartitionPath = NULL;
CHAR16 *DevicePathString;
EFI_STATUS efirc;

efirc = systab->BootServices->OpenProtocol(gLoadedImage,
        &gEfiLoadedImageProtocolGuid,
        (VOID**)&LoadedImage,
        gLoadedImage,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL);

BootPartitionPath = DevicePathFromHandle(LoadedImage->DeviceHandle);
DevicePathString = DevicePathToStr(BootPartitionPath);
Print(L"Boot disk: %s", DevicePathString);
systab->BootServices->FreePool(DevicePathString);
return(0);
}
#endif
