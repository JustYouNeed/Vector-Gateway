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
# include "app_sys.h"
# include "usbh_msc.h"

# define FLASH_SECTOR_SIZE 	512			  

//对于W25Q128
//前12M字节给fatfs用,12M字节后,用于存放字库,	    
uint16_t	    FLASH_SECTOR_COUNT=2048*12;	//W25Q1218,前12M字节给FATFS占用
# define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区
# define FLASH_DISK			1
# define USB_DISK				0

FATFS * fat_FLASH;
FATFS * fat_USB;

static uint8_t flag = 0;

Disk_drvTypeDef disk = {{0},{0},{0},0};		/*  磁盘控制结构体  */



char Flash_Path[3];		/*  Flash根目录  */

/*  Flash Disk控制结构体  */
const Diskio_drvTypeDef Flash_Disk = 
{
	Flash_initialize,		/*  Flash初始化函数  */
	Flash_status,		/*  获取Flash状态  */
	Flash_read,		/*  读Flash  */
	Flash_write,	/*  写Flash  */
	Flash_ioctl,	/*  获取Flash相关信息  */
};


extern USBH_HandleTypeDef  HOST_HANDLE;

static DWORD scratch[_MAX_SS / 4];


char USB_Path[3];			/*  U盘根目录  */
/*  U盘控制结构体  */
const Diskio_drvTypeDef  USB_Disk =
{
  USBH_initialize,
  USBH_status,
  USBH_read, 
  USBH_write,
  USBH_ioctl,
};

/******************************************************  分割线  **************************************************************/

/***************************************************  Flash驱动程序  **********************************************************/

/*
*********************************************************************************************************
*                                        Flash_initialize  
*
* Description: 初始化Flash
*             
* Arguments  : 1> lun:未使用,无效参数
*
* Reutrn     : RES_OK:初始化成功
*
* Note(s)    : None.
*********************************************************************************************************
*/
DSTATUS Flash_initialize(BYTE lun)
{
	app_flash_Config();
	return RES_OK;
}


/*
*********************************************************************************************************
*                                         Flash_status 
*
* Description: 获取Flash状态函数
*             
* Arguments  : 1> lun:未使用,无效参数
*
* Reutrn     : RES_OK: Flash状态良好
*
* Note(s)    : 对于FLash来说,状态都是良好的
*********************************************************************************************************
*/
DSTATUS Flash_status(BYTE lun)
{
	return RES_OK;
}


/*
*********************************************************************************************************
*                                       Flash_ioctl   
*
* Description: 获取Flash的信息
*             
* Arguments  : 1> lun:未使用,无效参数
*              2> cmd:命令
*              3> buff:信息存储缓存区
*
* Reutrn     : DRESULT枚举类型变量,函数执行结果
*              1> RES_OK: 函数执行成功
*              2> 其他: 执行失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
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


/*
*********************************************************************************************************
*                                    Flash_read      
*
* Description: 读Flash函数
*             
* Arguments  : 1> lun:未使用,无效参数
*              2> buff: 数据缓存区
*              3> sector: 读扇区地址
*              4> count: 要读取的数据数量
*
* Reutrn     : DRESULT枚举类型变量,函数执行结果
*              1> RES_OK: 函数执行成功
*              2> 其他: 执行失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
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


/*
*********************************************************************************************************
*                                          Flash_write
*
* Description: 写Flash函数
*             
* Arguments  : 1> lun:未使用,无效参数
*              2> buff: 数据缓存区
*              3> sector: 写扇区地址
*              4> count: 要写入的数据数量
*
* Reutrn     : DRESULT枚举类型变量,函数执行结果
*              1> RES_OK: 函数执行成功
*              2> 其他: 执行失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
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



/******************************************************  分割线  **************************************************************/


/****************************************************  U盘驱动程序  ***********************************************************/

/*
*********************************************************************************************************
*                                    USBH_initialize      
*
* Description: U盘初始化函数
*             
* Arguments  : 1> lun:未使用,无效参数
*
* Reutrn     : 1> RES_OK:函数执行成功
*
* Note(s)    : 对于U盘来说,该函数为空,故返回值总为RES_OK
*********************************************************************************************************
*/
DSTATUS USBH_initialize(BYTE lun)
{
  return RES_OK;
}


