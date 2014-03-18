# automatically generated makefile by ./scripts/create-make

CFLAGS=-Wall -g -std=gnu99 -I./include -L./lib

LIBS=-lpthread lib/libitsky.a


all: lib/libitsky.a processes/fork-write-twice processes/daemonize processes/fork-wait processes/daemonize-with-pipe processes/extract processes/transmit-via-pipe io/tty-read io/stdio-stat io/lowlevel-read-write io/stdin-read io/reading-deleted-file io/shared-locking io/highlevel-io io/advisory-locking-blocking io/file-stat io/lowlevel-io io/lockf-file-part1 io/reading-dir-short io/lockf-file-part2 io/abc-lowlevel io/not-reading-dir io/advisory-locking io/failing-io io/reading-dir io/password-test io/abc-highlevel threads/thread_return threads/thread-param-trap threads/thread-id threads/condition threads/timedlock threads/threadf threads/thread threads/thread_write_once threads/exit-pthread-exit threads/nonblocking threads/mutex-reentrance threads/mutex-order threads/deadlock threads/barriere ipc/shared-memory ipc/signal ipc/msgqueue-positive-typed ipc/daemonize-group-signal-sigaction ipc/semaphore ipc/msgqueue-simple ipc/shared-memory-count-simple ipc/semaphore-simple ipc/daemonize-group-signal ipc/daemonize-signal-and-pipe ipc/msgqueue ipc/msgqueue-hello ipc/shared-memory-count-output-semaphore ipc/msgqueue-negative-typed ipc/shared-memory-count

clean:
	rm -f lib/libitsky.a processes/fork-write-twice processes/daemonize processes/fork-wait processes/daemonize-with-pipe processes/extract processes/transmit-via-pipe io/tty-read io/stdio-stat io/lowlevel-read-write io/stdin-read io/reading-deleted-file io/shared-locking io/highlevel-io io/advisory-locking-blocking io/file-stat io/lowlevel-io io/lockf-file-part1 io/reading-dir-short io/lockf-file-part2 io/abc-lowlevel io/not-reading-dir io/advisory-locking io/failing-io io/reading-dir io/password-test io/abc-highlevel threads/thread_return threads/thread-param-trap threads/thread-id threads/condition threads/timedlock threads/threadf threads/thread threads/thread_write_once threads/exit-pthread-exit threads/nonblocking threads/mutex-reentrance threads/mutex-order threads/deadlock threads/barriere ipc/shared-memory ipc/signal ipc/msgqueue-positive-typed ipc/daemonize-group-signal-sigaction ipc/semaphore ipc/msgqueue-simple ipc/shared-memory-count-simple ipc/semaphore-simple ipc/daemonize-group-signal ipc/daemonize-signal-and-pipe ipc/msgqueue ipc/msgqueue-hello ipc/shared-memory-count-output-semaphore ipc/msgqueue-negative-typed ipc/shared-memory-count

lib/libitsky.a: lib/lib.c
	gcc -c $(CFLAGS) lib/lib.c -o lib/lib.o
	ar cr lib/libitsky.a lib/lib.o
	ranlib lib/libitsky.a

processes/fork-write-twice: processes/fork-write-twice.c
	gcc $(CFLAGS) $(LIBS) processes/fork-write-twice.c -o processes/fork-write-twice

processes/daemonize: processes/daemonize.c
	gcc $(CFLAGS) $(LIBS) processes/daemonize.c -o processes/daemonize

processes/fork-wait: processes/fork-wait.c
	gcc $(CFLAGS) $(LIBS) processes/fork-wait.c -o processes/fork-wait

processes/daemonize-with-pipe: processes/daemonize-with-pipe.c
	gcc $(CFLAGS) $(LIBS) processes/daemonize-with-pipe.c -o processes/daemonize-with-pipe

processes/extract: processes/extract.c
	gcc $(CFLAGS) $(LIBS) processes/extract.c -o processes/extract

processes/transmit-via-pipe: processes/transmit-via-pipe.c
	gcc $(CFLAGS) $(LIBS) processes/transmit-via-pipe.c -o processes/transmit-via-pipe

