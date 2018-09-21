//////////////////////////////////////////////////////////////////////
//                      North Carolina State University
//
//
//
//                             Copyright 2016
//
////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
//
////////////////////////////////////////////////////////////////////////
//
//   Author:  Hung-Wei Tseng, Yu-Chia Liu
//
//   Description:
//     Header File of Processor Container
//
////////////////////////////////////////////////////////////////////////

#ifndef PROCESSOR_CONTAINER_H
#define PROCESSOR_CONTAINER_H

#include <linux/types.h>

struct processor_container_cmd
{
    __u64 op;
    __u64 cid;
};

#define PCONTAINER_IOCTL_LOCK _IOWR('N', 0x43, struct processor_container_cmd)
#define PCONTAINER_IOCTL_UNLOCK _IOWR('N', 0x44, struct processor_container_cmd)
#define PCONTAINER_IOCTL_DELETE _IOWR('N', 0x45, struct processor_container_cmd)
#define PCONTAINER_IOCTL_CREATE _IOWR('N', 0x46, struct processor_container_cmd)
#define PCONTAINER_IOCTL_CSWITCH _IOWR('N', 0x47, struct processor_container_cmd)

#endif
