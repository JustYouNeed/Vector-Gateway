/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

# include "diskio.h"		/* FatFs lower layer API */
# include "app_w25qxx.h"
# include "bsp.h"		
# include "ff.h"
# define SD_CARD		1  //SD卡,卷标为0
# define EX_FLASH 	0	//外部flash,卷标为1

# define FLASH_SECTOR_SIZE 	512			  

//对于W25Q128
//前12M字节给fatfs用,12M字节后,用于存放字库,	    
uint16_t	    FLASH_SECTOR_COUNT=2048*12;	//W25Q1218,前12M字节给FATFS占用
# define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区

FATFS * fat_FLASH;
FIL*  file;
UINT br, bw;
FILINFO * fileInfo;
DIR dir;

static uint8_t flag = 0;

Disk_drvTypeDef disk = {{0},{0},{0},0};

char Flash_Path[4];
const Diskio_drvTypeDef Flash_Disk = 
{
	Flash_initialize,
	Flash_status,
	Flash_read,
	Flash_write,
	Flash_ioctl,
};


extern USBH_HandleTypeDef  HOST_HANDLE;

#if _USE_BUFF_WO_ALIGNMENT == 0
/* Local buffer use to handle buffer not aligned 32bits*/
static DWORD scratch[_MAX_SS / 4];
#endif

char USB_Path[4];
const Diskio_drvTypeDef  USB_Disk =
{
  USBH_initialize,
  USBH_status,
  USBH_read, 
#if  _USE_WRITE == 1
  USBH_write,
#endif /* _USE_WRITE == 1 */  
#if  _USE_IOCTL == 1
  USBH_ioctl,
#endif /* _USE_IOCTL == 1 */
};


DSTATUS Flash_initialize(BYTE lun)
{
	app_flash_Config();
	return RES_OK;
}

DSTATUS Flash_status(BYTE lun)
{
	return RES_OK;
}

DRESULT Flash_ioctl(BYTE lun, BYTE cmd, void *buff)
{
	DRESULT res;						  			     
	switch(cmd)
	{
		case CTRL_SYNC:
		res = RES_OK; 
				break;	 
		case GET_SECTOR_SIZE:
				*(WORD*)buff = FLASH_SECTOR_SIZE;
				res = RES_OK;
				break;	 
		case GET_BLOCK_SIZE:
				*(WORD*)buff = FLASH_BLOCK_SIZE;
				res = RES_OK;
				break;	 
		case GET_SECTOR_COUNT:
				*(DWORD*)buff = FLASH_SECTOR_COUNT;
				res = RES_OK;
				break;
		default:
				res = RES_PARERR;
				break;
	}
	return res;
}

DRESULT Flash_read(BYTE lun, BYTE* buff, DWORD sector, UINT count)
{
	DRESULT res = RES_OK;
	for(;count>0;count--)
	{
		app_flash_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
		sector++;
		buff += FLASH_SECTOR_SIZE;
	}
	
	return res;
}


