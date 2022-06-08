#ifndef MINIMAL_H
#define MINIMAL_H

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <linux/if_link.h>
#include <bpf/bpf.h>
#include <list>
#include "util.h"
#include "xdp_minimal.skel.h"

#define MAX_INTERFACES 20

using namespace std;

class Minimal {
	public:
		Minimal(int flags) 
		{
			if (flags < 0)
				throw std::invalid_argument("tnabr: invalid xdp flags");
			else
				_flags = flags;

			load_bpf();
		}

		Minimal(void)
		{
			load_bpf();
		}

		~Minimal(void) 
		{
			destroy_minimal();
        }

		int load_bpf(void) 
		{
			/* Load and verify BPF application */
			skel = xdp_minimal_bpf__open();
			if (!skel) {

				throw std::runtime_error("Failed to open xdp_minimal skel\n");

				return -1;
			}

			int err = xdp_minimal_bpf__load(skel);
			if (err) {

				throw std::runtime_error("Failed to load and verify BPF skeleton\n");

				destroy_minimal();
				return -2;
			}
			else 
				return 0;
		}

		int destroy_minimal(void) 
		{
			_destroy_minimal();
			return 0;
		}

		int install_xdp_minimal(int ifindex)
		{
			int err = util::install_xdp(skel->progs.xdp_minimal_main_0, ifindex, _flags);

			if (err < 0) {

				throw std::runtime_error("Failed to install xdp_minimal code\n");

				uninstall_xdp_minimal(ifindex);
			}

            interfaces[ifindex] = 1;

			cout << "Installing XDP minimal on interface: " << ifindex << endl;

			return err;
		}

		int uninstall_xdp_minimal(int ifindex) 
		{

			cout << "Uninstalling XDP minimal on interface: " << ifindex << endl;

			util::uninstall_xdp(ifindex, _flags);
            interfaces[ifindex] = 0;

			return 0;
		}

	private:
		int ret;
		struct xdp_minimal_bpf *skel;
		int _ifindex;
		int _flags = XDP_FLAGS_DRV_MODE; /* default */
		int map_fd;
        int interfaces[MAX_INTERFACES] = {0};

		int _destroy_minimal(void)
		{
            for (int i = 0; i < MAX_INTERFACES; i++) {
                if (interfaces[i] == 1)
                    uninstall_xdp_minimal(i);
            }
			/*unordered_map<string, struct tna_bridge>::iterator br_it;
			unordered_map<string, struct tna_interface>::iterator if_it;

			for (br_it = tnabrs.begin(); br_it != tnabrs.end(); ++br_it) {
				for (if_it = br_it->second.brifs.begin(); if_it != br_it->second.brifs.end(); ++if_it) {
					uninstall_xdp_tnabr(if_it->second);
				}
			}*/
			xdp_minimal_bpf__destroy(skel);

			return 0;
		}

};


#endif