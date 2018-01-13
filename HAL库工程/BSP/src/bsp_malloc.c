# include "bsp_malloc.h"
# include "bsp_sram.h"

# if USE_SRAMIN > 0u
/*  先申请一个大数组，用作内存池  */
__align(32) uint8_t MEM_SRAMIN[SRAMIN_MAX_SIZE];
/*  内存管理的地址表  */
uint16_t MAP_SRAMIN[SRAMIN_TABLE_SIZE];
# endif

# if USE_SRAMEX > 0u
/*  先在SRAM中申请一个大数组，用作内存池  */
__align(32) uint8_t MEM_SRAMEX[SRAMEX_MAX_SIZE] __attribute__((at(SRAMEX_BASE)));
/*  内存管理的地址表  */
uint16_t MAP_SRAMEX[SRAMEX_TABLE_SIZE] __attribute__((at(SRAMEX_BASE + SRAMEX_MAX_SIZE)));
# endif

# if USE_SRAMCCM > 0u
/*  在CCM内存中申请一个大数组，用作内存池  */
__align(32) uint8_t MEM_SRAMCCM[SRAMCCM_MAX_SIZE] __attribute__((at(SRAMCCM_BASE)));
/* 内存管理的地址表   */
uint16_t MAP_SRAMCCM[SRAMCCM_TABLE_SIZE] __attribute__((at(SRAMCCM_BASE + SRAMCCM_MAX_SIZE)));
# endif





/*  内存管理的地址表  */
const uint32_t MEM_TABLE_SIZE[SRAMBANK] = {
																						# if USE_SRAMIN > 0u
																						SRAMIN_TABLE_SIZE,
																						# endif

																						# if USE_SRAMEX > 0u
																						SRAMEX_TABLE_SIZE, 
																						# endif

																						# if USE_SRAMCCM > 0u	
																						SRAMCCM_TABLE_SIZE
																						# endif
																						};
/*  内存管理的地址表  */
const uint32_t MEM_BLOCK_SIZE[SRAMBANK] = {
																						# if USE_SRAMIN > 0u
																						SRAMIN_BLOCK_SIZE, 
																						# endif
																							
																						# if USE_SRAMEX > 0u
																						SRAMEX_BLOCK_SIZE, 
																						# endif
																							
																						# if USE_SRAMCCM > 0u	
																						SRAMCCM_BLOCK_SIZE
																						# endif
																							};
/*  内存管理的地址表  */
const uint32_t MEM_SIZE[SRAMBANK] = {
																			# if USE_SRAMIN > 0u
																			SRAMIN_MAX_SIZE,
																			# endif 
																				
																			# if USE_SRAMEX > 0u
																			SRAMEX_MAX_SIZE, 
																			# endif
																				
																			# if USE_SRAMCCM > 0u	
																			SRAMCCM_MAX_SIZE
																			# endif
																			};

/*  内存管理结构体  */
Malloc_Str malloc_dev = 
{
	bsp_mem_Config,
	# if USE_SRAMEX > 0u
	FSMC_SRAM_Init,
	# endif
	bsp_mem_GetUsed,

	# if USE_SRAMIN > 0u
	MEM_SRAMIN,
	# endif
	
	# if USE_SRAMEX > 0u
	MEM_SRAMEX,
	# endif
	
	# if USE_SRAMCCM > 0u	
	MEM_SRAMCCM,
	# endif
	
	# if USE_SRAMIN > 0u
	MAP_SRAMIN,
	# endif
	
	# if USE_SRAMEX > 0u
	MAP_SRAMEX,
	# endif
	
	# if USE_SRAMCCM > 0u	
	MAP_SRAMCCM,
	# endif
	
	# if USE_SRAMIN > 0u
	0,
	# endif
	
	# if USE_SRAMEX > 0u
	0,
	# endif
	
	# if USE_SRAMCCM > 0u	
	0,
	# endif
};


