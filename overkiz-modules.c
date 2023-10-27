#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dbus/dbus.h>

const char *const MY_NAME = "com.overkiz.Application.Internal.Modules";
const char *const OBJECT_PATH_NAME = "/com/overkiz/Application/Internal/Modules";
const char *const METHOD_NAME = "getModules";

int main(int argc, char **argv)
{
    DBusError       dbus_error;
    DBusConnection  *conn;
    DBusMessage     *message, *reply;
    int             ret;

    if (argc < 2) {
        fprintf(stderr, "Usuage %s [modules names ...]\n", argv[0]);
        exit(1);
    }

    dbus_error_init(&dbus_error);

    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
    if (!conn) {
        if (dbus_error_is_set(&dbus_error)) {
            fprintf(stderr, "dbus_bus_get DBUS_BUS_SYSTEM failed: %s\n",
                            dbus_error.message);
            dbus_error_free(&dbus_error);
        }

        exit(1);
    }

    ret = dbus_bus_request_name(conn, MY_NAME, DBUS_NAME_FLAG_DO_NOT_QUEUE, &dbus_error);
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        if (dbus_error_is_set(&dbus_error)) {
            fprintf(stderr, "dbus_bus_request_name failed: %s\n",
                            dbus_error.message);
            dbus_error_free(&dbus_error);
        }

        fprintf(stderr, "Failed to request name %s: %d\n", MY_NAME, ret);
        exit(1);
    }

    while (1) {
        if (!dbus_connection_read_write_dispatch(conn, -1)) {
            fprintf(stderr, "DBUS connection failed\n");
            exit(1);
        }
     
        message = dbus_connection_pop_message(conn);
        if (!message) {
            fprintf(stderr, "Failed to get message\n");
            if (dbus_error_is_set(&dbus_error)) {
                fprintf(stderr, "dbus_connection_pop_message failed: %s\n",
                                dbus_error.message);
                dbus_error_free(&dbus_error);
            }
            continue;
        } 

        if (dbus_message_get_type(message) != DBUS_MESSAGE_TYPE_METHOD_CALL)
            continue;


        fprintf(stderr, "Received method call %s %s\n",
                dbus_message_get_interface(message),
                dbus_message_get_member(message));

        reply = dbus_message_new_method_return(message);
        if (!reply) {
            fprintf(stderr, "Error in dbus_message_new_method_return\n");
            exit(1);
        }

        if (!dbus_message_is_method_call(message, MY_NAME, METHOD_NAME)) {
            fprintf(stderr, "Unexpected method or interface %s\n", 
                dbus_message_get_interface(message));

        } else {
            char** a_MODULES = argv + 1;

            dbus_message_append_args (reply,
                          DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &a_MODULES, argc - 1,
                          DBUS_TYPE_INVALID);
        }

        if (!dbus_connection_send(conn, reply, NULL)) {
            fprintf(stderr, "Error in dbus_connection_send\n");
            exit(1);
        }

        dbus_connection_flush(conn);
        dbus_message_unref(reply);
    }

    return 0;
}