/*
*********************************************************************************************************
*                                      USBH_status    
*
* Description: 获取U盘状态函数
*             
* Arguments  : 1> lun:未使用,无效参数 
*
* Reutrn     : 1> RES_OK: U盘已经准备好
*              2> RES_ERROR: U盘还未准备好
*
* Note(s)    : None.
*********************************************************************************************************
*/
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

/*
*********************************************************************************************************
*                                    USBH_read      
*
* Description: 读U盘函数
*             
* Arguments  : 1> lun:未使用,无效参数
*              2> buff: 数据缓存区
*              3> sector: 读扇区地址
*              4> count: 要读取的数据数量
*
* Reutrn     : DRESULT枚举类型变量,函数执行结果
*              1> RES_OK: 函数执行成功
*              2> 其他: 执行失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
DRESULT USBH_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;
  USBH_StatusTypeDef  status = USBH_OK;

  if ((DWORD)buff & 3) /* DMA Alignment issue, do single up to aligned buffer */
  {
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



/*
*********************************************************************************************************
*                                          USBH_write
*
* Description: 写U盘函数
*             
* Arguments  : 1> lun:未使用,无效参数
*              2> buff: 数据缓存区
*              3> sector: 写扇区地址
*              4> count: 要写入的数据数量
*
* Reutrn     : DRESULT枚举类型变量,函数执行结果
*              1> RES_OK: 函数执行成功
*              2> 其他: 执行失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
DRESULT USBH_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR; 
  MSC_LUNTypeDef info;
  USBH_StatusTypeDef  status = USBH_OK;  

  if ((DWORD)buff & 3) /* DMA Alignment issue, do single up to aligned buffer */
  {
    while (count--)
    {
      memcpy (scratch, &buff[count * _MAX_SS], _MAX_SS);
      
      status = USBH_MSC_Write(&HOST_HANDLE, lun, sector + count, (BYTE *)scratch, 1) ;
      if(status == USBH_FAIL)
      {
        break;
      }
    }
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


/*
*********************************************************************************************************
*                                       USBH_ioctl   
*
* Description: 获取U盘的信息
*             
* Arguments  : 1> lun:未使用,无效参数
*              2> cmd:命令
*              3> buff:信息存储缓存区
*
* Reutrn     : DRESULT枚举类型变量,函数执行结果
*              1> RES_OK: 函数执行成功
*              2> 其他: 执行失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
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



/******************************************************  分割线  **************************************************************/


/*********************************************   磁盘驱动程序管理部分  ********************************************************/

/*
*********************************************************************************************************
*                                         fatfs_init 
*
* Description: 将系统所拥有的磁盘链接起来,便于管理
*             
* Arguments  : None.
*
* Reutrn     : None.
*
* Note(s)    : 该函数需要根据实际磁盘使用情况移植
*********************************************************************************************************
*/
uint8_t fatfs_init(void)
{
	uint8_t result = FR_OK;
	
	/*  为两个磁盘申请文件系统空间  */
	fat_FLASH = (FATFS *)bsp_mem_Malloc(SRAMIN, sizeof(FATFS));
	fat_USB = (FATFS *)bsp_mem_Malloc(SRAMIN, sizeof(FATFS));
	
	FATFS_LinkDriverEx((Diskio_drvTypeDef *)&USB_Disk, USB_Path, USB_DISK);  /*  U盘,编号0,根目录:0:/  */
	FATFS_LinkDriverEx((Diskio_drvTypeDef *)&Flash_Disk, Flash_Path, FLASH_DISK);	/*  Flash,编号1,根目录:1:/  */
	
	disk_initialize(USB_DISK);
	disk_initialize(FLASH_DISK);
	
	result = f_mount(fat_FLASH, Flash_Path, 1);
	if(result != FR_OK)
	{
		printf("Flash Mount Failed\r\n");
		return result;
	}
	
	Sys_Info.FlashDiskStatus = 1;
	return result;
}


/*
*********************************************************************************************************
*                                       FATFS_LinkDriverEx   
*
* Description: 将外部磁盘与内部控制块链接起来
*             
* Arguments  : 1> drv: 外部磁盘控制块
*              2> path: 外部磁盘挂载后的根目录
*              3> lun: 挂载磁盘编号
*
* Reutrn     : 1> 0: 链接成功
*              2> 1: 失败
*
* Note(s)    : 链接磁盘有数量限制,由FATFS文件系统可挂载的磁盘数量控制
*********************************************************************************************************
*/
uint8_t FATFS_LinkDriverEx(Diskio_drvTypeDef *drv, char *path, uint8_t lun)
{
  uint8_t ret = 1;
  uint8_t DiskNum = 0;
  
  if(disk.nbr <= _VOLUMES)  /*  最多可挂载 _VOLUMES 个磁盘 */
  {
    disk.is_initialized[disk.nbr] = 0;
    disk.drv[disk.nbr] = drv;  
    disk.lun[disk.nbr] = lun;  
    DiskNum = disk.nbr++;
    path[0] = DiskNum + '0';
    path[1] = ':';
    path[2] = 0;
    ret = 0;
  }
  
  return ret;
}

/*
*********************************************************************************************************
*                                         FATFS_LinkDriver 
*
* Description: 链接一个兼容的磁盘IO驱动程序,并增加活动的链接驱动程序的数量
*             
* Arguments  : 1> drv: 指向磁盘IO驱动程序结构的指针
*              2> path: 指向逻辑驱动器路径的指针
*
* Reutrn     : 1> 0: 链接成功
*              2> 1: 链接失败
*
* Note(s)    : 由于FATFS的限制,链接的驱动程序(卷)数量最多为10个
*********************************************************************************************************
*/
uint8_t FATFS_LinkDriver(Diskio_drvTypeDef *drv, char *path)
{
  return FATFS_LinkDriverEx(drv, path, 0);
}


/*
*********************************************************************************************************
*                                 FATFS_UnLinkDriverEx         
*
* Description: 取消链接磁盘驱动器,并减少活动的链接驱动程序的数量
*             
* Arguments  : 1> path: 指向逻辑驱动器路径的指针
*              2> lun: 未使用
*
* Reutrn     : 1> 0: 卸载成功
*              2> 1: 卸载失败 
*
* Note(s)    : None.
*********************************************************************************************************
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



/*
*********************************************************************************************************
*                                    FATFS_UnLinkDriver      
*
* Description: 取消链接磁盘驱动器,并减少活动的链接驱动程序的数量
*             
* Arguments  : 1> path: 指向逻辑驱动器路径的指针
*
* Reutrn     : 1> 0: 卸载成功
*              2> 1: 卸载失败 
*
* Note(s)    : None.
*********************************************************************************************************
*/
uint8_t FATFS_UnLinkDriver(char *path)
{ 
  return FATFS_UnLinkDriverEx(path, 0);
}


/*
*********************************************************************************************************
*                              FATFS_GetAttachedDriversNbr            
*
* Description: 获取FATFS文件系统的链接驱动程序的数量
*             
* Arguments  : None.
*
* Reutrn     : 链接的驱动程序的数量
*
* Note(s)    : None.
*********************************************************************************************************
*/
uint8_t FATFS_GetAttachedDriversNbr(void)
{
  return disk.nbr;
}

/******************************************************  分割线  **************************************************************/


/*****************************************  文件系统分配内存函数,需要移植  ****************************************************/

/*
*********************************************************************************************************
*                                    ff_memalloc      
*
* Description: FATFS文件系统动态分配内存函数
*             
* Arguments  : 1> size: 要分配的内存大小
*
* Reutrn     : 1> void*: 内存分配成功
*              2> NULL: 分配失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
void *ff_memalloc (UINT size)			
{
	return (void*)bsp_mem_Malloc(SRAMIN,size);
}


/*
*********************************************************************************************************
*                                     ff_memfree     
*
* Description: FATFS文件系统释放内存函数
*             
* Arguments  : 1> mf: 指向内存的指针
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void ff_memfree (void* mf)		 
{
	bsp_mem_Free(SRAMIN,mf);
}

/******************************************************  分割线  **************************************************************/



/**************************  以下部分为FATFS文件系统调用,采用链接驱动程序的方式可以不需要修改  *********************************/

/*
*********************************************************************************************************
*                                 disk_initialize         
*
* Description: 磁盘初始化函数
*             
* Arguments  : 1> pdr: 磁盘编号
*
* Reutrn     : 1> 0: 初始化成功
*              2> 其他: 初始化失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
DSTATUS disk_initialize (BYTE pdrv)
{
	DSTATUS stat = 0;
	
	if(disk.is_initialized[pdrv] == 0)  /*  如果磁盘已经初始化过了就不需要再初始化  */
	{
		disk.is_initialized[pdrv] = 1;
		stat = disk.drv[pdrv]->disk_initialize(disk.lun[pdrv]);
	}
	
	return stat; //初始化成功
}  


/*
*********************************************************************************************************
*                                          disk_status
*
* Description: 获取磁盘信息
*             
* Arguments  : 1> pdr: 磁盘编号 
*
* Reutrn     : 1> 0: 初始化成功
*              2> 其他: 初始化失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
DSTATUS disk_status (BYTE pdrv)
{ 
	DSTATUS stat = 0;
	
	stat = disk.drv[pdrv]->disk_status(disk.lun[pdrv]);  /*  调用对应的磁盘控制函数  */
	
	return stat;
} 


