/* Test whether the compiler supports scalar storage order. */
struct __attribute__((__scalar_storage_order__("big-endian"))) {
	int n;
} sso;