DRESULT Flash_write(BYTE lun, const BYTE* buff, DWORD sector, UINT count)
{
	DRESULT res = RES_OK;
	for(;count>0;count--)
	{										    
		app_flash_Write((uint8_t*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
		sector++;
		buff+=FLASH_SECTOR_SIZE;
	}
	
	return res;
}



DSTATUS USBH_initialize(BYTE lun)
{
  return RES_OK;
}



DSTATUS USBH_status(BYTE lun)
{
  DRESULT res = RES_ERROR;
  
  if(USBH_MSC_UnitIsReady(&HOST_HANDLE, lun))
  {
    res = RES_OK;
  }
  else
  {
    res = RES_ERROR;
  }
  
  return res;
}




DRESULT USBH_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;
  USBH_StatusTypeDef  status = USBH_OK;

  if ((DWORD)buff & 3) /* DMA Alignment issue, do single up to aligned buffer */
  {
#if _USE_BUFF_WO_ALIGNMENT == 0
    while ((count--)&&(status == USBH_OK))
    {
      status = USBH_MSC_Read(&HOST_HANDLE, lun, sector + count, (uint8_t *)scratch, 1);
      if(status == USBH_OK)
      {
        memcpy (&buff[count * _MAX_SS] ,scratch, _MAX_SS);
      }
      else
      {
        break;
      }
    }
#else
    return res;
#endif
  }
  else
  {
    status = USBH_MSC_Read(&HOST_HANDLE, lun, sector, buff, count);
  }
  
  if(status == USBH_OK)
  {
    res = RES_OK;
  }
  else
  {
    USBH_MSC_GetLUNInfo(&HOST_HANDLE, lun, &info); 
    
    switch (info.sense.asc)
    {
    case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
    case SCSI_ASC_MEDIUM_NOT_PRESENT:
    case SCSI_ASC_NOT_READY_TO_READY_CHANGE: 
      USBH_ErrLog ("USB Disk is not ready!");  
      res = RES_NOTRDY;
      break; 
      
    default:
      res = RES_ERROR;
      break;
    }
  }
  
  return res;
}


#if _USE_WRITE == 1
DRESULT USBH_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR; 
  MSC_LUNTypeDef info;
  USBH_StatusTypeDef  status = USBH_OK;  

  if ((DWORD)buff & 3) /* DMA Alignment issue, do single up to aligned buffer */
  {
#if _USE_BUFF_WO_ALIGNMENT == 0
    while (count--)
    {
      memcpy (scratch, &buff[count * _MAX_SS], _MAX_SS);
      
      status = USBH_MSC_Write(&HOST_HANDLE, lun, sector + count, (BYTE *)scratch, 1) ;
      if(status == USBH_FAIL)
      {
        break;
      }
    }
#else
    return res;
#endif
  }
  else
  {
    status = USBH_MSC_Write(&HOST_HANDLE, lun, sector, (BYTE *)buff, count);
  }
  
  if(status == USBH_OK)
  {
    res = RES_OK;
  }
  else
  {
    USBH_MSC_GetLUNInfo(&HOST_HANDLE, lun, &info); 
    
    switch (info.sense.asc)
    {
    case SCSI_ASC_WRITE_PROTECTED:
      USBH_ErrLog("USB Disk is Write protected!");
      res = RES_WRPRT;
      break;
      
    case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
    case SCSI_ASC_MEDIUM_NOT_PRESENT:
    case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
      USBH_ErrLog("USB Disk is not ready!");      
      res = RES_NOTRDY;
      break; 
      
    default:
      res = RES_ERROR;
      break;
    }
  }
  
  return res;   
}
#endif /* _USE_WRITE == 1 */



#if _USE_IOCTL == 1
DRESULT USBH_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC: 
    res = RES_OK;
    break;
    
  /* Get number of sectors on the disk (DWORD) */  
  case GET_SECTOR_COUNT : 
    if(USBH_MSC_GetLUNInfo(&HOST_HANDLE, lun, &info) == USBH_OK)
    {
      *(DWORD*)buff = info.capacity.block_nbr;
      res = RES_OK;
    }
    else
    {
      res = RES_ERROR;
    }
    break;
    
  /* Get R/W sector size (WORD) */  
  case GET_SECTOR_SIZE :	
    if(USBH_MSC_GetLUNInfo(&HOST_HANDLE, lun, &info) == USBH_OK)
    {
      *(DWORD*)buff = info.capacity.block_size;
      res = RES_OK;
    }
    else
    {
      res = RES_ERROR;
    }
    break;
    
    /* Get erase block size in unit of sector (DWORD) */ 
  case GET_BLOCK_SIZE : 
    
    if(USBH_MSC_GetLUNInfo(&HOST_HANDLE, lun, &info) == USBH_OK)
    {
      *(DWORD*)buff = info.capacity.block_size;
      res = RES_OK;
    }
    else
    {
      res = RES_ERROR;
    }
    break;
    
  default:
    res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL == 1 */


/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat = 0;
	if(flag == 0)
	{
		flag = 1;
		fat_FLASH = (FATFS *)bsp_mem_Malloc(SRAMIN, sizeof(FATFS));
		file = (FIL *)bsp_mem_Malloc(SRAMIN, sizeof(FIL));
	}
	
	if(disk.is_initialized[pdrv] == 0)
	{
		disk.is_initialized[pdrv] = 1;
		stat = disk.drv[pdrv]->disk_initialize(disk.lun[pdrv]);
	}
	
	return stat; //初始化成功
}  


