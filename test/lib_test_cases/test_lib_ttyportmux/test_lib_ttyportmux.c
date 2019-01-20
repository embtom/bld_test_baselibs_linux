/* ****************************************************************************************************
 * test_lib_list.c within the following project: lib_thread
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/


/* *******************************************************************
 * includes
 * ******************************************************************/

/*c-runime */

/*frame*/
#include <lib_convention__errno.h>
#include <lib_log.h>

/*project*/
#include <embUnit.h>
#include <test_cases.h>
#include <test_lib_ttyportmux.h>
#include <lib_ttyportmux.h>

/* *******************************************************************
 * Defines
 * ******************************************************************/
#define M_TEST_MODULE_NAME 		"test_lib_ttyportmux"

/* *******************************************************************
 * Global Functions
 * ******************************************************************/

static void setUp(void)
{
}

static void tearDown(void)
{
}

static void test_lib_ttyportmux_get_devices(void)
{
	char test_buffer[] = "test_message\n";
	char test_inBuffer[20] = {0};

	int ret;
	ret = lib_ttyportmux__ttydevice_count();

	struct ttyDeviceInfo* deviceInfo;
	struct list_node *itr_node;
	struct list_node *first_node = lib_ttyportmux__get_ttydevice_listentry();

	lib__ttyportmux_listentry_foreach(itr_node, first_node)
	{
		deviceInfo = lib_ttyportmux__get_ttydevice_info(itr_node);
		TEST_INFO("name %s\n",deviceInfo->deviceName);
	}

	TEST_INFO("Number of %u\n", ret);
}

static void test_lib_ttyportmux_get_streams()
{
	int ret;
	int streamsCount= lib_ttyportmux__get_stream_count();
	struct ttyStreamMap mapping[8];
	ret = lib_ttyportmux__get_stream_mapping(&mapping[0], sizeof(mapping));
	mapping[TTYSTREAM_error].deviceType	= TTYDEVICE_syslog;
	ret = lib_ttyportmux__set_stream_mapping(&mapping[0], sizeof(mapping));

}

static TestRef lib_ttyportmux_test(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures){
		// TEST: test cases
		new_TestFixture("test_lib_ttyportmux_get_devices", test_lib_ttyportmux_get_devices),
		new_TestFixture("test_lib_ttyportmux_get_streams", test_lib_ttyportmux_get_streams)
	};
	EMB_UNIT_TESTCALLER(lib_ttyportmux__test, "lib_ttyportmux__test", setUp, tearDown, fixtures);

	return (TestRef)&lib_ttyportmux__test;
}

TEST_CASE_INIT(lib_ttyportmux, test_lib_ttyportmux__start , &lib_ttyportmux_test);


