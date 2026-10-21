*This activity has been created as part of the 42 curriculum by ccaro.*

# Codexion

## Description

Codexion is a concurrency simulation built on the classic *dining
philosophers* problem. `number_of_coders` coder threads sit around a
circular table of shared USB dongles. Compiling requires holding both
neighboring dongles at once (the coder's left and right dongle), so two
adjacent coders can never compile at the same time, but non-adjacent ones
can.

Each coder repeatedly cycles through four states: **waiting** for its two
dongles, **compiling**, **debugging**, and **refactoring**. A separate
**monitor** thread continuously watches every coder's deadline and stops
the whole simulation as soon as either:
- a coder **burns out** (fails to start a new compile within
  `time_to_burnout` milliseconds of its last one), or
- every coder has reached `number_of_compiles_required` successful
  compiles.

Dongle access is arbitrated by a hand-written scheduler backed by a
hand-written binary heap (no library priority queue), selectable between
two policies:
- **FIFO** — the coder that has been waiting longest for a given dongle
  is served first.
- **EDF** (Earliest Deadline First) — the coder whose burnout deadline is
  closest is served first.

The goal of the activity is to practice POSIX threads synchronization
(mutexes, condition variables) on a non-trivial shared-resource problem,
and to reason concretely — not just in theory — about deadlock, Coffman's
conditions, starvation, and the practical difference between two classic
scheduling policies.

## Architecture

### Directory structure

```
Codexion/
├── Makefile
├── README.md
├── includes/
│   └── codexion.h
└── src/
    ├── main.c             (setup, run)
    ├── parsing/
    │   ├── parse_args.c
    │   └── parse_utils.c
    ├── coder/
    │   ├── coder_init.c   (coder table + thread creation)
    │   ├── routine.c      (thread entry point, startup barrier)
    │   └── actions.c      (take_dongle / release_dongle)
    ├── cleanup/
    │   └── cleanup.c      (all teardown: heap, dongles, simulation)
    ├── dongle/
    │   └── dongle_init.c  (dongle table)
    ├── scheduler/
    │   ├── heap.c / heap_init.c  (generic binary heap)
    │   ├── fifo.c / edf.c        (ordering keys + EDF tie-break)
    │   └── ready.c               (readiness check, registration)
    ├── monitor/
    │   └── monitoring.c   (burnout / completion detection thread)
    ├── logger/
    │   └── log.c          (serialized output)
    └── utils/
        └── utils.c        (timestamp helpers)
```

`src/cleanup/` mirrors the other single-responsibility directories
(`scheduler/`, `monitor/`, `logger/`, `utils/`): every destructor in the
project lives there — `destroy_heap`, `destroy_dongle_table`, and the
top-level `cleanup_simulation` that calls the former and frees/destroys
everything else — rather than being split across the modules that
allocated each resource.

### Design rationale

- **No global variables.** Everything the threads need — configuration,
  the coder table, the scheduling mutex/condition variable, the log
  mutex, the stop flag — lives in one `t_simulation` structure, allocated
  in `main` and reached by every thread only through the `t_coder *`/
  `t_simulation *` pointer it was handed at creation.
- **`dongle/` is separated from `coder/`.** A dongle owns its own state
  (`is_taken`, `available_at`, its own `mutex` and wait-queue heap). A
  coder is only a *consumer* of two dongles it points to. Keeping the
  resource's synchronization state away from the coder's business logic
  (compile → debug → refactor) keeps each side easy to reason about
  independently.
- **`scheduler/` isolates the priority queue from the policy.** `heap.c`/
  `heap_init.c` implement a fully generic binary heap that only ever
  compares a `long key` — it has no notion of "deadline" or "arrival
  order". `fifo_key`/`edf_key` are the only places that decide what that
  key *means*. This is what makes the live FIFO → LIFO demonstration a
  one-line change (flip the sign in `fifo_key`) instead of a rewrite.
- **`monitor/` is a dedicated thread**, independent of every coder's own
  logic, whose only job is to detect the two stopping conditions
  (burnout, or everyone finished) and raise the shared `stop` flag.
- **`logger/` centralizes the output mutex.** Every state-change message
  goes through one `log_event()` function, so two lines can never
  interleave and the timestamp format stays consistent everywhere.

### Data structures

Defined in `includes/codexion.h`.

| Structure | Role |
|-----------|------|
| `t_config` | The 8 validated command-line arguments, read-only after parsing. |
| `t_dongle` | One shared dongle: `mutex`, `is_taken`, `available_at` (cooldown deadline), and its own `wait_queue` (a capacity-2 heap — a dongle only ever has its two fixed neighbors as potential waiters). |
| `t_simulation` | The single non-global shared state: `config`, the `coders` table, `log_mutex`, `sched_mutex`/`sched_cond` (the scheduling condition variable shared by every coder and the monitor), `stop`, and `started_count` (the startup barrier counter). |
| `t_coder`  | One coder: `id`, `state`, `curr_nb_compil`, `burnout_at`, pointers to its two dongles, and a pointer back to its `t_simulation`. |
| `t_heap` / `t_heap_entry` | A generic array-based binary heap of `{key, coder}` pairs — used identically by FIFO and EDF. |

`available_at` (dongle) and `burnout_at` (coder) are **points in time**
(milliseconds since the simulation's start, measured with
`CLOCK_MONOTONIC`), not durations that count down — "is this dongle in
cooldown?" is simply `now < available_at`, evaluated on demand, with a
single source of truth.

## Instructions

### Compilation

```sh
make        # builds the codexion binary
make clean  # removes object files
make fclean # removes object files and the binary
make re     # fclean + all
```

Compiles with `cc -Wall -Wextra -Werror -pthread`, no warnings.

### Usage

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
    time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument                       | Unit | Meaning                                                                              |
|---------------------------------|------|---------------------------------------------------------------------------------------|
| `number_of_coders`               | —    | Number of coder threads (and dongles).                                               |
| `time_to_burnout`                 | ms   | Time without starting a compile before a coder burns out.                            |
| `time_to_compile`                 | ms   | Duration of the compiling state (the only phase during which a coder holds dongles). |
| `time_to_debug`                    | ms   | Duration of the debugging state.                                                     |
| `time_to_refactor`                 | ms   | Duration of the refactoring state.                                                    |
| `number_of_compiles_required`      | —    | Simulation stops once every coder reached this many compiles.                        |
| `dongle_cooldown`                  | ms   | Time a released dongle stays unavailable before anyone can take it again.            |
| `scheduler`                        | —    | Dongle arbitration policy: `fifo` or `edf`.                                          |

### Example

```sh
./codexion 4 800 200 200 200 3 100 fifo
```

Each output line has the format `<timestamp_ms> <coder_id> <message>`,
`coder_id` numbered from 1.

## Blocking cases handled

### Deadlock prevention (Coffman's conditions)

Compiling needs both dongles at once (*hold and wait*), the classic setup
for deadlock on a circular table. Two specific decisions break it:

- **No circular wait: dongles are always locked in address order.**
  `lock_dongles()` compares the two dongle pointers and always locks the
  lower address first, regardless of which one is the coder's "left" or
  "right" dongle:
  ```c
  void	lock_dongles(t_coder *coder)
  {
  	if (coder->left_dongle < coder->right_dongle)
  	{
  		pthread_mutex_lock(&coder->left_dongle->mutex);
  		pthread_mutex_lock(&coder->right_dongle->mutex);
  	}
  	else
  	{
  		pthread_mutex_lock(&coder->right_dongle->mutex);
  		pthread_mutex_lock(&coder->left_dongle->mutex);
  	}
  }
  ```
  Without this, two neighbors locking "my left, then my right" in
  opposite physical order is exactly A-then-B / B-then-A — a textbook
  deadlock.
- **Dongle mutexes are held only briefly, never for the whole compile.**
  `take_dongle()` locks both dongle mutexes only long enough to flip
  `is_taken` to `true` and pop the coder from each wait-queue, then
  unlocks immediately; the actual compiling happens with no dongle mutex
  held at all. An earlier version kept both dongle mutexes locked for the
  entire compile, which produced an intermittent deadlock between a
  coder's dongle mutex and the simulation-wide `sched_mutex` (confirmed
  with `gdb`, ~1 run in 10) — fixed by shrinking the critical section.

