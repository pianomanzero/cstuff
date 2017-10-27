/*
 * lockmenu.c - a simple menu-driven program for basic testing and diagnosis of
 * filesystem locks using the fcntl() API.
 *
 * DISCLAIMERS: This is FREE CODE, intended primarily for instructional purposes
 * in non-production environments! This code has no warranties of any sort whatsoever,
 * expressed or implied, including any warranties of correctness or suitability for
 * any particular purpose. Use is entirely AT YOUR OWN RISK!
 *
 * CAUTION: Using this program in a production environment has the potential to
 * disrupt production workflows. In particular, taking a read lock will block any
 * other process from obtaining a write lock, and taking a write lock will block
 * any other process from taking a read or write lock. Merely QUerying the status
 * locks on a file should be a non-intrusive operation.
 *
 * --- Revision Information ---
 *
 * Version 2.00 - Bob.Sneed@emc.com, August 4, 2015
 *	- General re-write, changed menu structure, main intents preserved
 *	- Target file for lock provided on the command line (see usage())
 *	- Allow observing the STATUS of the file when we hold a lock
 * Version 1.00 - by SAS Institute, March 4, 2013
 *
 * --- Program Description ---
 *
 * lockmenu provides a simple interactive means of testing fcntl() filesystem
 * locking primitives and for determmining the lock status of a file. lockmenu
 * allows the user to interactively obtain and release read and writes locks on
 * a file named on the command line, or inquire about the status of existing
 * locks on a file.
 *
 * --- Limitations ---
 *
 * The locking APIs used in the Unix/Linux realm have certain inherent design
 * limitations of which application programmers must be aware, and these should
 * also be understand by lockmenu users. These are the main limitations;
 *
 *    1. The same process that creates a lock on a file cannot properly
 *       check the lock status of that file using fcntl(), which is the
 *       only tool available to do such checking.  fcntl() issued from the
 *       same process that created the lock reports that there are no locks
 *       on the file. (lockmenu still allows a 'Status' operation on the
 *       file when it holds a lock; it just provides that status based on
 *       it's knowledge that it holds a lock.)
 *
 *    2. If there are multiple instances open file handles on a given file,
 *       a close() operation on *any* of these file handles will release
 *       *all* locks on the file, whether of which file descriptor was used
 *       to obtain the lock.
 *
 * --- General Locking Notes ---
 *
 * Read locks are also called 'shared locks', and Write locks are also called
 * 'exclusive locks'.
 *
 * File Locking Rules:
 *
 *    1. A file CAN have concurrent shared Read locks.
 *
 *    2. When one or more process has placed a Read lock on a file,
 *       NO processes can obtain an exclusive Write lock on the file
 *       and/or any region of the file.
 *
 *    3. Whenever a process holds a Write lock on a file, no other process
 *       can obtain either a Read lock or another Write lock on that file.
 *
 *    4. Only "regular" file types can have a file lock established.  File
 *       types of "directory", "block special", "fifo", "symbolic links",
 *       "character special", and "socket" cannot have file locks established
 *       on them.
 *
 * Noted Operating System Functions:
 *
 *    The function from the UNIX API that establishes the file locking requests or
 *    checks for existing file locks is fcntl(2).  fcntl(2) is termed "file control",
 *    and provides various other functionality in additional to control over locks.
 *    Consult your system-specific documentation for details on the implementation.
 *
 *    In general, to perform file/record locking on an a file, the file MUST exist,
 *    MUST be a regular file, and MUST be accessable with sufficient permissions by
 *    the caller to perform the operation for whuch the lock is obtained.  The
 *    following is a list and the descriptions of the file locking parameters for
 *    fcntl(2):
 *
 *    F_GETLK:  This function code can be used to inquire if the specified lock type
 *              *could* be taken on a file without actually taking a lock. In order
 *              to detect both read and write locks, the F_WRTLK lock type must be
 *              passed. If a lock in already held that would prevent the specified
 *              lock type from being obtained, return information about the first such
 *              conflicting lock held on the file. The process id of the process that
 *              holds the conflicting lock is returned if that process in on the same
 *              client as where the query is issued, but is reported as 0 if the
 *              conflicting locks is held by another filesystem client in shared
 *              filesystem context. If no current lock would prevent the specified
 *              lock from being granted, the value F_UNLCK is passed back to caller's
 *              the flock.l_type structure entry -- otherwise, the type of the
 *              conflicting lock (F_RDLCK or F_WRLCK) is returned to this structure.
 *
 *              NOTE: Inasmuch as the results from F_GETLK may no longer be valid
 *              by the time they are received, F_GETLK is rarely used in application
 *              programming.
 *
 *    F_SETLK:  This function code is used to obtain a Read lock (F_RDLCK) or a Write
 *              lock (F_WRLCK) on a file, and also to release (F_UNLCK) any lock on a
 *              file that is held by the caller. It will fail if a conflicting lock is
 *              outstanding or if the caller lacks appropriate read/write access to
 *              the file.
 *
 *              NOTE: Obtaining a lock is an 'atomic' operation that inherently tests
 *              the status of a lock before granting it. The 'atomic' characteristic
 *              of these operations assures that no two processes can ever mistakenly
 *              be granted conflicting locks as the same time.
 *
 *    F_SETLKW: This function code is the same as F_SETLK, but will wait (perhaps
 *              indefinitely) until the requested lock can be granted. Lock managers
 *              typically service such lock requests in a First-In-First-Out (FIFO)
 *              order, so this can be handy in certain applications scenarios that
 *              involve heavily-contended locking operations.
 *
 *              NOTE: Inasmuch as the possibility of waiting forever for a lock does not
 *              appeal to most application programmers, the most popular locking strategy
 *              among developers frequently resembles the following pseudo-code which
 *              demonstrates using retry logic to implement a degree of application
 *              relience in the event of a contended lock;
 *
 *                 for (try=0; try < MAXTRY; try++) {
 *                     if (gotlock = getlock(file, lock_type)) break;
 *                     sleep(<milliseconds>);
 *                 }
 *                 if (!gotlock) fail();
 *                 // perform operation on locked data
 *                 unlock();
 *
 *              ... where getlock() is fcntl(F_SETLK, ... {with F_RDLCK or F_WRLCK} ...)
 *              ... and unlock() is fcntl(F_SETLK, ... {with F_UNLCK} ...)
 *
 *
 *    flock()ata structure members:
 *    =============================
 *
 *    short l_type   F_RDLCK, F_WRLCK, F_UNLCK
 *                   file locking types
 *    short l_start  starting byte offset in file
 *    off_t l_whence see lseek(2)
 *    off_t l_len    number of bytes to lock.
 *                   if 0, then locks entire file.
 *    pid_t l_pid    process holding lock
 *
 * Note that the off_t data type is either 32- or 64-bits, depending
 * on whether or not the program supports large files (> 2 GB).
 *
|
| Notes on file locking across NFS:
|
|    The network daemons "rpc.statd" and "rpc.lockd" perform NFS
|    file locking requests. If there are problems establishing file
|    locks on files across NFS, these network daemons may need to be
|    restarted on both the server and the client NFS machine.
|    Please refer this task to your UNIX system administrator.
|    Reference your Operating System documentation for more
|    information.
//
// File locking in NFSv3 and NFSv4 are quite different.
//
// NFSv3 uses a sideband Network Lock Manager (NLM) protocol, which
// implements ADVISORY locks. Advisory locks are only useful between
// processes which are explictly coded to cooperate in their use.
// Regardless of the status of NLM-based advisory locks on a file,
// any process with write access to the file can write the file at
// any time.
//
// NFSv4 does its locks in-band (that is, in the same protocol
// connection used to transfer data), and its locks are MANDATORY.
// NFSv4 locks are enforced between all processes accessing a file,
// whether or not some of those processes even include code for
// lock awareness.
//
// Depending on the implementation specifics of the NFS server platform,
// NFSv3 and NFSv4 may not be interoperable, and may not be consistent
   with other locking protocols supported by the same server platform;
      (*) NFSv3 and NFSv4 locks may be generally completely distinct and
          unaware of each other, in which case care must be taken to avoid
          the two locking protocols being used on the same files at the
          same time.
// UNAWARE of each other, though an NFS server implementation could
// choose to somehow merge the implementations. On file servers that
// allow shared access to files between different protocols, there
// is no implicit assurance that locking activity from one protocol
// will be consistent or in any way connected with locking activity
// from another protocol, or from the file locking implementation
// on the server's native file system.
//
|
*/

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int		rc;			// Return code 
int		fd = 0;			// File descriptor from open(2)
char           *filename = NULL;	// Ptr to filename (argv[1])
char		c         [1024];	// Space to hold user input
char           *choice = c;		// Ptr to user input
struct flock	Rlock, Wlock, Slock, Ulock;	// For fcntl()
int		locked = 0;		// Boolean for 'a lock is held by us'
char		command   [1024];	// 
struct stat	sb;

