/* SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause) */

/*
 * common eBPF ELF operations.
 *
 * Copyright (C) 2013-2015 Alexei Starovoitov <ast@kernel.org>
 * Copyright (C) 2015 Wang Nan <wangnan0@huawei.com>
 * Copyright (C) 2015 Huawei Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License (not later!)
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not,  see <http://www.gnu.org/licenses>
 */
#ifndef __LIBBPF_BPF_H
#define __LIBBPF_BPF_H

#include <linux/bpf.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "libbpf_common.h"
#include "libbpf_legacy.h"

#ifdef __cplusplus
extern "C" {
#endif

int libbpf_set_memlock_rlim(size_t memlock_bytes);

struct bpf_map_create_opts {
	size_t sz; /* size of this struct for forward/backward compatibility */

	__u32 btf_fd;
	__u32 btf_key_type_id;
	__u32 btf_value_type_id;
	__u32 btf_vmlinux_value_type_id;

	__u32 inner_map_fd;
	__u32 map_flags;
	__u64 map_extra;

	__u32 numa_node;
	__u32 map_ifindex;
};
#define bpf_map_create_opts__last_field map_ifindex

LIBBPF_API int bpf_map_create(enum bpf_map_type map_type,
			      const char *map_name,
			      __u32 key_size,
			      __u32 value_size,
			      __u32 max_entries,
			      const struct bpf_map_create_opts *opts);

struct bpf_create_map_attr {
	const char *name;
	enum bpf_map_type map_type;
	__u32 map_flags;
	__u32 key_size;
	__u32 value_size;
	__u32 max_entries;
	__u32 numa_node;
	__u32 btf_fd;
	__u32 btf_key_type_id;
	__u32 btf_value_type_id;
	__u32 map_ifindex;
	union {
		__u32 inner_map_fd;
		__u32 btf_vmlinux_value_type_id;
	};
};

LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_map_create() instead")
LIBBPF_API int bpf_create_map_xattr(const struct bpf_create_map_attr *create_attr);
LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_map_create() instead")
LIBBPF_API int bpf_create_map_node(enum bpf_map_type map_type, const char *name,
				   int key_size, int value_size,
				   int max_entries, __u32 map_flags, int node);
LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_map_create() instead")
LIBBPF_API int bpf_create_map_name(enum bpf_map_type map_type, const char *name,
				   int key_size, int value_size,
				   int max_entries, __u32 map_flags);
LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_map_create() instead")
LIBBPF_API int bpf_create_map(enum bpf_map_type map_type, int key_size,
			      int value_size, int max_entries, __u32 map_flags);
LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_map_create() instead")
LIBBPF_API int bpf_create_map_in_map_node(enum bpf_map_type map_type,
					  const char *name, int key_size,
					  int inner_map_fd, int max_entries,
					  __u32 map_flags, int node);
LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_map_create() instead")
LIBBPF_API int bpf_create_map_in_map(enum bpf_map_type map_type,
				     const char *name, int key_size,
				     int inner_map_fd, int max_entries,
				     __u32 map_flags);

struct bpf_prog_load_opts {
	size_t sz; /* size of this struct for forward/backward compatibility */

	/* libbpf can retry BPF_PROG_LOAD command if bpf() syscall returns
	 * -EAGAIN. This field determines how many attempts libbpf has to
	 *  make. If not specified, libbpf will use default value of 5.
	 */
	int attempts;

	enum bpf_attach_type expected_attach_type;
	__u32 prog_btf_fd;
	__u32 prog_flags;
	__u32 prog_ifindex;
	__u32 kern_version;

	__u32 attach_btf_id;
	__u32 attach_prog_fd;
	__u32 attach_btf_obj_fd;

	const int *fd_array;

	/* .BTF.ext func info data */
	const void *func_info;
	__u32 func_info_cnt;
	__u32 func_info_rec_size;

	/* .BTF.ext line info data */
	const void *line_info;
	__u32 line_info_cnt;
	__u32 line_info_rec_size;

	/* verifier log options */
	__u32 log_level;
	__u32 log_size;
	char *log_buf;
};
#define bpf_prog_load_opts__last_field log_buf

LIBBPF_API int bpf_prog_load(enum bpf_prog_type prog_type,
			     const char *prog_name, const char *license,
			     const struct bpf_insn *insns, size_t insn_cnt,
			     const struct bpf_prog_load_opts *opts);
/* this "specialization" should go away in libbpf 1.0 */
LIBBPF_API int bpf_prog_load_v0_6_0(enum bpf_prog_type prog_type,
				    const char *prog_name, const char *license,
				    const struct bpf_insn *insns, size_t insn_cnt,
				    const struct bpf_prog_load_opts *opts);

/* This is an elaborate way to not conflict with deprecated bpf_prog_load()
 * API, defined in libbpf.h. Once we hit libbpf 1.0, all this will be gone.
 * With this approach, if someone is calling bpf_prog_load() with
 * 4 arguments, they will use the deprecated API, which keeps backwards
 * compatibility (both source code and binary). If bpf_prog_load() is called
 * with 6 arguments, though, it gets redirected to __bpf_prog_load.
 * So looking forward to libbpf 1.0 when this hack will be gone and
 * __bpf_prog_load() will be called just bpf_prog_load().
 */
#ifndef bpf_prog_load
#define bpf_prog_load(...) ___libbpf_overload(___bpf_prog_load, __VA_ARGS__)
#define ___bpf_prog_load4(file, type, pobj, prog_fd) \
	bpf_prog_load_deprecated(file, type, pobj, prog_fd)
#define ___bpf_prog_load6(prog_type, prog_name, license, insns, insn_cnt, opts) \
	bpf_prog_load(prog_type, prog_name, license, insns, insn_cnt, opts)
#endif /* bpf_prog_load */

struct bpf_load_program_attr {
	enum bpf_prog_type prog_type;
	enum bpf_attach_type expected_attach_type;
	const char *name;
	const struct bpf_insn *insns;
	size_t insns_cnt;
	const char *license;
	union {
		__u32 kern_version;
		__u32 attach_prog_fd;
	};
	union {
		__u32 prog_ifindex;
		__u32 attach_btf_id;
	};
	__u32 prog_btf_fd;
	__u32 func_info_rec_size;
	const void *func_info;
	__u32 func_info_cnt;
	__u32 line_info_rec_size;
	const void *line_info;
	__u32 line_info_cnt;
	__u32 log_level;
	__u32 prog_flags;
};

/* Flags to direct loading requirements */
#define MAPS_RELAX_COMPAT	0x01

/* Recommended log buffer size */
#define BPF_LOG_BUF_SIZE (UINT32_MAX >> 8) /* verifier maximum in kernels <= 5.1 */

LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_prog_load() instead")
LIBBPF_API int bpf_load_program_xattr(const struct bpf_load_program_attr *load_attr,
				      char *log_buf, size_t log_buf_sz);
LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_prog_load() instead")
LIBBPF_API int bpf_load_program(enum bpf_prog_type type,
				const struct bpf_insn *insns, size_t insns_cnt,
				const char *license, __u32 kern_version,
				char *log_buf, size_t log_buf_sz);
LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_prog_load() instead")
LIBBPF_API int bpf_verify_program(enum bpf_prog_type type,
				  const struct bpf_insn *insns,
				  size_t insns_cnt, __u32 prog_flags,
				  const char *license, __u32 kern_version,
				  char *log_buf, size_t log_buf_sz,
				  int log_level);

struct bpf_btf_load_opts {
	size_t sz; /* size of this struct for forward/backward compatibility */

	/* kernel log options */
	char *log_buf;
	__u32 log_level;
	__u32 log_size;
};
#define bpf_btf_load_opts__last_field log_size

LIBBPF_API int bpf_btf_load(const void *btf_data, size_t btf_size,
			    const struct bpf_btf_load_opts *opts);

LIBBPF_DEPRECATED_SINCE(0, 8, "use bpf_btf_load() instead")
LIBBPF_API int bpf_load_btf(const void *btf, __u32 btf_size, char *log_buf,
			    __u32 log_buf_size, bool do_log);

LIBBPF_API int bpf_map_update_elem(int fd, const void *key, const void *value,
				   __u64 flags);

LIBBPF_API int bpf_map_lookup_elem(int fd, const void *key, void *value);
LIBBPF_API int bpf_map_lookup_elem_flags(int fd, const void *key, void *value,
					 __u64 flags);
LIBBPF_API int bpf_map_lookup_and_delete_elem(int fd, const void *key,
					      void *value);
LIBBPF_API int bpf_map_lookup_and_delete_elem_flags(int fd, const void *key,
						    void *value, __u64 flags);
LIBBPF_API int bpf_map_delete_elem(int fd, const void *key);
LIBBPF_API int bpf_map_get_next_key(int fd, const void *key, void *next_key);
LIBBPF_API int bpf_map_freeze(int fd);

struct bpf_map_batch_opts {
	size_t sz; /* size of this struct for forward/backward compatibility */
	__u64 elem_flags;
	__u64 flags;
};
#define bpf_map_batch_opts__last_field flags


/**
 * @brief **bpf_map_delete_batch()** allows for batch deletion of multiple
 * elements in a BPF map.
 *
 * @param fd BPF map file descriptor
 * @param keys pointer to an array of *count* keys
 * @param count input and output parameter; on input **count** represents the
 * number of  elements in the map to delete in batch;
 * on output if a non-EFAULT error is returned, **count** represents the number of deleted
 * elements if the output **count** value is not equal to the input **count** value
 * If EFAULT is returned, **count** should not be trusted to be correct.
 * @param opts options for configuring the way the batch deletion works
 * @return 0, on success; negative error code, otherwise (errno is also set to
 * the error code)
 */
LIBBPF_API int bpf_map_delete_batch(int fd, const void *keys,
				    __u32 *count,
				    const struct bpf_map_batch_opts *opts);

/**
 * @brief **bpf_map_lookup_batch()** allows for batch lookup of BPF map elements.
 *
 * The parameter *in_batch* is the address of the first element in the batch to read.
 * *out_batch* is an output parameter that should be passed as *in_batch* to subsequent
 * calls to **bpf_map_lookup_batch()**. NULL can be passed for *in_batch* to indicate
 * that the batched lookup starts from the beginning of the map.
 *
 * The *keys* and *values* are output parameters which must point to memory large enough to
 * hold *count* items based on the key and value size of the map *map_fd*. The *keys*
 * buffer must be of *key_size* * *count*. The *values* buffer must be of
 * *value_size* * *count*.
 *
 * @param fd BPF map file descriptor
 * @param in_batch address of the first element in batch to read, can pass NULL to
 * indicate that the batched lookup starts from the beginning of the map.
 * @param out_batch output parameter that should be passed to next call as *in_batch*
 * @param keys pointer to an array large enough for *count* keys
 * @param values pointer to an array large enough for *count* values
 * @param count input and output parameter; on input it's the number of elements
 * in the map to read in batch; on output it's the number of elements that were
 * successfully read.
 * If a non-EFAULT error is returned, count will be set as the number of elements
 * that were read before the error occurred.
 * If EFAULT is returned, **count** should not be trusted to be correct.
 * @param opts options for configuring the way the batch lookup works
 * @return 0, on success; negative error code, otherwise (errno is also set to
 * the error code)
 */
LIBBPF_API int bpf_map_lookup_batch(int fd, void *in_batch, void *out_batch,
				    void *keys, void *values, __u32 *count,
				    const struct bpf_map_batch_opts *opts);

/**
 * @brief **bpf_map_lookup_and_delete_batch()** allows for batch lookup and deletion
 * of BPF map elements where each element is deleted after being retrieved.
 *
 * @param fd BPF map file descriptor
 * @param in_batch address of the first element in batch to read, can pass NULL to
 * get address of the first element in *out_batch*
 * @param out_batch output parameter that should be passed to next call as *in_batch*
 * @param keys pointer to an array of *count* keys
 * @param values pointer to an array large enough for *count* values
 * @param count input and output parameter; on input it's the number of elements
 * in the map to read and delete in batch; on output it represents the number of
 * elements that were successfully read and deleted
 * If a non-**EFAULT** error code is returned and if the output **count** value
 * is not equal to the input **count** value, up to **count** elements may
 * have been deleted.
 * if **EFAULT** is returned up to *count* elements may have been deleted without
 * being returned via the *keys* and *values* output parameters.
 * @param opts options for configuring the way the batch lookup and delete works
 * @return 0, on success; negative error code, otherwise (errno is also set to
 * the error code)
 */
LIBBPF_API int bpf_map_lookup_and_delete_batch(int fd, void *in_batch,
					void *out_batch, void *keys,
					void *values, __u32 *count,
					const struct bpf_map_batch_opts *opts);

/**
 * @brief **bpf_map_update_batch()** updates multiple elements in a map
 * by specifying keys and their corresponding values.
 *
 * The *keys* and *values* parameters must point to memory large enough
 * to hold *count* items based on the key and value size of the map.
 *
 * The *opts* parameter can be used to control how *bpf_map_update_batch()*
 * should handle keys that either do or do not already exist in the map.
 * In particular the *flags* parameter of *bpf_map_batch_opts* can be
 * one of the following:
 *
 * Note that *count* is an input and output parameter, where on output it
 * represents how many elements were successfully updated. Also note that if
 * **EFAULT** then *count* should not be trusted to be correct.
 *
 * **BPF_ANY**
 *    Create new elements or update existing.
 *
 * **BPF_NOEXIST**
 *    Create new elements only if they do not exist.
 *
 * **BPF_EXIST**
 *    Update existing elements.
 *
 * **BPF_F_LOCK**
 *    Update spin_lock-ed map elements. This must be
 *    specified if the map value contains a spinlock.
 *
 * @param fd BPF map file descriptor
 * @param keys pointer to an array of *count* keys
 * @param values pointer to an array of *count* values
 * @param count input and output parameter; on input it's the number of elements
 * in the map to update in batch; on output if a non-EFAULT error is returned,
 * **count** represents the number of updated elements if the output **count**
 * value is not equal to the input **count** value.
 * If EFAULT is returned, **count** should not be trusted to be correct.
 * @param opts options for configuring the way the batch update works
 * @return 0, on success; negative error code, otherwise (errno is also set to
 * the error code)
 */
LIBBPF_API int bpf_map_update_batch(int fd, const void *keys, const void *values,
				    __u32 *count,
				    const struct bpf_map_batch_opts *opts);

LIBBPF_API int bpf_obj_pin(int fd, const char *pathname);
LIBBPF_API int bpf_obj_get(const char *pathname);

struct bpf_prog_attach_opts {
	size_t sz; /* size of this struct for forward/backward compatibility */
	unsigned int flags;
	int replace_prog_fd;
};
#define bpf_prog_attach_opts__last_field replace_prog_fd

LIBBPF_API int bpf_prog_attach(int prog_fd, int attachable_fd,
			       enum bpf_attach_type type, unsigned int flags);
LIBBPF_API int bpf_prog_attach_opts(int prog_fd, int attachable_fd,
				     enum bpf_attach_type type,
				     const struct bpf_prog_attach_opts *opts);
LIBBPF_DEPRECATED_SINCE(0, 8, "use bpf_prog_attach_opts() instead")
LIBBPF_API int bpf_prog_attach_xattr(int prog_fd, int attachable_fd,
				     enum bpf_attach_type type,
				     const struct bpf_prog_attach_opts *opts);
LIBBPF_API int bpf_prog_detach(int attachable_fd, enum bpf_attach_type type);
LIBBPF_API int bpf_prog_detach2(int prog_fd, int attachable_fd,
				enum bpf_attach_type type);

union bpf_iter_link_info; /* defined in up-to-date linux/bpf.h */
struct bpf_link_create_opts {
	size_t sz; /* size of this struct for forward/backward compatibility */
	__u32 flags;
	union bpf_iter_link_info *iter_info;
	__u32 iter_info_len;
	__u32 target_btf_id;
	union {
		struct {
			__u64 bpf_cookie;
		} perf_event;
		struct {
			__u32 flags;
			__u32 cnt;
			const char **syms;
			const unsigned long *addrs;
			const __u64 *cookies;
		} kprobe_multi;
	};
	size_t :0;
};
#define bpf_link_create_opts__last_field kprobe_multi.cookies

LIBBPF_API int bpf_link_create(int prog_fd, int target_fd,
			       enum bpf_attach_type attach_type,
			       const struct bpf_link_create_opts *opts);

LIBBPF_API int bpf_link_detach(int link_fd);

struct bpf_link_update_opts {
	size_t sz; /* size of this struct for forward/backward compatibility */
	__u32 flags;	   /* extra flags */
	__u32 old_prog_fd; /* expected old program FD */
};
#define bpf_link_update_opts__last_field old_prog_fd

LIBBPF_API int bpf_link_update(int link_fd, int new_prog_fd,
			       const struct bpf_link_update_opts *opts);

LIBBPF_API int bpf_iter_create(int link_fd);

struct bpf_prog_test_run_attr {
	int prog_fd;
	int repeat;
	const void *data_in;
	__u32 data_size_in;
	void *data_out;      /* optional */
	__u32 data_size_out; /* in: max length of data_out
			      * out: length of data_out */
	__u32 retval;        /* out: return code of the BPF program */
	__u32 duration;      /* out: average per repetition in ns */
	const void *ctx_in; /* optional */
	__u32 ctx_size_in;
	void *ctx_out;      /* optional */
	__u32 ctx_size_out; /* in: max length of ctx_out
			     * out: length of cxt_out */
};

LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_prog_test_run_opts() instead")
LIBBPF_API int bpf_prog_test_run_xattr(struct bpf_prog_test_run_attr *test_attr);

/*
 * bpf_prog_test_run does not check that data_out is large enough. Consider
 * using bpf_prog_test_run_opts instead.
 */
LIBBPF_DEPRECATED_SINCE(0, 7, "use bpf_prog_test_run_opts() instead")
LIBBPF_API int bpf_prog_test_run(int prog_fd, int repeat, void *data,
				 __u32 size, void *data_out, __u32 *size_out,
				 __u32 *retval, __u32 *duration);
LIBBPF_API int bpf_prog_get_next_id(__u32 start_id, __u32 *next_id);
LIBBPF_API int bpf_map_get_next_id(__u32 start_id, __u32 *next_id);
LIBBPF_API int bpf_btf_get_next_id(__u32 start_id, __u32 *next_id);
LIBBPF_API int bpf_link_get_next_id(__u32 start_id, __u32 *next_id);
LIBBPF_API int bpf_prog_get_fd_by_id(__u32 id);
LIBBPF_API int bpf_map_get_fd_by_id(__u32 id);
LIBBPF_API int bpf_btf_get_fd_by_id(__u32 id);
LIBBPF_API int bpf_link_get_fd_by_id(__u32 id);
LIBBPF_API int bpf_obj_get_info_by_fd(int bpf_fd, void *info, __u32 *info_len);
LIBBPF_API int bpf_prog_query(int target_fd, enum bpf_attach_type type,
			      __u32 query_flags, __u32 *attach_flags,
			      __u32 *prog_ids, __u32 *prog_cnt);
LIBBPF_API int bpf_raw_tracepoint_open(const char *name, int prog_fd);
LIBBPF_API int bpf_task_fd_query(int pid, int fd, __u32 flags, char *buf,
				 __u32 *buf_len, __u32 *prog_id, __u32 *fd_type,
				 __u64 *probe_offset, __u64 *probe_addr);

enum bpf_stats_type /* defined in up-to-date linux/bpf.h */
//m-> changed to solve c++ compilation error (https://github.com/libbpf/libbpf/issues/249)
#ifdef __cplusplus
: int
#endif
;
LIBBPF_API int bpf_enable_stats(enum bpf_stats_type type);

struct bpf_prog_bind_opts {
	size_t sz; /* size of this struct for forward/backward compatibility */
	__u32 flags;
};
#define bpf_prog_bind_opts__last_field flags

LIBBPF_API int bpf_prog_bind_map(int prog_fd, int map_fd,
				 const struct bpf_prog_bind_opts *opts);

struct bpf_test_run_opts {
	size_t sz; /* size of this struct for forward/backward compatibility */
	const void *data_in; /* optional */
	void *data_out;      /* optional */
	__u32 data_size_in;
	__u32 data_size_out; /* in: max length of data_out
			      * out: length of data_out
			      */
	const void *ctx_in; /* optional */
	void *ctx_out;      /* optional */
	__u32 ctx_size_in;
	__u32 ctx_size_out; /* in: max length of ctx_out
			     * out: length of cxt_out
			     */
	__u32 retval;        /* out: return code of the BPF program */
	int repeat;
	__u32 duration;      /* out: average per repetition in ns */
	__u32 flags;
	__u32 cpu;
	__u32 batch_size;
};
#define bpf_test_run_opts__last_field batch_size

LIBBPF_API int bpf_prog_test_run_opts(int prog_fd,
				      struct bpf_test_run_opts *opts);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __LIBBPF_BPF_H */