io/tty-read: io/tty-read.c
	gcc $(CFLAGS) $(LIBS) io/tty-read.c -o io/tty-read

io/stdio-stat: io/stdio-stat.c
	gcc $(CFLAGS) $(LIBS) io/stdio-stat.c -o io/stdio-stat

io/lowlevel-read-write: io/lowlevel-read-write.c
	gcc $(CFLAGS) $(LIBS) io/lowlevel-read-write.c -o io/lowlevel-read-write

io/stdin-read: io/stdin-read.c
	gcc $(CFLAGS) $(LIBS) io/stdin-read.c -o io/stdin-read

io/reading-deleted-file: io/reading-deleted-file.c
	gcc $(CFLAGS) $(LIBS) io/reading-deleted-file.c -o io/reading-deleted-file

io/shared-locking: io/shared-locking.c
	gcc $(CFLAGS) $(LIBS) io/shared-locking.c -o io/shared-locking

io/highlevel-io: io/highlevel-io.c
	gcc $(CFLAGS) $(LIBS) io/highlevel-io.c -o io/highlevel-io

io/advisory-locking-blocking: io/advisory-locking-blocking.c
	gcc $(CFLAGS) $(LIBS) io/advisory-locking-blocking.c -o io/advisory-locking-blocking

io/file-stat: io/file-stat.c
	gcc $(CFLAGS) $(LIBS) io/file-stat.c -o io/file-stat

io/lowlevel-io: io/lowlevel-io.c
	gcc $(CFLAGS) $(LIBS) io/lowlevel-io.c -o io/lowlevel-io

io/lockf-file-part1: io/lockf-file-part1.c
	gcc $(CFLAGS) $(LIBS) io/lockf-file-part1.c -o io/lockf-file-part1

io/reading-dir-short: io/reading-dir-short.c
	gcc $(CFLAGS) $(LIBS) io/reading-dir-short.c -o io/reading-dir-short

io/lockf-file-part2: io/lockf-file-part2.c
	gcc $(CFLAGS) $(LIBS) io/lockf-file-part2.c -o io/lockf-file-part2

io/abc-lowlevel: io/abc-lowlevel.c
	gcc $(CFLAGS) $(LIBS) io/abc-lowlevel.c -o io/abc-lowlevel

io/not-reading-dir: io/not-reading-dir.c
	gcc $(CFLAGS) $(LIBS) io/not-reading-dir.c -o io/not-reading-dir

io/advisory-locking: io/advisory-locking.c
	gcc $(CFLAGS) $(LIBS) io/advisory-locking.c -o io/advisory-locking

io/failing-io: io/failing-io.c
	gcc $(CFLAGS) $(LIBS) io/failing-io.c -o io/failing-io

io/reading-dir: io/reading-dir.c
	gcc $(CFLAGS) $(LIBS) io/reading-dir.c -o io/reading-dir

io/password-test: io/password-test.c
	gcc $(CFLAGS) $(LIBS) io/password-test.c -o io/password-test

io/abc-highlevel: io/abc-highlevel.c
	gcc $(CFLAGS) $(LIBS) io/abc-highlevel.c -o io/abc-highlevel

threads/thread_return: threads/thread_return.c
	gcc $(CFLAGS) $(LIBS) threads/thread_return.c -o threads/thread_return

threads/thread-param-trap: threads/thread-param-trap.c
	gcc $(CFLAGS) $(LIBS) threads/thread-param-trap.c -o threads/thread-param-trap

threads/thread-id: threads/thread-id.c
	gcc $(CFLAGS) $(LIBS) threads/thread-id.c -o threads/thread-id

threads/condition: threads/condition.c
	gcc $(CFLAGS) $(LIBS) threads/condition.c -o threads/condition

threads/timedlock: threads/timedlock.c
	gcc $(CFLAGS) $(LIBS) threads/timedlock.c -o threads/timedlock

threads/threadf: threads/threadf.c
	gcc $(CFLAGS) $(LIBS) threads/threadf.c -o threads/threadf

threads/thread: threads/thread.c
	gcc $(CFLAGS) $(LIBS) threads/thread.c -o threads/thread

