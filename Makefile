# automatically generated makefile by ./scripts/create-make

CFLAGS=-Wall -lpthread -g


all: processes/fork-write-twice processes/daemonize processes/fork-wait processes/daemonize-with-pipe processes/extract processes/transmit-via-pipe io/tty-read io/stdio-stat io/lowlevel-read-write io/stdin-read io/reading-deleted-file io/shared-locking io/highlevel-io io/advisory-locking-blocking io/file-stat io/lowlevel-io io/lockf-file-part1 io/reading-dir-short io/lockf-file-part2 io/abc-lowlevel io/not-reading-dir io/advisory-locking io/failing-io io/reading-dir io/password-test io/abc-highlevel threads/thread_return threads/thread-id threads/condition threads/timedlock threads/threadf threads/thread threads/thread_write_once threads/exit-pthread-exit threads/nonblocking threads/mutex-reentrance threads/mutex-order threads/deadlock threads/barriere ipc/shared-memory ipc/signal ipc/msgqueue-positive-typed ipc/daemonize-group-signal-sigaction ipc/semaphore ipc/msgqueue-simple ipc/shared-memory-count-simple ipc/semaphore-simple ipc/daemonize-group-signal ipc/daemonize-signal-and-pipe ipc/msgqueue ipc/msgqueue-hello ipc/shared-memory-count-output-semaphore ipc/msgqueue-negative-typed ipc/shared-memory-count

clean:
	rm processes/fork-write-twice processes/daemonize processes/fork-wait processes/daemonize-with-pipe processes/extract processes/transmit-via-pipe io/tty-read io/stdio-stat io/lowlevel-read-write io/stdin-read io/reading-deleted-file io/shared-locking io/highlevel-io io/advisory-locking-blocking io/file-stat io/lowlevel-io io/lockf-file-part1 io/reading-dir-short io/lockf-file-part2 io/abc-lowlevel io/not-reading-dir io/advisory-locking io/failing-io io/reading-dir io/password-test io/abc-highlevel threads/thread_return threads/thread-id threads/condition threads/timedlock threads/threadf threads/thread threads/thread_write_once threads/exit-pthread-exit threads/nonblocking threads/mutex-reentrance threads/mutex-order threads/deadlock threads/barriere ipc/shared-memory ipc/signal ipc/msgqueue-positive-typed ipc/daemonize-group-signal-sigaction ipc/semaphore ipc/msgqueue-simple ipc/shared-memory-count-simple ipc/semaphore-simple ipc/daemonize-group-signal ipc/daemonize-signal-and-pipe ipc/msgqueue ipc/msgqueue-hello ipc/shared-memory-count-output-semaphore ipc/msgqueue-negative-typed ipc/shared-memory-count

processes/fork-write-twice: processes/fork-write-twice.c
	gcc $(CFLAGS) processes/fork-write-twice.c  -o processes/fork-write-twice

processes/daemonize: processes/daemonize.c
	gcc $(CFLAGS) processes/daemonize.c  -o processes/daemonize

processes/fork-wait: processes/fork-wait.c
	gcc $(CFLAGS) processes/fork-wait.c  -o processes/fork-wait

processes/daemonize-with-pipe: processes/daemonize-with-pipe.c
	gcc $(CFLAGS) processes/daemonize-with-pipe.c  -o processes/daemonize-with-pipe

processes/extract: processes/extract.c
	gcc $(CFLAGS) processes/extract.c  -o processes/extract

processes/transmit-via-pipe: processes/transmit-via-pipe.c
	gcc $(CFLAGS) processes/transmit-via-pipe.c  -o processes/transmit-via-pipe

io/tty-read: io/tty-read.c
	gcc $(CFLAGS) io/tty-read.c  -o io/tty-read

io/stdio-stat: io/stdio-stat.c
	gcc $(CFLAGS) io/stdio-stat.c  -o io/stdio-stat

io/lowlevel-read-write: io/lowlevel-read-write.c
	gcc $(CFLAGS) io/lowlevel-read-write.c  -o io/lowlevel-read-write

io/stdin-read: io/stdin-read.c
	gcc $(CFLAGS) io/stdin-read.c  -o io/stdin-read

io/reading-deleted-file: io/reading-deleted-file.c
	gcc $(CFLAGS) io/reading-deleted-file.c  -o io/reading-deleted-file

io/shared-locking: io/shared-locking.c
	gcc $(CFLAGS) io/shared-locking.c  -o io/shared-locking

io/highlevel-io: io/highlevel-io.c
	gcc $(CFLAGS) io/highlevel-io.c  -o io/highlevel-io

io/advisory-locking-blocking: io/advisory-locking-blocking.c
	gcc $(CFLAGS) io/advisory-locking-blocking.c  -o io/advisory-locking-blocking

io/file-stat: io/file-stat.c
	gcc $(CFLAGS) io/file-stat.c  -o io/file-stat

io/lowlevel-io: io/lowlevel-io.c
	gcc $(CFLAGS) io/lowlevel-io.c  -o io/lowlevel-io

io/lockf-file-part1: io/lockf-file-part1.c
	gcc $(CFLAGS) io/lockf-file-part1.c  -o io/lockf-file-part1

io/reading-dir-short: io/reading-dir-short.c
	gcc $(CFLAGS) io/reading-dir-short.c  -o io/reading-dir-short

io/lockf-file-part2: io/lockf-file-part2.c
	gcc $(CFLAGS) io/lockf-file-part2.c  -o io/lockf-file-part2