/*
*********************************************************************************************************
*                                          bsp_mem_Set
*
* Description: 内存设置函数
*             
* Arguments : s:要设置的内存基地址
*							value:设置的值
*							count:内存长度
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_mem_Set(void *s, uint8_t value, uint32_t len)
{
	uint8_t *xs = s;
	while(len -- ) *xs++ = value;
}


/*
*********************************************************************************************************
*                                          bsp_mem_Copy
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_mem_Copy(void *des, void *src, uint32_t len)
{
	uint8_t *xdes = des;
	uint8_t *xsrc = src;
	while(len -- )*xdes++ = *xsrc++;
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_mem_Config(void)
{
# if USE_SRAMIN > 0u
	bsp_mem_Set(malloc_dev.memmap[SRAMIN], 0, MEM_TABLE_SIZE[SRAMIN] * 2);
	bsp_mem_Set(malloc_dev.membase[SRAMIN], 0, MEM_SIZE[SRAMIN]);
	malloc_dev.memready[SRAMIN] = 1;
# endif
# if USE_SRAMEX > 0u
	if(malloc_dev.sram_config != 0)
		malloc_dev.sram_config();
	bsp_mem_Set(malloc_dev.memmap[SRAMEX], 0, MEM_TABLE_SIZE[SRAMEX] * 2);
	bsp_mem_Set(malloc_dev.membase[SRAMEX], 0, MEM_SIZE[SRAMEX]);
	malloc_dev.memready[SRAMEX] = 1;
# endif
# if USE_SRAMCCM > 0u
	bsp_mem_Set(malloc_dev.memmap[SRAMCCM], 0, MEM_TABLE_SIZE[SRAMCCM] * 2);
	bsp_mem_Set(malloc_dev.membase[SRAMCCM], 0, MEM_SIZE[SRAMCCM]);
	malloc_dev.memready[SRAMCCM] = 1;
# endif
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint8_t bsp_mem_GetUsed(uint8_t memx)
{
	uint32_t used = 0;
	uint32_t cnt = 0;
	
	for(cnt = 0; cnt < MEM_TABLE_SIZE[memx]; cnt++)
	{
		if(malloc_dev.memmap[memx][cnt]) used ++;
	}
	return (used * 100 )/(MEM_TABLE_SIZE[memx]);
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint32_t bsp_mem_mallocIN(uint8_t memx, uint32_t size)
{
	signed long offset = 0;
	uint32_t nmemblocks;
	uint32_t cmemblocks;
	uint32_t i;
	
	if(! malloc_dev.memready[memx])malloc_dev.malloc_config();
	if(size == 0) return 0XFFFFFFFF;
	nmemblocks = size/MEM_BLOCK_SIZE[memx];
	if(size % MEM_BLOCK_SIZE[memx])nmemblocks++;
	for(offset = MEM_TABLE_SIZE[memx] - 1; offset >= 0; offset --)
	{
		if(!malloc_dev.memmap[memx][offset]) cmemblocks ++;
		else cmemblocks = 0;
		if(cmemblocks == nmemblocks)
		{
			for(i = 0; i< nmemblocks; i++)
			{
				malloc_dev.memmap[memx][offset + i] = nmemblocks;
			}
			
			return (offset * MEM_BLOCK_SIZE[memx]);
			
		}
	}
	return 0XFFFFFFFF;
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint8_t bsp_mem_freeIN(uint8_t memx, uint32_t offset)
{
	int i = 0;
	int index = 0;
	int nmemblocks = 0;
	if(!malloc_dev.memready[memx]) 
	{
		malloc_dev.malloc_config();
		return 1;
	}
	if(offset < MEM_SIZE[memx])
	{
		index = offset / MEM_BLOCK_SIZE[memx];
		nmemblocks = malloc_dev.memmap[memx][index];
		for(i = 0; i < nmemblocks; i++)
		{
			malloc_dev.memmap[memx][index + i] = 0;
		}
		return 0;
	}else 
		return 2;
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void *bsp_mem_Malloc(uint8_t memx, uint32_t size)
{
	uint32_t offset = 0;
	offset = bsp_mem_mallocIN(memx, size);
	if(offset == 0XFFFFFFF) return NULL;
	else return (void *)((uint32_t )malloc_dev.membase[memx] + offset);
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_mem_Free(uint8_t memx, void *mem)
{
	uint32_t offset = 0;
	if(mem == NULL) return;
	offset = (uint32_t)mem - (uint32_t)malloc_dev.membase[memx];
	bsp_mem_freeIN(memx, offset);
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void *bsp_mem_Realloc(uint8_t memx, void *mem, uint32_t size)
{
	uint32_t offset = 0;
	offset = bsp_mem_mallocIN(memx, size);
	if(0XFFFFFFFF == offset) return NULL;
	else
	{
		bsp_mem_Copy((void *)((uint32_t)malloc_dev.membase[memx] + offset), mem, size);
		bsp_mem_Free(memx, mem);
		return (void*)((uint32_t)malloc_dev.membase[memx]+offset);
	}
}