/*****************************************************************************************************/
/* remove lock created by this application                                                           */
/*****************************************************************************************************/

void
unlock()
{
   if (locked == 1) {
      if ((rc = close(fd)) == -1) {
	 fprintf(stderr, "<<Unlock WARNING>>  Error closing file: %s\n", strerror(errno));
      } else {
	 fprintf(stdout, "<<Unlock STATUS>>   Lock removed successfully by close()\n");
	 fd = 0;
	 locked = 0;
      }
   } else {
      fprintf(stdout, "<<Unlock STATUS>>   File is not locked by this application\n");
   }
}

/*****************************************************************************************************/
/* create read lock                                                                                  */
/*****************************************************************************************************/
void
readlock()
{
   if ((fd = open(filename, O_RDONLY)) < 0) {
      fprintf(stderr, "<<F_RDLCK ERROR>>    Unable to open file for READ: %s\n", strerror(errno));
   } else {			/* attempt a read lock */
      Rlock.l_type = F_RDLCK;	/* F_RDLCK, F_WRLCK, F_UNLCK */
      Rlock.l_start = 0;		/* byte offset, relative to l_whence */
      Rlock.l_whence = SEEK_SET;	/* SEEK_SET, SEEK_CUR, SEEK_END */
      if ((rc = fcntl(fd, F_SETLK, &Rlock)) == -1) {
	 fprintf(stderr, "<<F_RDLCK ERROR>>    Unable to establish a READ lock: %s\n", strerror(errno));
      } else {
	 fprintf(stdout, "<<F_RDLCK STATUS>>   A READ lock has been established\n");
	 locked = 1;
      }
   }
}

