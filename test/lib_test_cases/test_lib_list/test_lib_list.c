/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2019 Thomas Willetal 
 * (https://github.com/tom3333)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/* system */

/* frame */
#include <test_cases.h>
#include <lib_list.h>
#include <lib_convention__macro.h>
#include <lib_convention__mem.h>
#include <lib_convention__errno.h>
#include <lib_thread.h>

#ifdef __gnu_linux__
#include <sched.h> // POSIX scheduling parameter functions and definitions
#include <pthread.h>
#include <unistd.h>
#endif

/* project */
#include <embUnit.h>
#include <test_cases.h>

#include <test_lib_list.h>

/* *******************************************************************
 * Defines
 * ******************************************************************/
#define TEST_INTERVAL 30000

#define GET_TEST_QUEUE_DATA(new_entry_list) GET_CONTAINER_OF(new_entry_list, struct test_queue_data, node)

#define EN_DE_QUEUE_WORKER_ENTRY(_name, _enq_deq_nmbr, _rel_prio, _context_id) \
	{                                                                          \
		.name = _name,                                                         \
		.running = 0,                                                          \
		.hdl = NULL,                                                           \
		.enq_deq_nmbr = _enq_deq_nmbr,                                         \
		.rel_prio = _rel_prio,                                                 \
		.context_id = _context_id,                                             \
		.alloc_function_selector = 0                                           \
	}

#define M_ICB_FIFO_BUF_SIZE(entry_size, entry_count) M_MEM_SIZE_1__MEM_INFO_ATTR +               \
														 M_MEM_SIZE_2__ENTRY_LOCK(entry_count) + \
														 M_MEM_SIZE_3__ENTRY_DATA(entry_count, ALIGN(entry_size, sizeof(uint32_t)))

/* *******************************************************************
 * Configuration
 * ******************************************************************/
#define M_TEST_ENTRY_COUNT 1600
#define M_TEST_ENTRY_COUNT_CHAIN_LIST 20

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
struct test_queue_data
{
	char *thd_name;
	unsigned int data;
	struct list_node node;
};

struct test_list_data
{
	char data[20];
	struct list_node node;
	unsigned int prio;
};

struct en_de_queue_thd_info
{
	char *name;
	int running;
	thread_hdl_t hdl;
	int enq_deq_nmbr;
	int rel_prio;
	uint32_t context_id;
	int alloc_function_selector;
};

/* *******************************************************************
 * Static Variables
/* *******************************************************************/

static struct queue_attr test_list;
static mem_hdl_t test_mem;

static struct queue_attr test_chain_list;
static mem_hdl_t test_chain_mem;
void *s_test_chain_base;

struct en_de_queue_thd_info s_enqueue_info[] = {/* name, 	enq_deq_nmbr, rel_prio, _context_id */
												EN_DE_QUEUE_WORKER_ENTRY("ThreadTestP17", 7, 4, 0),
												EN_DE_QUEUE_WORKER_ENTRY("ThreadTestP16", 3, 3, 1),
												EN_DE_QUEUE_WORKER_ENTRY("ThreadTestP14", 8, 1, 2),
												EN_DE_QUEUE_WORKER_ENTRY("ThreadTestP13", 4, 0, 3)};

struct en_de_queue_thd_info s_dequeue_info[] = {
	/* name, 	enq_deq_nmbr, rel_prio, _context_id */
	EN_DE_QUEUE_WORKER_ENTRY("ThreadTestP15", 20, 2, 4),
};

static mutex_hdl_t s_alloc_mtx;
static sem_hdl_t s_dequeue_sem;
static unsigned int s_enqueue_number = 0;
static unsigned int s_alloc_count = 0;


/* *******************************************************************
 * Static Function Prototypes
 * ******************************************************************/
/*Test cases - queue */
static void test_lib_list__init(void);
static void test_get_parent_structure(void);
static void test_lib_list__enqueue(void);
static void test_lib_list__dequeue(void);
static void test_lib_list__thread_en_de_queue_start(void);
static void test_lib_list__wait(void);
static void test_lib_list__thread_en_de_queue_stop(void);
static void test_set_mem_memory_management(void);

/*Test cases - queue memory management */
static void test_lib_list__init(void);
static void test_lib_list__mem_buffer_size(void);
static void test_lib_list__mem_alloc_free(void);
static void test_lib_list__mem_cleanup(void);

/*Helper Routines */
static struct test_queue_data *alloc_test_node(unsigned int _alloc_select, uint32_t _context_id);
static void free_test_node(unsigned int _alloc_select, uint32_t _context_id, struct test_queue_data *_node);
static void *icb_fifo_enqueue_worker(void *_arg);
static void *icb_fifo_dequeue_worker(void *_arg);
static void helper_print_list_content(struct queue_attr *_queue, void *_base);
static struct test_list_data *helper_find_list_entry(struct queue_attr *_queue, char const *_data_entry, void *_base);

/* *******************************************************************
 * Static Functions
 * ******************************************************************/

static void test_lib_list__chain_list_init(void)
{
	int ret;
	void *mem;
	size_t mem_size;

	ret = lib_list__mem_calc_size(&test_chain_mem, sizeof(struct test_list_data), M_TEST_ENTRY_COUNT_CHAIN_LIST);
	if (ret < EOK)
	{
		TEST_FAIL("lib_list__mem_calc_size failed\n");
	}

	mem_size = ret;
	mem = (void*)alloc_memory(1, mem_size);

	s_test_chain_base = (void*)(0xffff0000 & (unsigned int)mem);
	TEST_ASSERT_NOT_NULL(mem);

	ret = lib_list__mem_setup(&test_chain_mem, MEM_SETUP_MODE_master, mem, mem_size);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	ret = lib_list__init(&test_chain_list, s_test_chain_base);
	TEST_ASSERT_EQUAL_INT(EOK, ret);
}

static void test_lib_list__chain_list_add_data(void)
{
	int i, ret;
	struct test_list_data *enq_data;

	for (i = 0; i < 3; i++)
	{
		enq_data = (struct test_list_data *)lib_list__mem_alloc(&test_chain_mem, 1, 0, &ret);
		TEST_ASSERT_NOT_NULL(enq_data);

		sprintf(&enq_data->data[0], "data_%i\n", i);
		ret = lib_list__enqueue(&test_chain_list, &enq_data->node, 0, s_test_chain_base);
		TEST_ASSERT_EQUAL_INT(EOK, ret);
	}
}

static void test_lib_list__get_first(void)
{
	int ret;
	struct list_node *first_node;
	struct test_list_data *first;

	ret = lib_list__get_begin(&test_chain_list, &first_node, 0, s_test_chain_base);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	first = (struct test_list_data*)GET_CONTAINER_OF(first_node, struct test_list_data, node);
	TEST_ASSERT_NOT_NULL(first);

	TEST_INFO("%s Content: %s\n", __func__, first->data);
}

static void test_lib_list__get_all(void)
{
	int ret, cycle_count = 0;
	struct list_node *list_node;
	struct test_list_data *list_data;

	TEST_INFO("%s() called\n", __func__);

	ret = lib_list__get_begin(&test_chain_list, &list_node, 0, s_test_chain_base);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	list_data = (struct test_list_data *)GET_CONTAINER_OF(list_node, struct test_list_data, node);
	TEST_ASSERT_NOT_NULL(list_data);
	TEST_INFO("%s Content: %s\n", __func__, list_data->data);

	while (ret = lib_list__get_next(&test_chain_list, &list_node, 0, s_test_chain_base), ((ret == LIB_LIST__EOK) || (ret == -LIB_LIST__LIST_OVERFLOW)))
	{
		if (ret == -LIB_LIST__LIST_OVERFLOW)
		{
			cycle_count++;
		}

		if (cycle_count > 2)
			break;

		TEST_ASSERT_NOT_NULL(list_node);
		list_data = (struct test_list_data *)GET_CONTAINER_OF(list_node, struct test_list_data, node);
		TEST_ASSERT_NOT_NULL(list_data);
		TEST_INFO("%s Content: %s\n", __func__, list_data->data);
	}
}

static void test_lib_list__iterator(void)
{
	int ret;
	struct list_node *itr_node, *start, *end;
	struct test_list_data *list_data;
	TEST_INFO("%s() called\n", __func__);

	for (itr_node = ITR_BEGIN(&test_chain_list, 0, s_test_chain_base); itr_node != ITR_END(&test_chain_list, 0, s_test_chain_base); ITR_NEXT(&test_chain_list, &itr_node, 0, s_test_chain_base))
	{
		list_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
		TEST_INFO("%s Content: %s\n", __func__, list_data->data);
	}
	list_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
	TEST_INFO("%s Content: %s\n", __func__, list_data->data);

	TEST_INFO("-------------------------\n");

	start = ITR_BEGIN(&test_chain_list, 0, s_test_chain_base);
	ITR_NEXT(&test_chain_list, &start, 0, s_test_chain_base);
	end = ITR_END(&test_chain_list, 0, s_test_chain_base);
	ITR_NEXT(&test_chain_list, &end, 0, s_test_chain_base);

	for (itr_node = start; (itr_node != end); ITR_NEXT(&test_chain_list, &itr_node, 0, s_test_chain_base))
	{
		list_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
		TEST_INFO("%s Content: %s\n", __func__, list_data->data);
	}
	list_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
	TEST_INFO("%s Content: %s\n", __func__, list_data->data);
	//lib_list__delete(&test_chain_list, itr_node, 0 , s_test_chain_base);

	TEST_INFO("-------------------------\n");

	for (itr_node = ITR_BEGIN(&test_chain_list, 0, s_test_chain_base); itr_node != ITR_END(&test_chain_list, 0, s_test_chain_base); ITR_NEXT(&test_chain_list, &itr_node, 0, s_test_chain_base))
	{
		list_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
		TEST_INFO("%s Content: %s\n", __func__, list_data->data);
	}
	list_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
	TEST_INFO("%s Content: %s\n", __func__, list_data->data);
}

static void test_lib_list__advanced_add_delete(void)
{
	struct list_node *start, *end, *itr_node;
	struct test_list_data *list_data, *enq_data;
	int ret;

	TEST_INFO("%s()\n", __func__);
	helper_print_list_content(&test_chain_list, s_test_chain_base);

	list_data = helper_find_list_entry(&test_chain_list, "data_2\n", s_test_chain_base);
	TEST_ASSERT_NOT_NULL(list_data);

	enq_data = (struct test_list_data *)lib_list__mem_alloc(&test_chain_mem, 1, 0, &ret);
	TEST_ASSERT_NOT_NULL(enq_data);
	sprintf(&(enq_data->data)[0], "test_new_1\n");
	ret = lib_list__add_after(&test_chain_list, &list_data->node, &enq_data->node, 0, s_test_chain_base);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	helper_print_list_content(&test_chain_list, s_test_chain_base);

	start = ITR_BEGIN(&test_chain_list, 0, s_test_chain_base);
	enq_data = (struct test_list_data *)lib_list__mem_alloc(&test_chain_mem, 1, 0, &ret);
	TEST_ASSERT_NOT_NULL(enq_data);
	sprintf(&(enq_data->data)[0], "test_new_2\n");
	ret = lib_list__add_after(&test_chain_list, start, &enq_data->node, 0, s_test_chain_base);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	start = ITR_BEGIN(&test_chain_list, 0, s_test_chain_base);
	enq_data = (struct test_list_data *)lib_list__mem_alloc(&test_chain_mem, 1, 0, &ret);
	TEST_ASSERT_NOT_NULL(enq_data);
	sprintf(&(enq_data->data)[0], "test_new_3\n");
	ret = lib_list__add_before(&test_chain_list, start, &enq_data->node, 0, s_test_chain_base);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	helper_print_list_content(&test_chain_list, s_test_chain_base);

	start = ITR_BEGIN(&test_chain_list, 0, s_test_chain_base);
	end = ITR_END(&test_chain_list, 0, s_test_chain_base);

	for (itr_node = start; itr_node != end; ITR_NEXT(&test_chain_list, &itr_node, 0, s_test_chain_base))
	{
		list_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
		ret = lib_list__contains(&test_chain_list, itr_node, 0, s_test_chain_base);
		TEST_ASSERT_EQUAL_INT(1, ret);
		ret = lib_list__delete(&test_chain_list, itr_node, 0, s_test_chain_base);
		TEST_ASSERT_EQUAL_INT(EOK, ret);
		ret = lib_list__mem_free(&test_chain_mem, &list_data->node, 0);
		TEST_ASSERT_EQUAL_INT(EOK, ret);
	}
	list_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
	ret = lib_list__contains(&test_chain_list, itr_node, 0, s_test_chain_base);
	TEST_ASSERT_EQUAL_INT(1, ret);
	ret = lib_list__delete(&test_chain_list, itr_node, 0, s_test_chain_base);
	TEST_ASSERT_EQUAL_INT(EOK, ret);
	ret = lib_list__mem_free(&test_chain_mem, &list_data->node, 0);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	ret = lib_list__get_begin(&test_chain_list, &itr_node, 0, s_test_chain_base);
	TEST_ASSERT_EQUAL_INT(-LIB_LIST__ESTD_AGAIN, ret);
}

static void test_lib_list__init(void)
{
	int ret;
#ifdef __gnu_linux__ /* set round robin scheduling on Linux, as this is not the default here */
	int policy;
	int ret_val;
	struct sched_param param_par;

	pthread_getschedparam(pthread_self(), &policy, &param_par);
	policy = SCHED_RR;
	param_par.sched_priority = 50;
	ret_val = pthread_setschedparam(pthread_self(), policy, &param_par);
	if (ret_val != 0)
	{
		TEST_INFO("pthread_setschedparam");
		TEST_INFO("If this binary was launched directly, run it as superuser (sudo ...).\n");
		TEST_INFO("If this binary was launched via eclipse, run eclipse as superuser (sudo eclipse).\n");
	}
#endif /* __gnu_linux__ */
	s_enqueue_number = 0;
	s_alloc_count = 0;

	ret = lib_thread__mutex_init(&s_alloc_mtx);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	ret = lib_thread__sem_init(&s_dequeue_sem, 0);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	lib_list__init(&test_list, 0);
}

static void test_lib_list__mem_init(void)
{
	int ret;
	void *buf_ptr;
	int req_buffer_size;

	req_buffer_size = lib_list__mem_calc_size(&test_mem, sizeof(struct test_queue_data), M_TEST_ENTRY_COUNT);
	if (req_buffer_size < EOK)
	{
		TEST_FAIL("lib_queue__mem_buffer_size failed \n");
	}

	buf_ptr = (void*)alloc_memory(1, (size_t)req_buffer_size);
	TEST_ASSERT_NOT_NULL(buf_ptr);

	ret = lib_list__mem_setup(&test_mem, MEM_SETUP_MODE_master, buf_ptr, (size_t)req_buffer_size);
	TEST_ASSERT_EQUAL_INT(EOK, ret);
}

static void test_lib_list__mem_cleanup(void)
{
	int ret;
	void *buf_ptr;
	size_t buf_size;

	ret = lib_list__mem_cleanup(&test_mem, MEM_SETUP_MODE_master, &buf_ptr, &buf_size);
	TEST_ASSERT_EQUAL_INT(EOK, ret);
	TEST_ASSERT_NOT_NULL(buf_ptr);

	free(buf_ptr);
}

static void test_lib_list__mem_alloc_free(void)
{
	unsigned int count;
	int ret;
	struct test_queue_data *test_data_1;
	struct test_queue_data *test_data_2;
	struct test_queue_data *test_data_3;

	test_data_1 = (struct test_queue_data *)lib_list__mem_alloc(&test_mem, 20, 1, &ret);
	TEST_ASSERT_NOT_NULL(test_data_1);
	for (count = 0; count < 20; count++)
	{
		test_data_1[count].data = count;
	}

	test_data_2 = (struct test_queue_data *)lib_list__mem_alloc(&test_mem, 20, 1, &ret);
	TEST_ASSERT_NOT_NULL(test_data_2);
	for (count = 0; count < 20; count++)
	{
		test_data_2[count].data = count;
	}

	test_data_3 = (struct test_queue_data *)lib_list__mem_alloc(&test_mem, 10, 1, &ret);
	TEST_ASSERT_NOT_NULL(test_data_3);
	for (count = 0; count < 10; count++)
	{
		test_data_3[count].data = count;
	}

	//	test_data_4 = (struct test_queue_data *)lib_list__mem_alloc(&test_mem,10,1,&ret);
	//	TEST_ASSERT_NULL(test_data_4);
	//	for(count=0; count < 20; count++) {
	//		test_data_4[count].data = count;
	//	}

	ret = lib_list__mem_free(&test_mem, (void *)test_data_2, 1);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	ret = lib_list__mem_free(&test_mem, (void *)test_data_3, 1);
	TEST_ASSERT_EQUAL_INT(EOK, ret);
}

static void test_get_parent_structure(void)
{
	struct test_queue_data *new_entry;
	struct test_queue_data *new_test_entry;
	struct list_node *new_entry_list;

	new_entry = alloc_test_node(0, 1);
	TEST_ASSERT_NOT_NULL(new_entry);

	new_entry_list = &(new_entry->node);
	new_test_entry = (struct test_queue_data *)GET_TEST_QUEUE_DATA(new_entry_list);

	lib_list__enqueue(&test_list, new_entry_list, 0, 0);
}

static void test_lib_list__enqueue(void)
{
	int count;
	struct test_queue_data *new_entry;

	for (count = 0; count < 5; count++)
	{
		new_entry = alloc_test_node(0, 1);
		lib_list__enqueue(&test_list, &new_entry->node, 0, 0);
	}
}

static void test_lib_list__dequeue(void)
{
	int ret, count;
	struct test_queue_data *new_entry;
	struct list_node *list_node;
	struct test_queue_data *dequeue_node;
	unsigned int dequeue_value_last = 0;

	for (count = 0; count < 10; count++)
	{
		ret = lib_list__dequeue(&test_list, &list_node, 0, 0);
		if (ret < EOK)
		{
			if (ret == -LIB_LIST__ESTD_AGAIN)
			{
				TEST_INFO("%s Queue empty\n", __func__);
				break;
			}
			else
			{
				TEST_INFO("%s Error %i\n", __func__, ret);
				break;
			}
		}

		if ((count % 5) == 0)
		{
			new_entry = alloc_test_node(0, 1);
			lib_list__enqueue(&test_list, &new_entry->node, 0, 0);
		}

		dequeue_node = (struct test_queue_data *)GET_TEST_QUEUE_DATA(list_node);
		TEST_ASSERT_EQUAL_INT((int)dequeue_value_last + 1, (int)dequeue_node->data);
		dequeue_value_last = dequeue_node->data;

		TEST_INFO("LIST Value %u\n", dequeue_node->data);
	}
}

static void test_lib_list__thread_en_de_queue_start(void)
{
	int ret = EOK;
	unsigned int count;

	s_enqueue_number = 0;

	for (count = 0; count < sizeof(s_dequeue_info) / sizeof(*s_dequeue_info); count++)
	{
		ret = lib_thread__create(&s_dequeue_info[count].hdl, &icb_fifo_dequeue_worker, &s_dequeue_info[count], s_dequeue_info[count].rel_prio, s_dequeue_info[count].name);
		TEST_ASSERT_EQUAL_INT(EOK, ret);
	}

	for (count = 0; count < sizeof(s_enqueue_info) / sizeof(*s_enqueue_info); count++)
	{
		ret = lib_thread__create(&s_enqueue_info[count].hdl, &icb_fifo_enqueue_worker, &s_enqueue_info[count], s_enqueue_info[count].rel_prio, s_enqueue_info[count].name);
		TEST_ASSERT_EQUAL_INT(EOK, ret);
	}
}

static void test_lib_list__wait(void)
{
	lib_thread__msleep(TEST_INTERVAL);
}

static void test_lib_list__thread_en_de_queue_stop(void)
{
	unsigned int count;
	int ret;

	for (count = 0; count < sizeof(s_enqueue_info) / sizeof(*s_enqueue_info); count++)
	{
		s_enqueue_info[count].running = 0;
		ret = lib_thread__join(&s_enqueue_info[count].hdl, NULL);
		TEST_ASSERT_EQUAL_INT(EOK, ret);
	}

	TEST_INFO("called_1\n");
	for (count = 0; count < sizeof(s_dequeue_info) / sizeof(*s_dequeue_info); count++)
	{
		s_dequeue_info[count].running = 0;
		lib_thread__sem_post(s_dequeue_sem);
		ret = lib_thread__join(&s_dequeue_info[count].hdl, NULL);
		TEST_INFO("called_2\n");
		TEST_ASSERT_EQUAL_INT(EOK, ret);
	}
	TEST_INFO("called_3\n");
}

static void test_set_mem_memory_management(void)
{
	int count;

	for (count = 0; count < sizeof(s_enqueue_info) / sizeof(*s_enqueue_info); count++)
	{
		s_enqueue_info[count].alloc_function_selector = 1;
	}

	for (count = 0; count < sizeof(s_dequeue_info) / sizeof(*s_dequeue_info); count++)
	{
		s_dequeue_info[count].alloc_function_selector = 1;
	}
	s_enqueue_number = 0;
}

static void test_lib_list__mem_buffer_size(void)
{
	mem_hdl_t mem_hdl;

	size_t req_buffer_size;
	req_buffer_size = lib_list__mem_calc_size(&mem_hdl, sizeof(struct test_queue_data) - 1, 1);

	TEST_INFO("Calculated buffer size %u\n", req_buffer_size);
}

static int signal_waiter__sort_insert(struct queue_attr *_queue, struct test_list_data *_insert_node)
{
	struct list_node *start, *end, *itr_node;
	struct test_list_data *itr_data;
	int ret;

	if ((_queue == NULL) || (_insert_node == NULL))
	{
		return -EPAR_NULL;
	}

	start = ITR_BEGIN(_queue, 0, NULL);
	end = ITR_END(_queue, 0, NULL);

	/* First entry*/
	if (start == NULL)
	{
		lib_list__add_after(_queue, &_queue->head, &_insert_node->node, 0, NULL);
		return EOK;
	}

	/* Second entry*/
	if (start == end)
	{
		itr_data = (struct test_list_data *)GET_CONTAINER_OF(start, struct test_list_data, node);
		if (itr_data->prio < _insert_node->prio)
		{
			lib_list__add_before(_queue, &itr_data->node, &_insert_node->node, 0, NULL);
		}
		else
		{
			lib_list__add_after(_queue, &itr_data->node, &_insert_node->node, 0, NULL);
		}
		return EOK;
	}

	for (itr_node = start; itr_node != end; ITR_NEXT(_queue, &itr_node, 0, NULL))
	{
		itr_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
		if (itr_data->prio < _insert_node->prio)
		{
			lib_list__add_before(_queue, &itr_data->node, &_insert_node->node, 0, NULL);
			return EOK;
		}
	}

	/*last entry*/
	itr_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
	if (itr_data->prio < _insert_node->prio)
	{
		lib_list__add_before(_queue, &itr_data->node, &_insert_node->node, 0, NULL);
	}
	else
	{
		lib_list__add_after(_queue, &itr_data->node, &_insert_node->node, 0, NULL);
	}
	return EOK;
}

static struct test_list_data *signal_waiter__get_and_remove(struct queue_attr *_queue)
{
	int ret;
	struct list_node *itr_node;
	static struct test_list_data *itr_node_data;

	ret = lib_list__get_begin(_queue, &itr_node, 0, NULL);
	if (ret < EOK)
	{
		return NULL;
	}

	itr_node_data = (struct test_list_data *)GET_CONTAINER_OF(itr_node, struct test_list_data, node);
	lib_list__delete(_queue, &itr_node_data->node, 0, NULL);
	return itr_node_data;
}

static void test_lib_list__insert(void)
{
	int ret;
	struct queue_attr local_test_chain;
	struct test_list_data *entry;
	struct test_list_data *two_entry;
	struct test_list_data *three_entry;
	struct test_list_data *four_entry;

	struct test_list_data *deq;

	ret = lib_list__init(&local_test_chain, NULL);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	entry = (struct test_list_data *)calloc(1, sizeof(struct test_list_data));
	memcpy(&entry->data[0], "prio_3", 6);
	entry->prio = 3;
	//lib_list__add_after(&local_test_chain, &(local_test_chain.head), &entry->node,0, NULL);
	signal_waiter__sort_insert(&local_test_chain, entry);

	two_entry = (struct test_list_data *)calloc(1, sizeof(struct test_list_data));
	memcpy(&two_entry->data[0], "prio_4", 6);
	two_entry->prio = 4;
	signal_waiter__sort_insert(&local_test_chain, two_entry);
	//lib_list__add_after(&local_test_chain, &(entry->node), &two_entry->node,0, NULL);

	three_entry = (struct test_list_data *)calloc(1, sizeof(struct test_list_data));
	memcpy(&three_entry->data[0], "prio_7", 6);
	three_entry->prio = 7;
	signal_waiter__sort_insert(&local_test_chain, three_entry);

	helper_print_list_content(&local_test_chain, NULL);

	four_entry = (struct test_list_data *)calloc(1, sizeof(struct test_list_data));
	memcpy(&four_entry->data[0], "prio_8", 6);
	four_entry->prio = 8;
	signal_waiter__sort_insert(&local_test_chain, four_entry);

	three_entry = (struct test_list_data *)calloc(1, sizeof(struct test_list_data));
	memcpy(&three_entry->data[0], "prio_1", 6);
	three_entry->prio = 1;
	signal_waiter__sort_insert(&local_test_chain, three_entry);

	helper_print_list_content(&local_test_chain, NULL);

	do
	{
		deq = signal_waiter__get_and_remove(&local_test_chain);
		if (deq != NULL)
		{
			TEST_INFO("Deq element %s\n", deq->data);
		}
	} while (deq != NULL);
}

static void setUp(void)
{
}

static void tearDown(void)
{
}

/* *******************************************************************
 * Global Functions
 * ******************************************************************/
static TestRef lib_list_test(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures){
		// TEST: test cases

		new_TestFixture("test_lib_list__chain_list_init", test_lib_list__chain_list_init),
		new_TestFixture("test_lib_list__chain_list_add_data", test_lib_list__chain_list_add_data),
		new_TestFixture("test_lib_list__get_first",test_lib_list__get_first),
		new_TestFixture("test_lib_list__get_all",test_lib_list__get_all),
		new_TestFixture("test_lib_list__iterator",test_lib_list__iterator),
		new_TestFixture("test_lib_list__delete",test_lib_list__advanced_add_delete),
		new_TestFixture("test_lib_list__init", test_lib_list__init),
		new_TestFixture("test_lib_list__mem_init", test_lib_list__mem_init),
		new_TestFixture("test_lib_list__mem_alloc_free", test_lib_list__mem_alloc_free),
		new_TestFixture("test_get_parent_structure",	test_get_parent_structure),
		new_TestFixture("test_lib_list__mem_buffer_size", test_lib_list__mem_buffer_size),
		new_TestFixture("test_lib_list__enqueue", test_lib_list__enqueue),
		new_TestFixture("test_lib_list__dequeue", test_lib_list__dequeue),
		new_TestFixture("test_lib_list__thread_en_de_queue_start", test_lib_list__thread_en_de_queue_start),
		new_TestFixture("test_lib_list__wait", test_lib_list__wait),
		new_TestFixture("test_lib_list__thread_en_de_queue_stop", test_lib_list__thread_en_de_queue_stop),
		new_TestFixture("test_set_mem_memory_management", test_set_mem_memory_management),
		new_TestFixture("test_lib_list__thread_en_de_queue_start", test_lib_list__thread_en_de_queue_start),
		new_TestFixture("test_lib_list__wait", test_lib_list__wait),
		new_TestFixture("test_lib_list__thread_en_de_queue_stop", test_lib_list__thread_en_de_queue_stop),
		new_TestFixture("test_lib_list__mem_cleanup", test_lib_list__mem_cleanup),
		new_TestFixture("test_lib_list__insert", test_lib_list__insert),

	};
	EMB_UNIT_TESTCALLER(lib_queue__test, "lib_queue__test", setUp, tearDown, fixtures);

	return (TestRef)&lib_queue__test;
}