threads/thread_write_once: threads/thread_write_once.c
	gcc $(CFLAGS) $(LIBS) threads/thread_write_once.c -o threads/thread_write_once

threads/exit-pthread-exit: threads/exit-pthread-exit.c
	gcc $(CFLAGS) $(LIBS) threads/exit-pthread-exit.c -o threads/exit-pthread-exit

threads/nonblocking: threads/nonblocking.c
	gcc $(CFLAGS) $(LIBS) threads/nonblocking.c -o threads/nonblocking

threads/mutex-reentrance: threads/mutex-reentrance.c
	gcc $(CFLAGS) $(LIBS) threads/mutex-reentrance.c -o threads/mutex-reentrance

threads/mutex-order: threads/mutex-order.c
	gcc $(CFLAGS) $(LIBS) threads/mutex-order.c -o threads/mutex-order

threads/deadlock: threads/deadlock.c
	gcc $(CFLAGS) $(LIBS) threads/deadlock.c -o threads/deadlock

threads/barriere: threads/barriere.c
	gcc $(CFLAGS) $(LIBS) threads/barriere.c -o threads/barriere

ipc/shared-memory: ipc/shared-memory.c
	gcc $(CFLAGS) $(LIBS) ipc/shared-memory.c -o ipc/shared-memory

ipc/signal: ipc/signal.c
	gcc $(CFLAGS) $(LIBS) ipc/signal.c -o ipc/signal

ipc/msgqueue-positive-typed: ipc/msgqueue-positive-typed.c
	gcc $(CFLAGS) $(LIBS) ipc/msgqueue-positive-typed.c -o ipc/msgqueue-positive-typed

ipc/daemonize-group-signal-sigaction: ipc/daemonize-group-signal-sigaction.c
	gcc $(CFLAGS) $(LIBS) ipc/daemonize-group-signal-sigaction.c -o ipc/daemonize-group-signal-sigaction

ipc/semaphore: ipc/semaphore.c
	gcc $(CFLAGS) $(LIBS) ipc/semaphore.c -o ipc/semaphore

ipc/msgqueue-simple: ipc/msgqueue-simple.c
	gcc $(CFLAGS) $(LIBS) ipc/msgqueue-simple.c -o ipc/msgqueue-simple

ipc/shared-memory-count-simple: ipc/shared-memory-count-simple.c
	gcc $(CFLAGS) $(LIBS) ipc/shared-memory-count-simple.c -o ipc/shared-memory-count-simple

ipc/semaphore-simple: ipc/semaphore-simple.c
	gcc $(CFLAGS) $(LIBS) ipc/semaphore-simple.c -o ipc/semaphore-simple

ipc/daemonize-group-signal: ipc/daemonize-group-signal.c
	gcc $(CFLAGS) $(LIBS) ipc/daemonize-group-signal.c -o ipc/daemonize-group-signal

ipc/daemonize-signal-and-pipe: ipc/daemonize-signal-and-pipe.c
	gcc $(CFLAGS) $(LIBS) ipc/daemonize-signal-and-pipe.c -o ipc/daemonize-signal-and-pipe

ipc/msgqueue: ipc/msgqueue.c
	gcc $(CFLAGS) $(LIBS) ipc/msgqueue.c -o ipc/msgqueue

ipc/msgqueue-hello: ipc/msgqueue-hello.c
	gcc $(CFLAGS) $(LIBS) ipc/msgqueue-hello.c -o ipc/msgqueue-hello

ipc/shared-memory-count-output-semaphore: ipc/shared-memory-count-output-semaphore.c
	gcc $(CFLAGS) $(LIBS) ipc/shared-memory-count-output-semaphore.c -o ipc/shared-memory-count-output-semaphore

ipc/msgqueue-negative-typed: ipc/msgqueue-negative-typed.c
	gcc $(CFLAGS) $(LIBS) ipc/msgqueue-negative-typed.c -o ipc/msgqueue-negative-typed

ipc/shared-memory-count: ipc/shared-memory-count.c
	gcc $(CFLAGS) $(LIBS) ipc/shared-memory-count.c -o ipc/shared-memory-count


