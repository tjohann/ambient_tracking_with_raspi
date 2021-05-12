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

No additional libaries are needed.


Funcions
--------

	char * alloc_string(const char *s);
	int become_daemon(const char* name);
	int set_cloexec(int fd);
	int lock_file(int fd);
	int already_running(const char* lockfile);


Configuraion
------------

t.b.d.