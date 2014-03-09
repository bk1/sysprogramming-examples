# automatically generated makefile by scripts/create-make


all: processes/fork-write-twice processes/daemonize processes/fork-wait processes/daemonize-with-pipe processes/extract processes/transmit-via-pipe io/tty-read io/stdio-stat io/lowlevel-read-write io/stdin-read io/reading-deleted-file io/shared-locking io/highlevel-io io/advisory-locking-blocking io/file-stat io/lowlevel-io io/lockf-file-part1 io/reading-dir-short io/lockf-file-part2 io/abc-lowlevel io/not-reading-dir io/advisory-locking io/failing-io io/reading-dir io/password-test io/abc-highlevel threads/threadf threads/thread ipc/signal ipc/msgqueue

processes/fork-write-twice: processes/fork-write-twice.c
	gcc -Wall processes/fork-write-twice.c  -o processes/fork-write-twice

processes/daemonize: processes/daemonize.c
	gcc -Wall processes/daemonize.c  -o processes/daemonize

processes/fork-wait: processes/fork-wait.c
	gcc -Wall processes/fork-wait.c  -o processes/fork-wait

processes/daemonize-with-pipe: processes/daemonize-with-pipe.c
	gcc -Wall processes/daemonize-with-pipe.c  -o processes/daemonize-with-pipe

processes/extract: processes/extract.c
	gcc -Wall processes/extract.c  -o processes/extract

processes/transmit-via-pipe: processes/transmit-via-pipe.c
	gcc -Wall processes/transmit-via-pipe.c  -o processes/transmit-via-pipe

io/tty-read: io/tty-read.c
	gcc -Wall io/tty-read.c  -o io/tty-read

io/stdio-stat: io/stdio-stat.c
	gcc -Wall io/stdio-stat.c  -o io/stdio-stat

io/lowlevel-read-write: io/lowlevel-read-write.c
	gcc -Wall io/lowlevel-read-write.c  -o io/lowlevel-read-write

io/stdin-read: io/stdin-read.c
	gcc -Wall io/stdin-read.c  -o io/stdin-read

io/reading-deleted-file: io/reading-deleted-file.c
	gcc -Wall io/reading-deleted-file.c  -o io/reading-deleted-file

io/shared-locking: io/shared-locking.c
	gcc -Wall io/shared-locking.c  -o io/shared-locking

io/highlevel-io: io/highlevel-io.c
	gcc -Wall io/highlevel-io.c  -o io/highlevel-io

io/advisory-locking-blocking: io/advisory-locking-blocking.c
	gcc -Wall io/advisory-locking-blocking.c  -o io/advisory-locking-blocking

io/file-stat: io/file-stat.c
	gcc -Wall io/file-stat.c  -o io/file-stat

io/lowlevel-io: io/lowlevel-io.c
	gcc -Wall io/lowlevel-io.c  -o io/lowlevel-io

io/lockf-file-part1: io/lockf-file-part1.c
	gcc -Wall io/lockf-file-part1.c  -o io/lockf-file-part1

io/reading-dir-short: io/reading-dir-short.c
	gcc -Wall io/reading-dir-short.c  -o io/reading-dir-short

io/lockf-file-part2: io/lockf-file-part2.c
	gcc -Wall io/lockf-file-part2.c  -o io/lockf-file-part2

io/abc-lowlevel: io/abc-lowlevel.c
	gcc -Wall io/abc-lowlevel.c  -o io/abc-lowlevel

io/not-reading-dir: io/not-reading-dir.c
	gcc -Wall io/not-reading-dir.c  -o io/not-reading-dir

io/advisory-locking: io/advisory-locking.c
	gcc -Wall io/advisory-locking.c  -o io/advisory-locking

io/failing-io: io/failing-io.c
	gcc -Wall io/failing-io.c  -o io/failing-io

io/reading-dir: io/reading-dir.c
	gcc -Wall io/reading-dir.c  -o io/reading-dir

io/password-test: io/password-test.c
	gcc -Wall io/password-test.c  -o io/password-test

io/abc-highlevel: io/abc-highlevel.c
	gcc -Wall io/abc-highlevel.c  -o io/abc-highlevel

threads/threadf: threads/threadf.c
	gcc -Wall threads/threadf.c  -pthread -o threads/threadf

threads/thread: threads/thread.c
	gcc -Wall threads/thread.c  -pthread -o threads/thread

ipc/signal: ipc/signal.c
	gcc -Wall ipc/signal.c  -o ipc/signal

ipc/msgqueue: ipc/msgqueue.c
	gcc -Wall ipc/msgqueue.c  -o ipc/msgqueue


