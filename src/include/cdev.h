#ifndef CDEV_H
#define CDEV_H

#define CDEV_NAME  "kdo"
#define CDEV_MAX_WRITE 1024


int cdev_create(void);
int cdev_destroy(void);
#endif