### Starvation prevention

Both scheduling policies use the same generic heap, keyed differently:

- **FIFO**: `key = now * 2 + parity_bit` — the registration timestamp is
  the primary discriminant; `parity_bit` is 0 if the coder's id parity
  matches the target dongle's current `favor_even` flag, 1 otherwise. It
  only ever breaks a *strict* tie on `now` (the `* 2` guarantees a real
  timestamp difference always dominates it).
- **EDF**: the same construction, on `burnout_at` instead of `now`,
  taken once at registration — the heap is trusted as-is afterward (a
  coder's deadline cannot change again while still queued: it only
  moves once the coder leaves the queue to actually compile).
- **Why a parity bit**: on a ring, two non-adjacent coders only actually
  run in parallel when the two best keys land exactly on a non-adjacent
  pair — with a plain scalar key that happens only by chance (on a
  4-coder ring, 1 of the 3 possible pairings). Parity guarantees it on
  an even-sized ring (every dongle always connects one even and one odd
  id) and leaves only one unresolved link on an odd-sized one. Each
  dongle flips which parity it favors every time it is granted
  (`take_dongle`), so the same side isn't favored forever. An id-aware
  tie-break was chosen over aging (doesn't help FIFO, whose key never
  goes stale), true randomization (`rand` isn't in the subject's allowed
  function list), or a global matching across the ring (would need a
  coder to act on another coder's state beyond the shared dongle model,
  close to the inter-coder communication the subject explicitly
  forbids) — it's the only option that stays strictly local to each
  dongle.
