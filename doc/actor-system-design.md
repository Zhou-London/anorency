# Actor System Design

## Overview

```
┌──────────────────────────────────────────────────┐
│                     Stage                        │
│                                                  │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐         │
│  │ Actor A │  │ Actor B │  │ Actor C │         │
│  │ [inbox] │  │ [inbox] │  │ [inbox] │         │
│  └─────────┘  └─────────┘  └─────────┘         │
│                                                  │
│  ┌──────────────────────────────────────────┐   │
│  │              Dispatcher                   │   │
│  └──────────────────────────────────────────┘   │
└──────────────────────────────────────────────────┘
```

Data never leaves a Stage. Actors communicate via message passing.

## Core Entities

### Stage
Top-level container. Owns all actors and the dispatcher. Templated on Dispatcher type.

```cpp
Stage<SingleThreadDispatcher> stage;
Address addr = stage.introduce(setup_fn, init_fn);
stage.run();
```

### Actor
Reactive unit with:
- **Inbox**: `Stream<MessageS>` - receives messages
- **Subscriptions**: type → handler mapping

Actors don't run loops. They react to incoming messages.

### Interface
Actor's view of the system. Passed during setup.

```cpp
stage.introduce(
    [&](Interface& iface) {
        iface.subscribe<Greeting>([](Greeting& msg) {
            // handle
        });
    },
    [&](Interface& iface) {
        iface.publish<Greeting>(other_addr, "hello");
    }
);
```

| Method | Description |
|--------|-------------|
| `subscribe<T>(fn)` | Register handler for message type T |
| `publish<T>(addr, args...)` | Send message to target address |
| `terminate()` | Destroy this actor immediately |

### Address
Opaque handle to an actor. Uses generation counter for safety.

```cpp
struct Address {
    uint32_t index;       // slot in actor array
    uint32_t generation;  // detects stale references
};
```

Stale address (post-terminate) is detected on publish.

### Dispatcher
Controls actor execution. Defined by concept, no virtual overhead.

```cpp
template <typename D>
concept Dispatcher = requires(D d, Actor* a) {
    { d.add_actor(a) };
    { d.remove_actor(a) };
    { d.run() };
    { d.stop() };
};
```

| Variant | Behavior |
|---------|----------|
| `SingleThreadDispatcher` | One thread, round-robin all actors |
| `ThreadPerActorDispatcher` | Dedicated thread per actor |
| `ThreadPoolDispatcher` | (Future) Shared thread pool |

## Message Flow

```
A calls publish<Msg>(B_addr, data)
         │
         ▼
    ┌─────────┐
    │  Stage  │  resolve B_addr → B's inbox
    └────┬────┘
         │
         ▼
    ┌─────────┐
    │ B.inbox │  Stream<MessageS>::try_write()
    └────┬────┘
         │
         ▼ (Dispatcher polls)
    ┌─────────┐
    │ B.subs  │  lookup handler by type_id
    └────┬────┘
         │
         ▼
    handler(msg)
```

## Constraints

- Inbox is SPSC now, MPSC later
- Direct addressing only (no broadcast)
- `terminate()` is immediate, no drain
- Second lambda in `introduce()` runs once on entry

## Future Extensions

- Topic-based pub/sub
- MPSC inbox for multi-sender
- Thread pool dispatcher
- Address caching in Interface
