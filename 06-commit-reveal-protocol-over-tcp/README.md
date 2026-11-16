# 06 · Commit-Reveal Protocol over TCP

A **cheat-proof Rock Paper Scissors** game between two machines. A **SHA-256 commit-reveal protocol** makes sure that a player cannot change their move after seeing the opponent's.

📄 [Report (PDF)](report.pdf)

## Protocol

```mermaid
sequenceDiagram
    participant A as Alice (server)
    participant B as Bob (client)
    Note over A: move + random 16-byte salt<br/>C = SHA-256(move ‖ salt)
    A->>B: commitment C
    Note over B: C reveals nothing about the move
    B->>A: Bob's move (plaintext)
    A->>B: reveal: move | salt
    Note over B: recompute SHA-256(move ‖ salt)<br/>equal to C → valid round<br/>different → Alice cheated
```

- **Hiding:** the random salt (`secrets.token_hex(16)`) prevents Bob from brute-forcing the three possible moves from the hash.
- **Binding:** Alice cannot find a different move that produces the same SHA-256 hash.

## Demo (two Linux VMs in bridged mode)

| Alice tries to cheat (rock → scissors) | Detected by Bob |
|:---:|:---:|
| <img src="figures/4_alice_tries_to_cheat.png" width="400"> | <img src="figures/5_cheat_detected.png" width="400"> |

An honest round, where the two hashes match:

<img src="figures/6_honest_round.png" width="500">

## Run it

On Alice's machine, which must start first:

```bash
python3 alice_server.py
```

On Bob's machine, after setting Alice's IP address in `bob_client.py` (`soc.connect(("192.168.1.7", 12345))`):

```bash
python3 bob_client.py
```

Both scripts use only the Python standard library (`socket`, `hashlib`, `secrets`). The messages travel over plain TCP: the protocol guarantees fairness between the two players, not the confidentiality of the channel.