- All `n` coder threads are released to start competing for dongles at
  the same logical instant (see "startup barrier" below), instead of in
  the order `pthread_create` happened to run them. Earlier testing showed
  that otherwise, on a ring topology, the thread created last is
  systematically delayed on *both* of its dongles at once, which could
  turn a single straggler into a repeatable, avoidable burnout even with
  an otherwise generous `time_to_burnout` margin.
- **Known limit, not a bug**: at a `time_to_burnout` margin close to the
  topology's real worst-case waiting time, even a fully correct EDF
  policy occasionally lets a coder burn out, because several coders can
  legitimately share the exact same deadline at startup — some strict
  order has to be picked, and the loser of that order is still bound by
  real synchronization latency. This was measured directly (repeated
  runs at a tight margin showing a non-zero, non-systematic burnout
  rate) rather than assumed.

### Cooldown handling

A released dongle is not reusable immediately: `release_dongle()` sets
`available_at = now + dongle_cooldown` for both dongles. Checking the
cooldown is a plain timestamp comparison
(`!dongle->is_taken && now >= dongle->available_at`), done on demand
inside `is_ready()` — no dedicated thread or sleep is needed just to track
it, and a coder that is only blocked on a cooldown waits on
`pthread_cond_timedwait` (woken exactly when the cooldown ends) rather
than polling.

### Precise burnout detection

The monitor thread (`monitor_routine`) does not poll on a fixed interval.
Each pass computes the *earliest* `burnout_at` among coders still
compiling, and sleeps on exactly that deadline:
```c
deadline = ms_to_deadline(sim->config->start_time, earliest);
pthread_cond_timedwait(&sim->sched_cond, &sim->sched_mutex, &deadline);
```
It is also woken early by every `pthread_cond_broadcast()` a coder emits
(each `release_dongle()` call), so a shortened deadline (a coder just
started a new compile with a sooner burnout time) is picked up on the
very next loop iteration, not after a stale wait finishes.

### Log serialization

Every log line goes through `log_event()`, which takes `log_mutex` around
the single `printf` call. No other function calls `printf` directly, so
two coders' lines (or a coder's and the monitor's) can never interleave
mid-line.

## Thread synchronization mechanisms

| Primitive | Scope | Protects |
|---|---|---|
| `sim->log_mutex` | simulation-wide | The single `printf` in `log_event()`. |
| `sim->sched_mutex` | simulation-wide | `stop`, `started_count`, `curr_nb_compil`, and is the mutex every `pthread_cond_wait`/`timedwait` on `sched_cond` is paired with. |
| `dongle->mutex` (one per dongle) | per dongle | `is_taken`, `available_at`, and that dongle's `wait_queue` heap. |
| `sim->sched_cond` | simulation-wide | The single condition variable every coder waits on while a dongle is unavailable, that the monitor waits on between scans, and that the startup barrier uses. |

### Why one shared condition variable is enough

`sched_cond` is used for four distinct reasons (a coder waiting for
dongles, a coder releasing dongles, the monitor waiting for the next
deadline, and the startup barrier), but there is no risk of one waiter
reacting to an unrelated wakeup: every wait is written as a loop that
re-checks *its own* condition after waking, which is the standard,
required pattern for condition variables (a wakeup is only ever a hint to
re-check, never a guarantee). For example, `take_dongle()`'s wait loop:
```c
while (!is_ready(coder) && !coder->sim->stop)
    pthread_cond_wait(&coder->sim->sched_cond, &coder->sim->sched_mutex);
```
A coder woken by an unrelated `release_dongle()` elsewhere on the table
simply finds `is_ready(coder)` still false and goes back to sleep — no
extra bookkeeping needed.

