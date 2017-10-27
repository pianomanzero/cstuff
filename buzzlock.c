/*
   buzzlock.c - Stress test microbenchmark for file locking.

   DISCLAIMERS: This is FREE CODE, intended purely for instructional purposes in
   non-production environments! This code has no warranties of any sort whatsoever,
   expressed or implied, including any warranties of correctness or suitability for
   any particular purpose. Use is entirely AT YOUR OWN RISK!

   WARNING: By default, this code will create a burst of 1000 iterations of file-
   lock-related requests specified on the command line. With '-n0', it will iterate
   indefininately, which is likely to cause a severe impact on the locking mechanism
   of the server that hosts the specified test file.

   NOTE: No; there is no documentation apart from the comments and the code itself.

   By: Bob.Sneed@Isilon.com June, 2015

   1.00 - Initial release
*/

char Version[] = "buzzlock 1.00";

/*
   To build on Linux:	gcc -DLINUX buzzlock.c -o buzzlock -lrt
   To build on OSX:	cc buzzlock.c -o buzzlock

   <<< Interesting test cases >>>

   Test case #0: Running this alone ...
	./buzzlock -r -u <test_file>
   ... gives a notion of peak uncontended single-stream lock op rate

   Test case #1: Running these in competition ...
      ./buzzlock -rw -u -n0 <test_file>
      ./buzzlock -ww -u -n0 <test_file>
   ... all locks succeed (due to wait)

   Test case #2: Running these in competition (SAS case) ...
      ./buzzlock -ts -r -u -n0 <test_file>
      ./buzzlock -ts -r -u -n0 <test_file>
   ... on OneFS (pre-8.0) may get transient -ts or -r failures due to locks taken 'under the hood' by OneFS
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/time.h>

#if defined(__MACH__)
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#if !defined(TRUE)
#define FALSE 0
#define TRUE (!FALSE)
#endif

#ifndef PATH_MAX
#define PATH_MAX MAXPATHNAME
#endif

unsigned long N_REPS=1000;	// Total iterations; 0 is infinite
unsigned long N_REPORT=1000;	// Reporting interval; defaults to N_REPS
#define MAX_OPS 64		// Max locking ops in a single iteration (-r -w -rw -ww -tr -tw -ts -u sequence)

struct OpTable {		// Scoreboard
   int index;
   char *cmd;
   char *desc;
   unsigned long s_iter, t_iter;	// Subtotal and total iters
   unsigned long s_errs, t_errs;	// Subtotal and total errs
   unsigned long long s_ns, t_ns;	// Subtotal and total ns
} Ops[] = {
   1, "-tw", "TestW", 0, 0, 0, 0, 0, 0,
   2, "-tr", "TestR", 0, 0, 0, 0, 0, 0,
   3, "-ts", "TestS", 0, 0, 0, 0, 0, 0,
   4, "-rw", "RLockWait", 0, 0, 0, 0, 0, 0,
   5, "-ww", "WLockWait", 0, 0, 0, 0, 0, 0,
   6, "-r", "Rlock", 0, 0, 0, 0, 0, 0,
   7, "-w", "Wlock", 0, 0, 0, 0, 0, 0,
   8, "-u", "Unlock", 0, 0, 0, 0, 0, 0,
   0, NULL, NULL, 0, 0, 0, 0, 0, 0
};

struct OpTable *OpT[MAX_OPS];	// Ops for each iteration (from command line)

unsigned long T_RW_Locks = 0;	// Locks obtained, cumulative
long long NS_LastReport = 0;	// Period time for report()
long long NS_StartTime = 0;	// For total elapsed time

// usage() - exit path

void
usage(void)
{
   printf("%s usage: buzzlock [-n<N>] [-i<N>] {<op> ...} <lockfile>\n  Where <op> values are:\n", Version);
   printf("   -tw - TestW - fail if write lock not available (read or write lock inferred)\n");
   printf("   -tr - TestR - fail if read lock not available (write lock inferred)\n");
   printf("   -ts - TestS - fail if write lock is held (write lock explicity detected)\n");
   printf("   -rw - RLockWait - get read lock, with wait\n");
   printf("   -ww - WLockWait - get write lock, with wait\n");
   printf("    -r - Rlock - fails if read lock not immediately available\n");
   printf("    -w - Wlock - fails if write lock not immediately available\n");
   printf("    -u - Unlock - always succeeds, even if no lock held\n");
   printf(" -n<N> - Number of iterations; defaults to 1000, N=0 -> INFINITE\n");
   printf(" -i<N> - Report interval; defaults to -n iterations\n");
   printf("Any <op> error skips the remainder of the current iteration.\n");
   exit(-1);
}

// timestamp() - Output timestamp ff time has changed or force arg is non-zero

void
timestamp(int force)
{
   time_t Now;
   static time_t last_time = 0;

   time(&Now);
   if (force || (Now != last_time)) {
      printf("%s", ctime(&Now));
      last_time = Now;
   }
}

void
parse_options(char *arg)
{
   char *pc;
   int i;

   assert(arg[0] == '-');
   if (arg[1] == '\0') {
      printf("Missing letter options after '-'!\n");
      exit(-1);
   }
}

// gethrtime(void) - return nanoseconds, monotonically ascending, platform-independent.

// See also:
// http://nadeausoftware.com/articles/2012/04/c_c_tip_how_measure_elapsed_real_time_benchmarking#machabsolutetimenbsp
// http://stackoverflow.com/questions/12392278/measure-time-in-linux-getrusage-vs-clock-gettime-vs-clock-vs-gettimeofday
// ... all interesting, but OSX case remains troublesome.
// http://stackoverflow.com/questions/11680461/monotonic-clock-on-osx
// ... seems to fill the knowledge gap.

#if defined(BSD) || defined(LINUX) || defined(__MACH__)
long long
gethrtime(void)
{
   struct timespec ts; 
#if defined(__MACH__)
   // OS X does not have clock_gettime, use clock_get_time
   // see http://stackoverflow.com/questions/11680461/monotonic-clock-on-osx
   clock_serv_t cclock;
   mach_timespec_t mts;

   host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
   clock_get_time(cclock, &mts);
   mach_port_deallocate(mach_task_self(), cclock);
   ts.tv_sec = mts.tv_sec;
   ts.tv_nsec = mts.tv_nsec;
#elif defined(BSD)
   clock_gettime(CLOCK_REALTIME_PRECISE, &ts);
#elif defined(LINUX)
   clock_gettime(CLOCK_MONOTONIC_RAW, &ts);             // NOTE: CLOCK_PROCESS_CPUTIME_ID gets %sys only
#endif
   return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}
#endif // High-resolution clock

void
report(int total)
{
   int i, t_equals_s;
   static int totaled = FALSE;			// One-pass tripwire
   unsigned long s_rw_locks;
   long long ns_now;

   if (total && totaled) return;
   timestamp(0);
   ns_now = gethrtime();
   s_rw_locks = 0;
   t_equals_s = TRUE;

   for (i=0; Ops[i].index; i++)	{			// Accumulate subtotals ....
      if (Ops[i].s_iter) {
         Ops[i].t_iter += Ops[i].s_iter;
         Ops[i].t_errs += Ops[i].s_errs;
         Ops[i].t_ns += Ops[i].s_ns;
         if (i >= 4 && i <= 7)			// -r -w -rw -rr cases
            s_rw_locks += (Ops[i].s_iter - Ops[i].s_errs);
         if (t_equals_s && (Ops[i].t_iter != Ops[i].s_iter)) t_equals_s = FALSE;
      }
   }
   T_RW_Locks += s_rw_locks;

   if (!total || (total && !t_equals_s)) {		// Per-op sub-totals ...
      printf("<<< Interval Subtotal >>>\n");
      for (i=0; Ops[i].index; i++) {
         if (Ops[i].s_iter) {
            printf("%10s = %lu (%lu tries, %lu errs, avlat=%5.3f ms)\n",
               Ops[i].desc, (Ops[i].s_iter - Ops[i].s_errs), Ops[i].s_iter, Ops[i].s_errs,
               (Ops[i].s_ns*1./Ops[i].s_iter)/1000000.);
         }
         Ops[i].s_iter = Ops[i].s_errs = Ops[i].s_ns = 0;	// Reset subtotals
      }
      printf(" locks/sec = %3.1f\n", s_rw_locks/((ns_now - NS_LastReport)/1000000000.));
      NS_LastReport = ns_now;
   }

   if (total) {						// Per-op totals ...
      printf("<<< Grand Totals >>>\n");
      for (i=0; Ops[i].index; i++) {
         if (Ops[i].t_iter) {
            printf("%10s = %lu (%lu tries, %lu errs, avlat=%5.3f ms)\n",
               Ops[i].desc, (Ops[i].t_iter - Ops[i].t_errs), Ops[i].t_iter, Ops[i].t_errs,
               (Ops[i].t_ns*1./Ops[i].t_iter)/1000000.);
         }
      }
      printf(" locks/sec = %3.1f\n", T_RW_Locks/((ns_now - NS_StartTime)/1000000000.));
      printf("   elapsed = %8.6f sec\n", (ns_now - NS_StartTime)/1000000000.);
      totaled = TRUE;
   }
}

// struct flock {
//    off_t       l_start;    /* starting offset */
//    off_t       l_len;      /* len = 0 means until end of file */
//    pid_t       l_pid;      /* lock owner */
//    short       l_type;     /* lock type: read/write, etc. */
//    short       l_whence;   /* type of l_start */
// };

