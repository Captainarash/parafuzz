//#include "fuzz_interface.h"
#include <xpc/xpc.h>
#include <xpc/connection.h>
#include <stdio.h>
//#include <Foundation/Foundation.h>


int main(int argc, char *argv[])
{
	//char *keys = {"clientName","messageAction", "AssetType"};
	xpc_object_t msg = xpc_dictionary_create(NULL,NULL,0);
	xpc_dictionary_set_string(msg, "clientName", "\x41\x41\x41\x41");
	xpc_dictionary_set_uint64(msg, "messageAction", 12345678);
	xpc_dictionary_set_string(msg, "AssetType", "\x43\x43\x43\x43");

	char *desc_msg = xpc_copy_description(msg);
	printf("our message: %s\n", desc_msg);


	xpc_connection_t conn = xpc_connection_create_mach_service("com.apple.mobileassetd", NULL, 0);

	xpc_connection_set_event_handler(conn, ^(xpc_object_t event) {
		xpc_type_t t = xpc_get_type(event);
		if (t == XPC_TYPE_ERROR){
			printf("err: %s\n", xpc_dictionary_get_string(event, XPC_ERROR_KEY_DESCRIPTION));
		}
		printf("received an event\n");
	});

	xpc_connection_resume(conn);

	xpc_dictionary_set_connection(msg,"Connection",conn);

	xpc_object_t reply = xpc_connection_send_message_with_reply_sync(conn, msg);
	char *desc = xpc_copy_description(reply);
	printf("reply: %s\n", desc);
//	NSLog(@"reply: %@", reply);
	//com.apple.mobileassetd
	return 0;
}