/*****************************************************************************************************/
/* create write lock                                                                                 */
/*****************************************************************************************************/
void
writelock()
{
   if ((fd = open(filename, O_RDWR)) < 0) {
      fprintf(stderr, "<<F_WRLCK ERROR>>    Unable to open file for WRITE: %s\n", strerror(errno));
   } else {			/* attempt a write lock */
      Wlock.l_type = F_WRLCK;		/* F_RDLCK, F_WRLCK, F_UNLCK */
      Wlock.l_start = 0;		/* byte offset, relative to l_whence */
      Wlock.l_whence = SEEK_SET;	/* SEEK_SET, SEEK_CUR, SEEK_END */
      if ((rc = fcntl(fd, F_SETLK, &Wlock)) == -1) {
	 fprintf(stderr, "<<F_WRLCK ERROR>>    Unable to establish a WRITE lock: %s\n", strerror(errno));
      } else {
	 fprintf(stdout, "<<F_WRLCK STATUS>>   A WRITE lock has been established\n");
	 locked = 1;
      }
   }
}

/*****************************************************************************************************/
/* describe file                                                                                     */
/*****************************************************************************************************/
void
describefile()
{
   if (lstat(filename, &sb) == -1) {
      exit(EXIT_FAILURE);
   } else {
      printf("File type               : ");
      switch (sb.st_mode & S_IFMT) {
      case S_IFBLK:
	 printf("block device\n");
	 break;
      case S_IFCHR:
	 printf("character device\n");
	 break;
      case S_IFDIR:
	 printf("directory\n");
	 break;
      case S_IFIFO:
	 printf("FIFO/pipe\n");
	 break;
      case S_IFLNK:
	 printf("symlink\n");
	 break;
      case S_IFREG:
	 printf("regular file\n");
	 break;
      case S_IFSOCK:
	 printf("socket\n");
	 break;
      default:
	 printf("<unknown>\n");
	 break;
      }
      printf("I-node number           : %ld\n", (long)sb.st_ino);
      printf("Mode                    : %lo (octal)\n", (unsigned long)sb.st_mode);
      printf("Link count              : %ld\n", (long)sb.st_nlink);
      printf("Ownership               : UID=%ld   GID=%ld\n", (long)sb.st_uid, (long)sb.st_gid);
      printf("Preferred I/O block size: %ld bytes\n", (long)sb.st_blksize);
      printf("File size               : %lld bytes\n", (long long)sb.st_size);
      printf("Blocks allocated        : %lld\n", (long long)sb.st_blocks);
      printf("Last status change      : %s", ctime(&sb.st_ctime));
      printf("Last file access        : %s", ctime(&sb.st_atime));
      printf("Last file modification  : %s\n", ctime(&sb.st_mtime));
   }
}