int
main(int argc, char **argv)
{
   int arg, fd, ourpid;
   int i, opinx, rc;
   unsigned long reps;
   long long ns_prev, ns_now;
   // Shared lock struct ...
   struct flock LockS;
   int LockS_cmd;

   // Process parameters .......................................................
   for (arg=1; arg < argc; arg++) {
      if (argv[arg][0] != '-') break;
      if (argv[arg][1] == 'n') {
         N_REPS = atol(argv[arg]+2);
         if (N_REPS > 0) N_REPORT = N_REPS;
         continue;
      } else if (argv[arg][1] == 'i') {
         N_REPORT = atol(argv[arg]+2);
         if (N_REPORT < 1) usage();
         continue;
      }
      for (i=0; Ops[i].cmd; i++) {
         if (strcmp(argv[arg], Ops[i].cmd) == 0) {
            OpT[arg-1] = &Ops[i];
            OpT[arg] = NULL;
            break;
         }
      }
      if (!Ops[i].cmd) {
         printf("Invalid arg (\"%s\")\n", argv[arg]);
         usage();
      }
   }
   if (arg < 2) { printf("No <op> values specified!\n"); usage(); }
   if (arg > (argc-1)) { printf("No lockfile specified!\n"); usage(); }
   if ((fd = open(argv[arg], O_RDWR, 0)) < 0) {
      printf("Cannot open (\"%s\") for read/write access!\n", argv[arg]);
     usage();
   }

   // Report parameters ........................................................
   timestamp(1);
   if (N_REPS > 0) printf("<<< %lu iterations with report per %lu iterations >>>\n", N_REPS, N_REPORT);
   else            printf("<<< INFINITE iterations with report per %lu iterations >>>\n", N_REPORT);
   for (opinx=0; OpT[opinx]; opinx++)			// Scan OpTable ...
      printf("%10s = %s\n", OpT[opinx]->cmd, OpT[opinx]->desc);

   NS_LastReport = NS_StartTime = gethrtime();	// START elapsed and report interval counters...

   // Main loop ................................................................
   ourpid = getpid();
   ns_prev = gethrtime();
   reps = 0;
   while (1) {					// Breaks out at end of loop ...
      for (opinx=0; OpT[opinx]; opinx++) {
         rc = 0;
         bzero(&LockS, sizeof(LockS));
         LockS.l_whence = SEEK_SET;
         switch (OpT[opinx]->index) {
         case 1: // "-tw", "TestW", 0, 0, 0LL,
            LockS.l_type = F_WRLCK;
            LockS.l_pid = ourpid;
            LockS_cmd = F_GETLK;
            break;
         case 2: // "-tr", "TestR", 0, 0, 0LL,
            LockS.l_type = F_RDLCK;
            LockS.l_pid = ourpid;
            LockS_cmd = F_GETLK;
            break;
         case 3: // "-ts", "TestS", 0, 0, 0LL,
            LockS.l_type = F_WRLCK;
            LockS.l_pid = ourpid;
            LockS_cmd = F_GETLK;
            break;
         case 4: // "-rw", "RLockWait", 0, 0, 0LL,
            LockS.l_type = F_RDLCK;
            LockS_cmd = F_SETLKW;
            break;
         case 5: // "-ww", "WLockWait", 0, 0, 0LL,
            LockS.l_type = F_WRLCK;
            LockS_cmd = F_SETLKW;
            break;
         case 6: // "-r" "Rlock", 0, 0, 0LL,
            LockS.l_type = F_RDLCK;
            LockS_cmd = F_SETLK;
            break;
         case 7: // "-w" "Wlock", 0, 0, 0LL,
            LockS.l_type = F_WRLCK;
            LockS_cmd = F_SETLK;
            break;
         case 8: // "-u" "Unlock", 0, 0, 0LL,
            LockS.l_type = F_UNLCK;
            LockS_cmd = F_SETLK;
            break;
         }
         rc = fcntl(fd, LockS_cmd, &LockS);	// Do op ...
         OpT[opinx]->s_iter++;
         ns_now = gethrtime();
         OpT[opinx]->s_ns += (ns_now - ns_prev);
         ns_prev = ns_now;
         if (rc == 0 && LockS_cmd == F_GETLK) {	// Adjust results ...
            if (opinx == 3 && LockS.l_type == F_WRLCK) rc = -1; // TestS
            else if (LockS.l_type != F_UNLCK) rc = -1;		// TestR, TestW
         }
         if (rc == -1) {
            OpT[opinx]->s_errs++;
            break;				// Skips to next iteration ...
         }
      }
      reps++;
      if (((reps + N_REPORT) % N_REPORT) == 0) report(reps == N_REPS);	// Periodic report (subtotals)
      if (reps == N_REPS) break;					// Never if (N_REPS == 0) ...
   }

   // Wrapup ...................................................................
   report(1);							// Final report (totals)

   return(0);
}
