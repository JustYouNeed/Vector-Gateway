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

//����W25Q128
//ǰ12M�ֽڸ�fatfs��,12M�ֽں�,���ڴ���ֿ�,	    
uint16_t	    FLASH_SECTOR_COUNT=2048*12;	//W25Q1218,ǰ12M�ֽڸ�FATFSռ��
# define FLASH_BLOCK_SIZE   	8     	//ÿ��BLOCK��8������
# define FLASH_DISK			1
# define USB_DISK				0

FATFS * fat_FLASH;
FATFS * fat_USB;

static uint8_t flag = 0;

Disk_drvTypeDef disk = {{0},{0},{0},0};		/*  ���̿��ƽṹ��  */



char Flash_Path[3];		/*  Flash��Ŀ¼  */

/*  Flash Disk���ƽṹ��  */
const Diskio_drvTypeDef Flash_Disk = 
{
	Flash_initialize,		/*  Flash��ʼ������  */
	Flash_status,		/*  ��ȡFlash״̬  */
	Flash_read,		/*  ��Flash  */
	Flash_write,	/*  дFlash  */
	Flash_ioctl,	/*  ��ȡFlash�����Ϣ  */
};


extern USBH_HandleTypeDef  HOST_HANDLE;

static DWORD scratch[_MAX_SS / 4];


char USB_Path[3];			/*  U�̸�Ŀ¼  */
/*  U�̿��ƽṹ��  */
const Diskio_drvTypeDef  USB_Disk =
{
  USBH_initialize,
  USBH_status,
  USBH_read, 
  USBH_write,
  USBH_ioctl,
};

/******************************************************  �ָ���  **************************************************************/

/***************************************************  Flash��������  **********************************************************/

/*
*********************************************************************************************************
*                                        Flash_initialize  
*
* Description: ��ʼ��Flash
*             
* Arguments  : 1> lun:δʹ��,��Ч����
*
* Reutrn     : RES_OK:��ʼ���ɹ�
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
* Description: ��ȡFlash״̬����
*             
* Arguments  : 1> lun:δʹ��,��Ч����
*
* Reutrn     : RES_OK: Flash״̬����
*
* Note(s)    : ����FLash��˵,״̬�������õ�
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
* Description: ��ȡFlash����Ϣ
*             
* Arguments  : 1> lun:δʹ��,��Ч����
*              2> cmd:����
*              3> buff:��Ϣ�洢������
*
* Reutrn     : DRESULTö�����ͱ���,����ִ�н��
*              1> RES_OK: ����ִ�гɹ�
*              2> ����: ִ��ʧ��
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
* Description: ��Flash����
*             
* Arguments  : 1> lun:δʹ��,��Ч����
*              2> buff: ���ݻ�����
*              3> sector: ��������ַ
*              4> count: Ҫ��ȡ����������
*
* Reutrn     : DRESULTö�����ͱ���,����ִ�н��
*              1> RES_OK: ����ִ�гɹ�
*              2> ����: ִ��ʧ��
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
* Description: дFlash����
*             
* Arguments  : 1> lun:δʹ��,��Ч����
*              2> buff: ���ݻ�����
*              3> sector: д������ַ
*              4> count: Ҫд�����������
*
* Reutrn     : DRESULTö�����ͱ���,����ִ�н��
*              1> RES_OK: ����ִ�гɹ�
*              2> ����: ִ��ʧ��
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



/******************************************************  �ָ���  **************************************************************/


/****************************************************  U����������  ***********************************************************/

/*
*********************************************************************************************************
*                                    USBH_initialize      
*
* Description: U�̳�ʼ������
*             
* Arguments  : 1> lun:δʹ��,��Ч����
*
* Reutrn     : 1> RES_OK:����ִ�гɹ�
*
* Note(s)    : ����U����˵,�ú���Ϊ��,�ʷ���ֵ��ΪRES_OK
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
* Description: ��ȡU��״̬����
*             
* Arguments  : 1> lun:δʹ��,��Ч���� 
*
* Reutrn     : 1> RES_OK: U���Ѿ�׼����
*              2> RES_ERROR: U�̻�δ׼����
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
* Description: ��U�̺���
*             
* Arguments  : 1> lun:δʹ��,��Ч����
*              2> buff: ���ݻ�����
*              3> sector: ��������ַ
*              4> count: Ҫ��ȡ����������
*
* Reutrn     : DRESULTö�����ͱ���,����ִ�н��
*              1> RES_OK: ����ִ�гɹ�
*              2> ����: ִ��ʧ��
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
* Description: дU�̺���
*             
* Arguments  : 1> lun:δʹ��,��Ч����
*              2> buff: ���ݻ�����
*              3> sector: д������ַ
*              4> count: Ҫд�����������
*
* Reutrn     : DRESULTö�����ͱ���,����ִ�н��
*              1> RES_OK: ����ִ�гɹ�
*              2> ����: ִ��ʧ��
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
* Description: ��ȡU�̵���Ϣ
*             
* Arguments  : 1> lun:δʹ��,��Ч����
*              2> cmd:����
*              3> buff:��Ϣ�洢������
*
* Reutrn     : DRESULTö�����ͱ���,����ִ�н��
*              1> RES_OK: ����ִ�гɹ�
*              2> ����: ִ��ʧ��
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



/******************************************************  �ָ���  **************************************************************/