### Example: how a race on `is_taken` is prevented

Without synchronization, two coders could both read `is_taken == false`
for a shared dongle, and both proceed to take it. This is prevented by
requiring that **every** read or write of `is_taken`/`available_at`
happens with the relevant dongle mutex held — `lock_dongles()` /
`unlock_dongles()` around the check in `is_ready()` and around the
assignment in `take_dongle()`/`release_dongle()` — and that the decision
to actually proceed additionally happens under `sched_mutex`, so no two
coders can simultaneously pass the `is_ready()` check and both believe
they won the race. This specific race was demonstrated concretely with a
throwaway stress test early in the project (hundreds of double-claim
violations per run without the mutex, zero with it).

### Example: thread-safe communication between coders and the monitor

The monitor never reads a coder's state through a separate message queue
or signal — it reads the same `t_coder` fields the coder itself writes
(`curr_nb_compil`, `burnout_at`), through the shared `sim->coders`
pointer, with both sides serializing their access through `sched_mutex`.
When the monitor decides to stop the simulation, it does not call
`exit()` (not in the subject's allowed function list, and it would skip
every thread's own cleanup) — it sets `sim->stop = 1` and calls
`pthread_cond_broadcast(&sim->sched_cond)` while holding `sched_mutex`,
so every coder currently waiting wakes up, observes the flag, and returns
from its own routine normally, to be joined like any other thread.

### Startup barrier

`wait_for_start()` (`src/coder/routine.c`) makes every coder thread wait,
right after being spawned, until all `n` have reached that point, using
`sim->started_count` under `sched_mutex`/`sched_cond` — no new mutex or
condition variable is introduced for this. This exists purely for
fairness under EDF (see "Starvation prevention" above): without it, the
real (if tiny) time `pthread_create` takes between threads gives whichever
coder is spawned last a reproducible head start disadvantage on both of
its dongles.

## Known limitations

- Under EDF, at a `time_to_burnout` margin close to the structural
  worst-case wait time for the chosen `number_of_coders` (several coders
  can start with an identical deadline), occasional burnouts are expected
  and were measured, not just theorized, by repeating the same run dozens
  of times at a tight margin.
- With `dongle_cooldown = 0`, both FIFO and EDF could degenerate into a
  single coder active at a time (a one-way sweep around the ring)
  instead of exploiting the parallelism the topology allows. The
  per-dongle parity tie-break above fixes this reliably at the small
  coder counts actually exercised by grading configurations — measured
  directly on the three configurations this was reported against
  (`5 800 200 200 100 7 0 edf`, `4 800 200 200 100 5 0 edf`,
  `5 900 200 200 100 5 0 fifo`), each repeated 40 times: burnout rates
  dropped from 6/40, 16/40 and 8/40 to 0/40, 1/40 and 0/40 respectively.
  It is not a complete fix at every scale, though: each dongle flips
  which parity it favors independently, with no synchronization between
  neighboring dongles, so on a larger ring (measured at 10 coders) real
  scheduling jitter can leave two adjacent dongles out of phase with
  each other and reproduce the same kind of stall for whichever coder
  sits between them — around 10% of runs at that size, versus 0% before
  this tie-break existed. Resolving that fully would mean synchronizing
  parity across the whole ring, which starts to look like the global
  matching alternative already ruled out for requiring coders to act on
  state beyond their own shared dongles.

## Resources

- Dijkstra, E. W. — *Dining Philosophers Problem*, the classical
  formulation this activity is modeled after.
- Coffman, E. G. et al. — the four necessary conditions for deadlock
  (mutual exclusion, hold and wait, no preemption, circular wait).
- `man pthread_create`, `man pthread_join`, `man pthread_mutex_init`,
  `man pthread_cond_init`, `man pthread_cond_timedwait`,
  `man clock_gettime` — POSIX threads and time reference documentation.
- 42 Norm documentation (Norminette).

**AI usage:** Claude (Anthropic, Claude Code) was used throughout this
activity as a learning tool to learn how to use thread and mutex in C and to help writing the README.