/*
*********************************************************************************************************
*                                    disk_read      
*
* Description: 读磁盘函数
*             
* Arguments  : 1> pdrv: 磁盘编号
*              2> buff: 数据缓存区
*              3> sector: 读扇区地址
*              4> count: 要读取的数据数量,范围0-128
*
* Reutrn     : DRESULT枚举类型变量,函数执行结果
*              1> RES_OK: 函数执行成功
*              2> 其他: 执行失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
DRESULT disk_read (BYTE pdrv,	BYTE *buff,	DWORD sector,	UINT count)
{
	DRESULT res = RES_OK; 
//	if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	
	res = disk.drv[pdrv]->disk_read(disk.lun[pdrv], buff, sector, count);  /*  调用对应的磁盘控制函数  */
	
	return res;	   
}

/*
*********************************************************************************************************
*                                          disk_write
*
* Description: 写磁盘函数
*             
* Arguments  : 1> pdrv: 磁盘编号
*              2> buff: 数据缓存区
*              3> sector: 写扇区地址
*              4> count: 要写入的数据数量,,范围0-128
*
* Reutrn     : DRESULT枚举类型变量,函数执行结果
*              1> RES_OK: 函数执行成功
*              2> 其他: 执行失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
#if _USE_WRITE
DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
	
	DRESULT res = RES_OK;  	 	 

	res = disk.drv[pdrv]->disk_write(disk.lun[pdrv], buff, sector, count);  /*  调用对应的磁盘控制函数  */
	
	return res;
}
#endif


/*
*********************************************************************************************************
*                                       USBH_ioctl   
*
* Description: 获取U盘的信息
*             
* Arguments  : 1> pdrv: 磁盘编号
*              2> cmd:命令
*              3> buff:信息存储缓存区
*
* Reutrn     : DRESULT枚举类型变量,函数执行结果
*              1> RES_OK: 函数执行成功
*              2> 其他: 执行失败
*
* Note(s)    : None.
*********************************************************************************************************
*/
#if _USE_IOCTL
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff	)
{
	DRESULT res = RES_OK;
	res = disk.drv[pdrv]->disk_ioctl(disk.lun[pdrv], cmd, buff);  /*  调用对应的磁盘控制函数  */
	
	return res;
}
#endif


/*
*********************************************************************************************************
*                                          get_fattime
*
* Description: 获取系统时间函数
*             
* Arguments  : None.
*
* Reutrn     : 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) 
*              15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)
*
* Note(s)    : 由于板子上没有时钟芯片,所以此函数没有实现,直接返回0
*********************************************************************************************************
*/
DWORD get_fattime (void)
{				 
	return 0;
}			 

/******************************************************  分割线  **************************************************************/








