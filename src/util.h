#ifndef USER_UTIL_H
#define USER_UTIL_H

#include <bpf/libbpf.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <linux/if_link.h>


namespace util {

    static int uninstall_xdp(int ifindex, int flags)
    {
	    bpf_xdp_attach(ifindex, -1, flags, NULL);
	    return 1;
    }

    static int install_xdp(struct bpf_program *xdp_prog, int ifindex, int xdp_flags)
    {
	    int bpf_prog_fd = bpf_program__fd(xdp_prog);

	    if (bpf_xdp_attach(ifindex, bpf_prog_fd, XDP_FLAGS_DRV_MODE, NULL) < 0) {
			printf("Error linking fd to xdp with offload flags\n");
			return -1;
	    }
	    else {
		    printf("XDP program loaded\n");
	    }

	return 0;
    }
}

#endif
