#include "vmlinux.h"
#include <bpf/bpf_helpers.h>


/* trace is written to /sys/kernel/debug/tracing/trace_pipe */
#define bpf_debug(fmt, ...)                                             \
                ({                                                      \
                        char ____fmt[] = fmt;                           \
                        bpf_trace_printk(____fmt, sizeof(____fmt),      \
                                     ##__VA_ARGS__);                    \
                })

struct {
	__uint(type, BPF_MAP_TYPE_ARRAY);
	__uint(max_entries, 20);
	__type(key, int);
	__type(value, int);
} minimal_map SEC(".maps");

SEC("xdp")
int xdp_minimal_main_0(struct xdp_md* ctx) {

    bpf_debug("Return XDP_PASS\n");

    return XDP_PASS;
}


char _license[] SEC("license") = "GPL";
