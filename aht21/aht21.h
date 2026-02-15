/*
 * lcd1602.h
 *
 *  Copyright: Oct 23, 2019
 *      Author: Dan Walkes
 */
// NOLINT(build/header_guard)
#ifndef AHT21_AHT21_H_
#define AHT21_AHT21_H_

#define AHT_DEBUG 1  // Remove comment on this line to enable debug

#undef PDEBUG             /* undef it, just in case */
#ifdef AHT_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#  define PDEBUG(fmt, args...) printk(KERN_DEBUG "aesdchar: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif  // AHT21_AHT21_H_
#endif