io/abc-lowlevel: io/abc-lowlevel.c
	gcc $(CFLAGS) io/abc-lowlevel.c  -o io/abc-lowlevel

io/not-reading-dir: io/not-reading-dir.c
	gcc $(CFLAGS) io/not-reading-dir.c  -o io/not-reading-dir

io/advisory-locking: io/advisory-locking.c
	gcc $(CFLAGS) io/advisory-locking.c  -o io/advisory-locking

io/failing-io: io/failing-io.c
	gcc $(CFLAGS) io/failing-io.c  -o io/failing-io

io/reading-dir: io/reading-dir.c
	gcc $(CFLAGS) io/reading-dir.c  -o io/reading-dir

io/password-test: io/password-test.c
	gcc $(CFLAGS) io/password-test.c  -o io/password-test

io/abc-highlevel: io/abc-highlevel.c
	gcc $(CFLAGS) io/abc-highlevel.c  -o io/abc-highlevel

threads/thread_return: threads/thread_return.c
	gcc $(CFLAGS) threads/thread_return.c  -pthread -o threads/thread_return

threads/thread-id: threads/thread-id.c
	gcc $(CFLAGS) threads/thread-id.c  -pthread -o threads/thread-id

threads/condition: threads/condition.c
	gcc $(CFLAGS) threads/condition.c  -pthread -o threads/condition

threads/timedlock: threads/timedlock.c
	gcc $(CFLAGS) threads/timedlock.c  -pthread -o threads/timedlock

threads/threadf: threads/threadf.c
	gcc $(CFLAGS) threads/threadf.c  -pthread -o threads/threadf

threads/thread: threads/thread.c
	gcc $(CFLAGS) threads/thread.c  -pthread -o threads/thread

threads/thread_write_once: threads/thread_write_once.c
	gcc $(CFLAGS) threads/thread_write_once.c  -pthread -o threads/thread_write_once

threads/exit-pthread-exit: threads/exit-pthread-exit.c
	gcc $(CFLAGS) threads/exit-pthread-exit.c  -pthread -o threads/exit-pthread-exit

threads/nonblocking: threads/nonblocking.c
	gcc $(CFLAGS) threads/nonblocking.c  -pthread -o threads/nonblocking

threads/mutex-reentrance: threads/mutex-reentrance.c
	gcc $(CFLAGS) threads/mutex-reentrance.c  -pthread -o threads/mutex-reentrance

threads/mutex-order: threads/mutex-order.c
	gcc $(CFLAGS) threads/mutex-order.c  -pthread -o threads/mutex-order

threads/deadlock: threads/deadlock.c
	gcc $(CFLAGS) threads/deadlock.c  -pthread -o threads/deadlock

threads/barriere: threads/barriere.c
	gcc $(CFLAGS) threads/barriere.c  -pthread -o threads/barriere

ipc/shared-memory: ipc/shared-memory.c
	gcc $(CFLAGS) ipc/shared-memory.c  -o ipc/shared-memory

ipc/signal: ipc/signal.c
	gcc $(CFLAGS) ipc/signal.c  -o ipc/signal

ipc/msgqueue-positive-typed: ipc/msgqueue-positive-typed.c
	gcc $(CFLAGS) ipc/msgqueue-positive-typed.c  -o ipc/msgqueue-positive-typed

ipc/daemonize-group-signal-sigaction: ipc/daemonize-group-signal-sigaction.c
	gcc $(CFLAGS) ipc/daemonize-group-signal-sigaction.c  -o ipc/daemonize-group-signal-sigaction

ipc/semaphore: ipc/semaphore.c
	gcc $(CFLAGS) ipc/semaphore.c  -o ipc/semaphore

ipc/msgqueue-simple: ipc/msgqueue-simple.c
	gcc $(CFLAGS) ipc/msgqueue-simple.c  -o ipc/msgqueue-simple

ipc/shared-memory-count-simple: ipc/shared-memory-count-simple.c
	gcc $(CFLAGS) ipc/shared-memory-count-simple.c  -o ipc/shared-memory-count-simple

ipc/semaphore-simple: ipc/semaphore-simple.c
	gcc $(CFLAGS) ipc/semaphore-simple.c  -o ipc/semaphore-simple

ipc/daemonize-group-signal: ipc/daemonize-group-signal.c
	gcc $(CFLAGS) ipc/daemonize-group-signal.c  -o ipc/daemonize-group-signal

ipc/daemonize-signal-and-pipe: ipc/daemonize-signal-and-pipe.c
	gcc $(CFLAGS) ipc/daemonize-signal-and-pipe.c  -o ipc/daemonize-signal-and-pipe

ipc/msgqueue: ipc/msgqueue.c
	gcc $(CFLAGS) ipc/msgqueue.c  -o ipc/msgqueue

ipc/msgqueue-hello: ipc/msgqueue-hello.c
	gcc $(CFLAGS) ipc/msgqueue-hello.c  -o ipc/msgqueue-hello

ipc/shared-memory-count-output-semaphore: ipc/shared-memory-count-output-semaphore.c
	gcc $(CFLAGS) ipc/shared-memory-count-output-semaphore.c  -o ipc/shared-memory-count-output-semaphore

ipc/msgqueue-negative-typed: ipc/msgqueue-negative-typed.c
	gcc $(CFLAGS) ipc/msgqueue-negative-typed.c  -o ipc/msgqueue-negative-typed

ipc/shared-memory-count: ipc/shared-memory-count.c
	gcc $(CFLAGS) ipc/shared-memory-count.c  -o ipc/shared-memory-count


