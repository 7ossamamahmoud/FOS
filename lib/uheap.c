
#include <inc/lib.h>

// malloc()
//	This function use NEXT FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
struct alloc
{
	uint32 T_add,B_add;
}alloc_arr[(USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE];
uint32 c = (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE ,pos = 0, arr[1024*1024],glob_S = (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE,n_itr = 0;
void* malloc(uint32 size)
{
	//TODO: [PROJECT 2022 - [9] User Heap malloc()] [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	int FF = 0;
	uint32 re = 0, Cur_S = ROUNDUP(size, PAGE_SIZE),ree = 0,indx2,indx,Upd_S = Cur_S,F = 0;
	uint32 O_S = (Upd_S / PAGE_SIZE);

	if (O_S > glob_S)
			return NULL;

	for (indx = n_itr; indx <= c; indx++)
	{
		if (indx == c)
		{
			indx = 0;
		    F = 0;
		}
		if (indx == n_itr)
			FF += 1;

		if (FF == 2)
			return NULL;

		if (arr[indx] == 0)
		{
			if (F == 0)
			{
				alloc_arr[pos].T_add = USER_HEAP_START + (indx * PAGE_SIZE);
				re = indx;
			}
			alloc_arr[pos].B_add = USER_HEAP_START + (indx * PAGE_SIZE);
			ree = indx;
			F += PAGE_SIZE;
			Cur_S = Cur_S - PAGE_SIZE;
		}
		else
			F = 0;
		if (F == Upd_S) break;
	}
	if (sys_isUHeapPlacementStrategyNEXTFIT() != 1)
		panic("Next Fit strategy is NOT working");
	if (F == Upd_S)
	{
		sys_allocateMem(alloc_arr[pos].T_add, ree - re);
		glob_S = glob_S - (ree - re);
		for (indx2 = re; indx2 <= ree; indx2++)
			arr[indx2] = 1;
	}
	n_itr = ree + 1;
	return (void *)(alloc_arr[pos++].T_add);
	// Steps:
	//	1) Implement NEXT FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	// 	4) Return pointer containing the virtual address of allocated space,
	//

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyNEXTFIT() and
	//sys_isUHeapPlacementStrategyBESTFIT() for the bonus
	//to check the current strategy

}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("smalloc() is not required ..!!");
	return NULL;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("sget() is not required ..!!");
	return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2022 - [11] User Heap free()] [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	uint32 Bound_UP,Bound_Down,F_indx,indx;
	for (F_indx = 0; F_indx < pos; F_indx++)
	{
		if ((uint32)virtual_address == alloc_arr[F_indx].T_add)
		{
			Bound_UP = ((uint32)virtual_address - USER_HEAP_START)/PAGE_SIZE;
			Bound_Down = (alloc_arr[F_indx].B_add - USER_HEAP_START)/PAGE_SIZE;
			indx = Bound_UP;
			while (indx <= Bound_Down)
			{
				arr[indx] = 0;
				glob_S++;
				indx++;
			}
			sys_freeMem((uint32)virtual_address, alloc_arr[F_indx].B_add);
			alloc_arr[F_indx].T_add = 0;
			alloc_arr[F_indx].B_add = 0;
			break;
		}
	}
	//you shold get the size of the given allocation using its address
	//you need to call sys_freeMem()
	//refer to the project presentation and documentation for details
}


void sfree(void* virtual_address)
{
	panic("sfree() is not requried ..!!");
}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2022 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}
