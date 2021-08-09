Common helper library
=====================

This is all content around the common helper library.

[Background infos](../Documentation/knowledge_base.md).

The basic interface are make targets:

    +-----------------------------------------------------------+
    |                                                           |
    |                  Nothing to build                         |
    |                                                           |
    +-----------------------------------------------------------+
    | Example:                                                  |
    | make                    -> build all                      |
    | make clean              -> clean all                      |
    | make distclean          -> clean and uninstall            |
    | make install            -> install bin to $(HOME)/lib     |
    | make uninstall          -> remove bin from $(HOME)/lib    |
    +-----------------------------------------------------------+

WARNING: This is work in progress! Don't expect things to be complete in any dimension.

If you face a bug, then pls use https://github.com/tjohann/ambient_tracking_with_raspi/issues to create an ticket.


Requirement
-----------

The i2c-tools and i2c-tools-devel are needed.


Functions
---------

	char * alloc_string(const char *s);
	char * alloc_string_2(size_t len);
	int become_daemon(const char* name);
	int set_cloexec(int fd);
	int lock_file(int fd);
	int already_running(const char* lockfile);
	int init_i2c_device(char *adapter, unsigned char addr);
	int create_read_fifo(char *name);
	int create_write_fifo(char *name);
	void clr_flag(int fd, int flags);
	int gpio_export(int pin);
	int gpio_unexport(int pin);
	int gpio_set_direction(int pin, unsigned char dir);
	int gpio_set_edge_falling(int pin);
	int gpio_read(int pin);
	int gpio_write(int pin, int value);
	int cpu_temp_read(void);
	int bmp180_temp_read(void);
	int bmp180_pres_read(void);


Configuraion
------------

t.b.d.
