# Makefile for all system servers.
#

.include <bsd.own.mk>

.if ${MKIMAGEONLY} == "yes"

SUBDIR=	ds init mfs pfs pm rs sched sem vfs vm

.else

SUBDIR=	ds ext2 inet init ipc is iso9660fs \
	mfs pfs pm procfs rs sched sem vfs vm devman

.if ${MACHINE_ARCH} == "i386"
SUBDIR+= hgfs vbfs
.endif

.endif

.include <bsd.subdir.mk>
