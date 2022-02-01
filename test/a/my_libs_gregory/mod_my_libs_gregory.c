

static void foo() {}




#include "httpd.h"




#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"


//extern "C" {
/* The sample content handler */
static int my_libs_gregory_handler(request_rec *r)
{
    if (strcmp(r->handler, "my_libs_gregory")) {
        return DECLINED;
    };char buf[            1000] = {}; //ap_rputs(buf, getcwd());

    system("echo foo > /tmp/gr");
    int file=open("/tmp/file",O_APPEND|O_CREAT|O_WRONLY
         , 0642);//  );//           , O_WRONLY);
            ap_rprintf(r, "%s %d\n", strerror(errno), file); //"Hello",r);
    write(file, "Hello", 5);
    r->content_type = "text/html";      

    if (!r->header_only)   {
//    	ap_rputs(getwd(), r);
        ap_rputs("Theam\n", r);
        ap_rputs("Hello",r);
        puts("g");

     while (1==2) {
        FILE* f = fopen("/tmp/fifo", "w");

        if (!f) continue;

        while (fwrite("D", 1, 1, f) != 1) {
        	//continue;
        }

        fclose(f);
        break;

        }
        }
        
    return OK;
}

static void my_libs_gregory_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(my_libs_gregory_handler, NULL, NULL, APR_HOOK_MIDDLE);
}
//};

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA my_libs_gregory_module = {
    STANDARD20_MODULE_STUFF, 
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    my_libs_gregory_register_hooks  /* register hooks                      */
};