TEST_CASE_INIT(lib_list, test_lib_list__start , &lib_list_test);


/* *******************************************************************
 * Static Helper Functions Prototypes
 * ******************************************************************/
static struct test_queue_data *alloc_test_node(unsigned int _alloc_select, uint32_t _context_id)
{
	int ret;
	int redo_count = 0;
	struct test_queue_data *entry;

	switch (_alloc_select)
	{
	case 0:
		entry = (struct test_queue_data *)calloc(1, sizeof(struct test_queue_data));
		if (entry == NULL)
			return NULL;
		lib_thread__mutex_lock(s_alloc_mtx);
		s_alloc_count++;
		entry->data = s_alloc_count;
		lib_thread__mutex_unlock(s_alloc_mtx);
		return entry;

	case 1:
		do
		{
			entry = (struct test_queue_data *)lib_list__mem_alloc(&test_mem, 1, _context_id, &ret);
			if (entry == NULL)
			{
				TEST_INFO("ERROR MEM_ALLOC notify %i\n", ret);
				lib_thread__msleep(200);
			}
			redo_count++;
		} while ((entry == NULL) && (redo_count < 4));

		if (entry == NULL)
		{
			TEST_INFO("ERROR MEM_ALLOC error %i\n", ret);
			return NULL;
		}

		lib_thread__mutex_lock(s_alloc_mtx);
		s_alloc_count++;
		entry->data = s_alloc_count;
		lib_thread__mutex_unlock(s_alloc_mtx);
		return entry;
	}
	return NULL;

}

