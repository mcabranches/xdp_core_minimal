#include "minimal.h"


int stop = 0;


static void unload_prog(int sig) {
    std::cout <<"\nStopping minimal controller ..." <<  std::endl;
    stop = 1;
    return;
}

int main(void)
{
    signal(SIGINT, unload_prog);
    signal(SIGTERM, unload_prog);

    Minimal minimal(XDP_FLAGS_DRV_MODE);
    minimal.install_xdp_minimal(8);

    std::cout <<"Starting Minimal controller ..." <<  std::endl;

    while(!stop) { //controller's main loop
        sleep(1);
    }

    return 0;
}
