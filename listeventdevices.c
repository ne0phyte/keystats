#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>

int main (void)
{
	DIR *dp;
	struct dirent *ep;
	char* dir = "/dev/input/";
	dp = opendir (dir);
	char name[256] = "Unknown";
	char eventFile[128];
	int fd;
	if (dp != NULL)
	{
		while (ep = readdir (dp))
			if (ep->d_type != DT_DIR) {  
				snprintf(eventFile, sizeof eventFile, "%s%s", dir, ep->d_name);

				if ((fd = open (eventFile, O_RDONLY | O_NONBLOCK)) == -1) {
					printf ("%s is not a vaild device.\n", eventFile);
				} else {
					ioctl (fd, EVIOCGNAME (sizeof (name)), name);
					printf ("%s (%s)\n", eventFile, name);

				}
			}

		(void) closedir (dp);
	}
	else
		perror ("Couldn't open the directory");

	return 0;
}