static void free_test_node(unsigned int _alloc_select, uint32_t _context_id, struct test_queue_data *_node)
{
	int ret;

	switch (_alloc_select)
	{
	case 0:
		free_memory((void*)_node);
		return;
	case 1:
		ret = lib_list__mem_free(&test_mem, _node, _context_id);
		if (ret < EOK)
		{
			TEST_INFO("ERROR MEM_FREE error %i\n", ret);
		}
	}
}

static void *icb_fifo_enqueue_worker(void *_arg)
{
#ifdef CONFIG_ICB_FIFO__LOCK_TYPE_ARCHLOCK
	uint32_t msr_state;
#endif
	int count;
	struct test_queue_data *new_entry;
	unsigned int enqueue_number, trigger_dequeue = 0;
	struct en_de_queue_thd_info *info = (struct en_de_queue_thd_info *)_arg;

	info->running = 1;
	TEST_INFO("%s starts\n", info->name);

	while (info->running)
	{
		for (count = 0; count < info->enq_deq_nmbr; count++)
		{

#ifdef CONFIG_ICB_FIFO__LOCK_TYPE_ARCHLOCK
			msr_state = lib_arch__isr_lock();
			s_enqueue_number++;
			enqueue_number = s_enqueue_number;
			lib_arch__isr_unlock(msr_state);
#else
			enqueue_number = __sync_fetch_and_add(&s_enqueue_number, 1);
#endif
			do
			{
				new_entry = alloc_test_node(info->alloc_function_selector, info->context_id);
				if (new_entry == NULL)
				{
					lib_thread__sem_post(s_dequeue_sem);
				}
			} while (new_entry == NULL);
			new_entry->data = enqueue_number;
			new_entry->thd_name = info->name;
			lib_list__enqueue(&test_list, &new_entry->node, info->context_id, 0);
			if ((enqueue_number > 0) && (enqueue_number % 20 == 0))
			{
				trigger_dequeue = 1;
			}
		}
		if (trigger_dequeue)
		{
			trigger_dequeue = 0;
			lib_thread__sem_post(s_dequeue_sem);
		}
		lib_thread__msleep(4);
	}

	TEST_INFO("%s stops\n", info->name);
	return NULL;
}