/*****************************************************************************************************/
/* file status                                                                                       */
/*****************************************************************************************************/
void
filestatus()
{
   if ((fd = open(filename, O_RDONLY)) < 0) {
      fprintf(stderr, "<<ERROR>>    Unable to open file for READ: %s\n", strerror(errno));
      return;
   }

   describefile();

#ifdef TEST
   Slock.l_type = F_WRLCK;
   Slock.l_start = 0;
   Slock.l_whence = 0;
   Slock.l_len = 0;
   Slock.l_pid = getpid();		// if not over-written, holder musyt be us
#else
   Slock.l_type = F_WRLCK;
   Slock.l_start = 0;
   Slock.l_whence = SEEK_SET;
   Slock.l_len = 0;
   Slock.l_pid = getpid();
#endif

   if ((rc = fcntl(fd, F_GETLK, &Slock)) == -1) {
      fprintf(stderr, "<<F_GETLK ERROR>>    Unable to F_GETLK: %s\n", strerror(errno));
      return;
   }
   fprintf(stdout, "<<F_GETLK results>>");
   if (locked) {
      fprintf(stdout, " Existing %s lock on file by pid %d\n\t(WE are pid %d)\n",
         (Slock.l_type == F_RDLCK) ? "READ" : "WRITE", Slock.l_pid, getpid());
   } else if (Slock.l_type == F_RDLCK) {
      fprintf(stdout, " Existing READ lock on file by pid %d\n", Slock.l_pid);
   } else if (Slock.l_type == F_WRLCK) {
      fprintf(stdout, " Existing WRITE lock on file by pid %d\n", Slock.l_pid);
   } else if (Slock.l_type == F_UNLCK) {
      fprintf(stdout, " There is NO lock on the file\n");
   }
   if (locked || (Slock.l_type != F_UNLCK)) {
      if (Slock.l_pid == 0) {
         fprintf(stdout, "\t(Lock-owning pid not from this OS image.)\n");
      } else if (Slock.l_pid != getpid()) {
         fprintf(stdout, "\nps results:\n");
         sprintf(command, "ps -aef | grep %d | egrep -v grep", Slock.l_pid);
         rc = system(command);
      }
   }

#ifdef SAS_CODE
   fprintf(stdout, "\ndf results:\n\n");
   sprintf(command, "df -P %s", file);
   rc = system(command);
   FILE           *fp;
   char		   path   [1035] = "";
   sprintf(command, "df -P %s | awk \'{ print $6 }\' | tail -1", file);
   fp = popen(command, "r");
   if (fp != NULL) {
      if (fgets(path, sizeof(path) - 1, fp) != NULL) {
	 fprintf(stdout, "\nmount results:\n\n");
	 sprintf(command, "mount | grep %s", path);
	 rc = system(command);
	 fprintf(stdout, "\n");
	 pclose(fp);
      }
   }
#endif
}

/*****************************************************************************************************/
/* main                                                                                              */
/*****************************************************************************************************/
int
main(int argc, char *argv[])
{
   if (argc != 2) {
      fprintf(stderr, "Usage: filemenu <filename>\n");
      return -1;
   }
   filename = argv[1];
   while (1) {
      fprintf(stdout, "\nMenu for \'%s\':\n", filename);
      fprintf(stdout, "   u: Unlock\n");
      fprintf(stdout, "   r: Read lock\n");
      fprintf(stdout, "   w: Write lock\n");
      fprintf(stdout, "   s: Status file and lock\n");
      fprintf(stdout, "   q: Quit\n");
      fprintf(stdout, "Select option: ");
      strcpy(c, "?");
      if (scanf("%s", c) <= 0) c[0] = 'q';	// Catch EOF; 'x', 'q', ^D, and ^C will all cause exit
      c[0] = tolower(c[0]);
      printf("\n");
      switch (c[0]) {
      case 'u':
	 unlock();
	 break;
      case 'r':		/* read lock */
	 readlock();
	 break;
      case 'w':		/* write lock */
	 writelock();
	 break;
      case 's':		/* status of a file */
	 filestatus();
	 break;
      case 'x':		/* quit (was 'exit') in earlier version */
      case 'q':
	 exit(0);
      default:
	 fprintf(stderr, "<<ERROR>>    Invalid option.\n");
      }				/* switch */
   }				/* while */
}				/* main */