/*********************************************   �����������������  ********************************************************/

/*
*********************************************************************************************************
*                                         fatfs_init 
*
* Description: ��ϵͳ��ӵ�еĴ�����������,���ڹ���
*             
* Arguments  : None.
*
* Reutrn     : None.
*
* Note(s)    : �ú�����Ҫ����ʵ�ʴ���ʹ�������ֲ
*********************************************************************************************************
*/
uint8_t fatfs_init(void)
{
	uint8_t result = FR_OK;
	
	/*  Ϊ�������������ļ�ϵͳ�ռ�  */
	fat_FLASH = (FATFS *)bsp_mem_Malloc(SRAMIN, sizeof(FATFS));
	fat_USB = (FATFS *)bsp_mem_Malloc(SRAMIN, sizeof(FATFS));
	
	FATFS_LinkDriverEx((Diskio_drvTypeDef *)&USB_Disk, USB_Path, USB_DISK);  /*  U��,���0,��Ŀ¼:0:/  */
	FATFS_LinkDriverEx((Diskio_drvTypeDef *)&Flash_Disk, Flash_Path, FLASH_DISK);	/*  Flash,���1,��Ŀ¼:1:/  */
	
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
* Description: ���ⲿ�������ڲ����ƿ���������
*             
* Arguments  : 1> drv: �ⲿ���̿��ƿ�
*              2> path: �ⲿ���̹��غ�ĸ�Ŀ¼
*              3> lun: ���ش��̱��
*
* Reutrn     : 1> 0: ���ӳɹ�
*              2> 1: ʧ��
*
* Note(s)    : ���Ӵ�������������,��FATFS�ļ�ϵͳ�ɹ��صĴ�����������
*********************************************************************************************************
*/
uint8_t FATFS_LinkDriverEx(Diskio_drvTypeDef *drv, char *path, uint8_t lun)
{
  uint8_t ret = 1;
  uint8_t DiskNum = 0;
  
  if(disk.nbr <= _VOLUMES)  /*  ���ɹ��� _VOLUMES ������ */
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
* Description: ����һ�����ݵĴ���IO��������,�����ӻ�������������������
*             
* Arguments  : 1> drv: ָ�����IO��������ṹ��ָ��
*              2> path: ָ���߼�������·����ָ��
*
* Reutrn     : 1> 0: ���ӳɹ�
*              2> 1: ����ʧ��
*
* Note(s)    : ����FATFS������,���ӵ���������(��)�������Ϊ10��
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
* Description: ȡ�����Ӵ���������,�����ٻ�������������������
*             
* Arguments  : 1> path: ָ���߼�������·����ָ��
*              2> lun: δʹ��
*
* Reutrn     : 1> 0: ж�سɹ�
*              2> 1: ж��ʧ�� 
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
* Description: ȡ�����Ӵ���������,�����ٻ�������������������
*             
* Arguments  : 1> path: ָ���߼�������·����ָ��
*
* Reutrn     : 1> 0: ж�سɹ�
*              2> 1: ж��ʧ�� 
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
* Description: ��ȡFATFS�ļ�ϵͳ�������������������
*             
* Arguments  : None.
*
* Reutrn     : ���ӵ��������������
*
* Note(s)    : None.
*********************************************************************************************************
*/
uint8_t FATFS_GetAttachedDriversNbr(void)
{
  return disk.nbr;
}

/******************************************************  �ָ���  **************************************************************/


/*****************************************  �ļ�ϵͳ�����ڴ溯��,��Ҫ��ֲ  ****************************************************/

/*
*********************************************************************************************************
*                                    ff_memalloc      
*
* Description: FATFS�ļ�ϵͳ��̬�����ڴ溯��
*             
* Arguments  : 1> size: Ҫ������ڴ��С
*
* Reutrn     : 1> void*: �ڴ����ɹ�
*              2> NULL: ����ʧ��
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
* Description: FATFS�ļ�ϵͳ�ͷ��ڴ溯��
*             
* Arguments  : 1> mf: ָ���ڴ��ָ��
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

/******************************************************  �ָ���  **************************************************************/



/**************************  ���²���ΪFATFS�ļ�ϵͳ����,����������������ķ�ʽ���Բ���Ҫ�޸�  *********************************/

/*
*********************************************************************************************************
*                                 disk_initialize         
*
* Description: ���̳�ʼ������
*             
* Arguments  : 1> pdr: ���̱��
*
* Reutrn     : 1> 0: ��ʼ���ɹ�
*              2> ����: ��ʼ��ʧ��
*
* Note(s)    : None.
*********************************************************************************************************
*/
DSTATUS disk_initialize (BYTE pdrv)
{
	DSTATUS stat = 0;
	
	if(disk.is_initialized[pdrv] == 0)  /*  ��������Ѿ���ʼ�����˾Ͳ���Ҫ�ٳ�ʼ��  */
	{
		disk.is_initialized[pdrv] = 1;
		stat = disk.drv[pdrv]->disk_initialize(disk.lun[pdrv]);
	}
	
	return stat; //��ʼ���ɹ�
}  


/*
*********************************************************************************************************
*                                          disk_status
*
* Description: ��ȡ������Ϣ
*             
* Arguments  : 1> pdr: ���̱�� 
*
* Reutrn     : 1> 0: ��ʼ���ɹ�
*              2> ����: ��ʼ��ʧ��
*
* Note(s)    : None.
*********************************************************************************************************
*/
DSTATUS disk_status (BYTE pdrv)
{ 
	DSTATUS stat = 0;
	
	stat = disk.drv[pdrv]->disk_status(disk.lun[pdrv]);  /*  ���ö�Ӧ�Ĵ��̿��ƺ���  */
	
	return stat;
} 


/*
*********************************************************************************************************
*                                    disk_read      
*
* Description: �����̺���
*             
* Arguments  : 1> pdrv: ���̱��
*              2> buff: ���ݻ�����
*              3> sector: ��������ַ
*              4> count: Ҫ��ȡ����������,��Χ0-128
*
* Reutrn     : DRESULTö�����ͱ���,����ִ�н��
*              1> RES_OK: ����ִ�гɹ�
*              2> ����: ִ��ʧ��
*
* Note(s)    : None.
*********************************************************************************************************
*/
DRESULT disk_read (BYTE pdrv,	BYTE *buff,	DWORD sector,	UINT count)
{
	DRESULT res = RES_OK; 
//	if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	
	res = disk.drv[pdrv]->disk_read(disk.lun[pdrv], buff, sector, count);  /*  ���ö�Ӧ�Ĵ��̿��ƺ���  */
	
	return res;	   
}

/*
*********************************************************************************************************
*                                          disk_write
*
* Description: д���̺���
*             
* Arguments  : 1> pdrv: ���̱��
*              2> buff: ���ݻ�����
*              3> sector: д������ַ
*              4> count: Ҫд�����������,,��Χ0-128
*
* Reutrn     : DRESULTö�����ͱ���,����ִ�н��
*              1> RES_OK: ����ִ�гɹ�
*              2> ����: ִ��ʧ��
*
* Note(s)    : None.
*********************************************************************************************************
*/
#if _USE_WRITE
DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
	
	DRESULT res = RES_OK;  	 	 

	res = disk.drv[pdrv]->disk_write(disk.lun[pdrv], buff, sector, count);  /*  ���ö�Ӧ�Ĵ��̿��ƺ���  */
	
	return res;
}
#endif


/*
*********************************************************************************************************
*                                       USBH_ioctl   
*
* Description: ��ȡU�̵���Ϣ
*             
* Arguments  : 1> pdrv: ���̱��
*              2> cmd:����
*              3> buff:��Ϣ�洢������
*
* Reutrn     : DRESULTö�����ͱ���,����ִ�н��
*              1> RES_OK: ����ִ�гɹ�
*              2> ����: ִ��ʧ��
*
* Note(s)    : None.
*********************************************************************************************************
*/
#if _USE_IOCTL
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff	)
{
	DRESULT res = RES_OK;
	res = disk.drv[pdrv]->disk_ioctl(disk.lun[pdrv], cmd, buff);  /*  ���ö�Ӧ�Ĵ��̿��ƺ���  */
	
	return res;
}
#endif


/*
*********************************************************************************************************
*                                          get_fattime
*
* Description: ��ȡϵͳʱ�亯��
*             
* Arguments  : None.
*
* Reutrn     : 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) 
*              15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)
*
* Note(s)    : ���ڰ�����û��ʱ��оƬ,���Դ˺���û��ʵ��,ֱ�ӷ���0
*********************************************************************************************************
*/
DWORD get_fattime (void)
{				 
	return 0;
}			 

/******************************************************  �ָ���  **************************************************************/