static void *icb_fifo_dequeue_worker(void *_arg)
{
	int dequeue_count;
	int ret, count;
	struct list_node *list_node;
	struct test_queue_data *dequeue_node;
	struct en_de_queue_thd_info *info = (struct en_de_queue_thd_info *)_arg;

	info->running = 1;
	TEST_INFO("%s starts\n", info->name);

	while (info->running)
	{
		ret = lib_thread__sem_wait(s_dequeue_sem);
		if (ret != EOK)
		{
			info->running = 0;
			TEST_INFO("%s failed\n", info->name);
			return NULL;
		}
		for (count = 0; count < info->enq_deq_nmbr; count++)
		{
			ret = lib_list__dequeue(&test_list, &list_node, info->context_id, 0);
			if (ret < EOK)
			{
				if (ret == -LIB_LIST__ESTD_AGAIN)
				{
					TEST_INFO("%s Queue empty\n", info->name);
					TEST_INFO("Dequeue CNT %u\n", dequeue_count);
					break;
				}
				else
				{
					TEST_INFO("%s Error %i\n", info->name, ret);
				}
			}

			dequeue_node = (struct test_queue_data *)GET_TEST_QUEUE_DATA(list_node);
			dequeue_count = dequeue_node->data;

			//TEST_INFO("CNT %u - THD %s \n",dequeue_node->data, dequeue_node->thd_name);
			free_test_node(info->alloc_function_selector, info->context_id, dequeue_node);
		}
	}

	while (ret != -LIB_LIST__ESTD_AGAIN)
	{
		ret = lib_list__dequeue(&test_list, &list_node, info->context_id, 0);
		if (ret < EOK)
		{
			if (ret == -LIB_LIST__ESTD_AGAIN)
			{
				TEST_INFO("%s Trail run Queue empty\n", info->name);
				TEST_INFO("Trail run Dequeue CNT %u\n", dequeue_count);
				break;
			}
			else
			{
				TEST_INFO("%s Trail run Error %i\n", info->name, ret);
			}
		}

		dequeue_node = (struct test_queue_data *)GET_TEST_QUEUE_DATA(list_node);
		dequeue_count = dequeue_node->data;

		//TEST_INFO("CNT %u - THD %s \n",dequeue_node->data, dequeue_node->thd_name);
		free_test_node(info->alloc_function_selector, info->context_id, dequeue_node);
	}

	TEST_INFO("%s stops\n", info->name);
	return NULL;
}