/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{ 
	DSTATUS stat = 0;
	
	stat = disk.drv[pdrv]->disk_status(disk.lun[pdrv]);
	
	return stat;
} 

//读扇区
//drv:磁盘编号0~9
//*buff:数据接收缓冲首地址
//sector:扇区地址
//count:需要读取的扇区数
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	DRESULT res = RES_OK; 
	if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	
	res = disk.drv[pdrv]->disk_read(disk.lun[pdrv], buff, sector, count);
	
	return res;	   
}

//写扇区
//drv:磁盘编号0~9
//*buff:发送数据首地址
//sector:扇区地址
//count:需要写入的扇区数
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	
	DRESULT res = RES_OK;  
  if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 

	res = disk.drv[pdrv]->disk_write(disk.lun[pdrv], buff, sector, count);
	
	return res;
}
#endif


//其他表参数的获得
 //drv:磁盘编号0~9
 //ctrl:控制代码
 //*buff:发送/接收缓冲区指针
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;
	res = disk.drv[pdrv]->disk_ioctl(disk.lun[pdrv], cmd, buff);
	
	return res;
}
#endif
//获得时间
//User defined function to give a current time to fatfs module      
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)                                                                                                                                                                                                                                           
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)                                                                                                                                                                                                                                                 
DWORD get_fattime (void)
{				 
	return 0;
}			 
//动态分配内存
void *ff_memalloc (UINT size)			
{
	return (void*)bsp_mem_Malloc(SRAMIN,size);
}
//释放内存
void ff_memfree (void* mf)		 
{
	bsp_mem_Free(SRAMIN,mf);
}

void fatfs_init(void)
{
	FATFS_LinkDriverEx((Diskio_drvTypeDef *)&Flash_Disk, Flash_Path, 0);
	FATFS_LinkDriverEx((Diskio_drvTypeDef *)&USB_Disk, USB_Path, 1);
}



uint8_t FATFS_LinkDriverEx(Diskio_drvTypeDef *drv, char *path, uint8_t lun)
{
  uint8_t ret = 1;
  uint8_t DiskNum = 0;
  
  if(disk.nbr <= _VOLUMES)
  {
    disk.is_initialized[disk.nbr] = 0;
    disk.drv[disk.nbr] = drv;  
    disk.lun[disk.nbr] = lun;  
    DiskNum = disk.nbr++;
    path[0] = DiskNum + '0';
    path[1] = ':';
    path[2] = '/';
    path[3] = 0;
    ret = 0;
  }
  
  return ret;
}

/**
  * @brief  Links a compatible diskio driver and increments the number of active
  *         linked drivers.          
  * @note   The number of linked drivers (volumes) is up to 10 due to FatFs limits
  * @param  drv: pointer to the disk IO Driver structure
  * @param  path: pointer to the logical drive path 
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_LinkDriver(Diskio_drvTypeDef *drv, char *path)
{
  return FATFS_LinkDriverEx(drv, path, 0);
}

/**
  * @brief  Unlinks a diskio driver and decrements the number of active linked
  *         drivers.
  * @param  path: pointer to the logical drive path  
  * @param  lun : not used   
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_UnLinkDriverEx(char *path, uint8_t lun)
{ 
  uint8_t DiskNum = 0;
  uint8_t ret = 1;
  
  if(disk.nbr >= 1)
  {    
    DiskNum = path[0] - '0';
    if(disk.drv[DiskNum] != 0)
    {
      disk.drv[DiskNum] = 0;
      disk.lun[DiskNum] = 0;
      disk.nbr--;
      ret = 0;
    }
  }
  
  return ret;
}

/**
  * @brief  Unlinks a diskio driver and decrements the number of active linked
  *         drivers.
  * @param  path: pointer to the logical drive path  
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_UnLinkDriver(char *path)
{ 
  return FATFS_UnLinkDriverEx(path, 0);
}

/**
  * @brief  Gets number of linked drivers to the FatFs module.
  * @param  None
  * @retval Number of attached drivers.
  */
uint8_t FATFS_GetAttachedDriversNbr(void)
{
  return disk.nbr;
}













