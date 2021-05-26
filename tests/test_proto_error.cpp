#include "testutil.hpp"
#include "testutil_unity.hpp"
#include <iostream>
#include <string.h>

SETUP_TEARDOWN_TESTCONTEXT
char end[] = "tcp://127.0.0.1:55667";

int sock_type;

void test_pubreq ()
{
   

    std::cout << "1" << std::endl;
    void *sub = test_context_socket (ZMQ_SUB);
    TEST_ASSERT_SUCCESS_ERRNO (zmq_setsockopt (sub, ZMQ_SUBSCRIBE, "", 0));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (sub, end));

    std::cout << "2" << std::endl;
    void *pub1 = test_context_socket (sock_type);
    TEST_ASSERT_SUCCESS_ERRNO (zmq_bind (pub1, end));
    msleep(1000);
    TEST_ASSERT_SUCCESS_ERRNO (zmq_unbind (pub1, end));
    test_context_socket_close (pub1);

    std::cout << "3 - wait to make sure port is free" << std::endl;
    msleep(5000);
   
    // call connect again - will be a noop if the endpoint its still connected or reconnecting. 
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (sub, end));

    std::cout << "4" << std::endl;
    void *pub2 = test_context_socket (ZMQ_PUB);
    TEST_ASSERT_SUCCESS_ERRNO (zmq_bind (pub2, end));

    msleep(100);

    std::cout << "5" << std::endl;
    send_string_expect_success (pub2, "Hello", 0);
    std::cout << "6" << std::endl;
    recv_string_expect_success (sub, "Hello", 0);

    msleep(100);

    std::cout << "7" << std::endl;
    TEST_ASSERT_SUCCESS_ERRNO (zmq_disconnect (sub, end));
    test_context_socket_close (sub);
    TEST_ASSERT_SUCCESS_ERRNO (zmq_unbind (pub2, end));
    test_context_socket_close (pub2);

}

int main (int argc, char *argv[])
{
    if(argc == 2 && strcmp(argv[1],"req")==0) {
      std::cout << "Using ZMQ_REQ" << std::endl;
      sock_type = ZMQ_REQ;
    }else{
      std::cout << "Using ZMQ_PUB" << std::endl;
      sock_type = ZMQ_PUB;
    }
    setup_test_environment ();

    UNITY_BEGIN ();
    RUN_TEST (test_pubreq);
    return UNITY_END (); 
}