static void helper_print_list_content(struct queue_attr *_queue, void *_base)
{
	int ret;
	struct test_list_data *deq_data;
	struct list_node *deq_node;

	ret = lib_list__get_begin(_queue, &deq_node, 0, _base);
	TEST_ASSERT_EQUAL_INT(EOK, ret);
	deq_data = (struct test_list_data *)GET_CONTAINER_OF(deq_node, struct test_list_data, node);
	TEST_INFO("First Node: %s\n", deq_data->data);
	while (ret = lib_list__get_next(_queue, &deq_node, 0, _base), (ret == LIB_LIST__EOK))
	{
		deq_data = (struct test_list_data *)GET_CONTAINER_OF(deq_node, struct test_list_data, node);
		TEST_ASSERT_NOT_NULL(deq_data);
		TEST_INFO("%s\n", deq_data->data);
	}
}

static struct test_list_data *helper_find_list_entry(struct queue_attr *_queue, char const *_data_entry, void *_base)
{
	int ret;
	struct test_list_data *deq_data;
	struct list_node *deq_node;

	ret = lib_list__get_begin(_queue, &deq_node, 0, _base);
	if (ret != LIB_LIST__EOK)
	{
		return NULL;
	}

	deq_data = (struct test_list_data *)GET_CONTAINER_OF(deq_node, struct test_list_data, node);
	if (!strcmp(_data_entry, &deq_data->data[0]))
	{
		return deq_data;
	}

	while (ret = lib_list__get_next(_queue, &deq_node, 0, _base), (ret == LIB_LIST__EOK))
	{
		deq_data = (struct test_list_data *)GET_CONTAINER_OF(deq_node, struct test_list_data, node);
		if (!strcmp(_data_entry, &deq_data->data[0]))
		{
			return deq_data;
		}
	}
	return NULL;
}
