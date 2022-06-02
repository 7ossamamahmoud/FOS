#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
struct alloc
{
	uint32 Top_addr,Bottom_addr;
}h_arr[1024*1024];
uint32 Next_indx = 0,start_of_next = KERNEL_HEAP_START;
void* kmalloc(unsigned int size)
{
	uint32 next,indx = 0,*ptr_PT;
	uint32 Current_size = ROUNDUP(size, PAGE_SIZE),Updated_size = Current_size;
	int c = 0;
	for (next = start_of_next; next <= KERNEL_HEAP_MAX; next += PAGE_SIZE)
	{
		if (next == start_of_next)
			c += 1;

		if (c == 2)
			if (indx != Updated_size)
				return NULL;

		if (next == KERNEL_HEAP_MAX)
			next = KERNEL_HEAP_START;

		if (get_frame_info(ptr_page_directory, (void *)next, &ptr_PT) == NULL)
		{
			if (indx == 0)
				h_arr[Next_indx].Top_addr = next;
			h_arr[Next_indx].Bottom_addr = next;
			Current_size -= PAGE_SIZE;
			indx += PAGE_SIZE;
		}
		else
			indx = 0;

		if (indx == Updated_size) break;
	}

	if (indx == Updated_size)
	{
		struct Frame_Info *F_Info;
		uint32 ind = h_arr[Next_indx].Top_addr;
		while (ind <= h_arr[Next_indx].Bottom_addr)
		{
			allocate_frame(&F_Info);
			map_frame(ptr_page_directory, F_Info, (void*)ind,PERM_WRITEABLE | PERM_PRESENT);
			ind += PAGE_SIZE;
		}
	}

	start_of_next = h_arr[Next_indx].Bottom_addr;
	start_of_next+=PAGE_SIZE;
	return (void*)(h_arr[Next_indx++].Top_addr);
}

void kfree(void* virtual_address)
{
	uint32 addr_boundUp,found=0,addr_boundDown,indd,*ptr_PT,indx=0;
	for (indx = 0 ;indx < Next_indx;indx++)
	{
		for (uint32 F_indx = h_arr[indx].Top_addr; F_indx <= h_arr[indx].Bottom_addr; F_indx += PAGE_SIZE)
		{
			if ((uint32)virtual_address == F_indx)
			{
				found = 1;
				addr_boundUp = (uint32)virtual_address;
				addr_boundDown = h_arr[indx].Bottom_addr;
				break;
			}
		}
	}
	if (found)
	{

		for (uint32 ind = addr_boundUp; ind <= addr_boundDown;ind += PAGE_SIZE)
		{
			unmap_frame(ptr_page_directory, (void*)ind);
			get_page_table(ptr_page_directory, (void*)ind, &ptr_PT);
			indd = PTX(ind);
			ptr_PT[indd] = 0;
		}
	}
}
unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2022 - [3] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	uint32 virt_addr,frame_Num=(physical_address/PAGE_SIZE);
	for (virt_addr = KERNEL_HEAP_START; virt_addr < KERNEL_HEAP_MAX; virt_addr += PAGE_SIZE)
	{
		uint32 *ptr_PT = NULL , indd=PTX((void*)virt_addr);
		get_page_table(ptr_page_directory,(void*)virt_addr, &ptr_PT);
		uint32 p_va = ptr_PT[indd] & PERM_PRESENT;
		uint32 f = ptr_PT[PTX(virt_addr)] >> 12;
		if ( f == frame_Num )
			if(p_va != 0)
			return virt_addr;
	}
	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2022 - [4] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code

    uint32 *ptr_PT = NULL,F_Num;
	get_page_table(ptr_page_directory,(void*)virtual_address,&ptr_PT);
	if(ptr_PT != NULL)
	{
	    F_Num = (ptr_PT[PTX(virtual_address)] >> 12) * PAGE_SIZE;
		return F_Num += (virtual_address & 0x00000FFF);
	}
	return 0;
}

