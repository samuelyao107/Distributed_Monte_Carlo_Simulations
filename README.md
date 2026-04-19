# Distributed Monte Carlo Option Pricer

A high-performance, distributed system for pricing **European Call Options** using Monte Carlo simulation. Built with a C++ worker pool coordinated by a Python master node over a RabbitMQ message bus — demonstrating both quantitative finance fundamentals and production-grade distributed systems design.

---

## Highlights

- **Parallelized Monte Carlo** pricing via a master-worker architecture (Map-Reduce pattern)
- **C++17 workers** for high-throughput numerical simulation (GBM path generation)
- **Python master** for task orchestration and result aggregation
- **RabbitMQ** as the AMQP message broker for decoupled, asynchronous communication
- **Monte Carlo results validated** against the Black-Scholes analytical formula via Google Test
- **Fully containerized** with Docker and docker-compose

---

## Financial Model

The pricer values a **European Call Option** under the **Black-Scholes framework**, simulating the underlying asset via **Geometric Brownian Motion (GBM)**:

$$S_T = S_0 \cdot \exp\!\left(\left(r - \frac{\sigma^2}{2}\right)T + \sigma\sqrt{T}\,Z\right), \quad Z \sim \mathcal{N}(0,1)$$

The option price is estimated as the discounted expected payoff:

$$C = e^{-rT}\,\mathbb{E}\!\left[\max(S_T - K,\, 0)\right]$$

| Parameter | Symbol | Default |
|-----------|--------|---------|
| Spot price | $S_0$ | 100.0 |
| Strike price | $K$ | 100.0 |
| Time to maturity | $T$ | 1 year |
| Risk-free rate | $r$ | 5% |
| Volatility | $\sigma$ | 20% |
| Paths per worker | — | 100,000 |

The standard error of the estimator is computed alongside the price to provide a statistical confidence interval on the result.

---

## Architecture

The system follows a **Map-Reduce** pattern over a message queue:

```
┌─────────────────────────────────────────────────────┐
│                  Master (Python)                     │
│                                                      │
│   MAP: publish N task messages (option parameters)   │
│   REDUCE: average N partial prices from workers      │
└───────────────┬─────────────────────┬───────────────┘
                │                     ▲
         task_queue              result_queue
                │                     │
        ┌───────▼─────────────────────┴───────┐
        │            RabbitMQ Broker           │
        └───────┬─────────────────────┬───────┘
                │                     │
    ┌───────────▼──────────┐  ┌───────▼──────────────┐
    │    Worker (C++)       │  │    Worker (C++)        │
    │  GBM path simulation  │  │  GBM path simulation   │
    │  payoff discounting   │  │  payoff discounting    │
    └───────────────────────┘  └──────────────────────┘
```

Each worker independently simulates its batch of paths and publishes a `MonteCarloResult` (price, std\_error, num\_paths) back to the master. The master aggregates results by computing the mean price across all workers.

---

## Project Structure

```
.
├── CMakeLists.txt              # C++ build configuration (CMake 3.20+)
├── Dockerfile                  # Multi-stage build: compile worker → minimal runtime image
├── docker-compose.yml          # Orchestrates RabbitMQ + worker replicas
├── master.py                   # Python master: MAP → REDUCE coordinator
├── include/pricer/
│   ├── EuropeanCall.h          # Pricer class interface
│   ├── Parameters.h            # Option parameters (JSON-serializable)
│   └── MonteCarloResult.h      # Result struct (price, std_error, num_paths)
├── src/
│   ├── main.cpp                # Worker entry point: consumes tasks, publishes results
│   └── EuropeanCall.cpp        # Core Monte Carlo simulation loop
└── tests/
    └── test_pricer.cpp         # GTest suite: MC vs Black-Scholes analytical price
```

---

## Getting Started

### Prerequisites

- [Docker](https://docs.docker.com/get-docker/) and [docker-compose](https://docs.docker.com/compose/)
- Python 3.10+ with `pika` (`pip install pika`)

### Run with Docker Compose

**Step 1 — start RabbitMQ and scale the worker pool**

```bash
# Start RabbitMQ + N worker containers (replace 8 with any number you need)
docker-compose up --build --scale worker=8
```

`--scale worker=N` controls how many C++ worker containers are started. Each container picks tasks from `task_queue` independently — the more workers, the more parallel simulation batches.

**Step 2 — launch the master and tell it how many tasks to dispatch**

```bash
# In a separate terminal — must match the number of workers started above
python3 master.py --workers 8
```

The `--workers` argument controls how many pricing tasks the master publishes. Set it equal to `--scale worker=N` so every worker receives exactly one task.

```
python3 master.py --workers 8    # dispatch 8 tasks → 8 workers
python3 master.py --workers 4    # dispatch 4 tasks → 4 workers (default)
python3 master.py                # same as --workers 4
```

**Expected master output (with 4 workers):**

```
--- PHASE 1 : MAP (Distribution) --- [4 workers]
 [Master] Task 0 sent to workers.
 [Master] Task 1 sent to workers.
 [Master] Task 2 sent to workers.
 [Master] Task 3 sent to workers.

--- PHASE 2 : REDUCE (Aggregation) ---
 [Master] Partial result received: 10.4521
 [Master] Partial result received: 10.4489
 [Master] Partial result received: 10.4503
 [Master] Partial result received: 10.4517

 [SUCCESS] REDUCE COMPLETE. Option price: 10.4508
```

The RabbitMQ management UI is available at [http://localhost:15672](http://localhost:15672) (guest/guest).

### Local Build (C++ Worker Only)

```bash
mkdir build && cd build
cmake ..
cmake --build .

# Run unit tests (Monte Carlo vs Black-Scholes)
./tests

# Run a standalone worker (requires RabbitMQ on localhost:5672)
./worker
```

---

## Testing & Validation

Unit tests in [tests/test_pricer.cpp](tests/test_pricer.cpp) verify the Monte Carlo estimator converges to the **Black-Scholes closed-form price**. With 1,000,000 simulation paths, the absolute error is expected to be below **±0.05** — consistent with the $O(1/\sqrt{N})$ convergence rate of Monte Carlo integration.

```bash
cd build && ./tests
```

---

## Technology Stack

| Layer | Technology |
|-------|-----------|
| Numerical engine | C++17, GBM simulation |
| Serialization | nlohmann/json |
| Messaging (C++) | SimpleAmqpClient, librabbitmq |
| Message broker | RabbitMQ (AMQP 0-9-1) |
| Orchestration (Python) | pika |
| Build system | CMake 3.20+ |
| Testing | Google Test (GTest) |
| Containerization | Docker, docker-compose |

---

## Configuration

| Variable | Default | Description |
|----------|---------|-------------|
| `RABBITMQ_HOST` | `localhost` | RabbitMQ hostname (set in docker-compose for workers) |
| AMQP port | `5672` | Standard RabbitMQ port |
| Management UI port | `15672` | RabbitMQ admin dashboard |
| `--workers N` (master.py) | `4` | Number of tasks dispatched by the master; set equal to `--scale worker=N` |
| `--scale worker=N` (docker-compose) | `1` | Number of C++ worker containers to spin up |
| Paths per task | `100,000` | Monte Carlo paths per worker batch |

---

## Author

**Samuel Yao** — [samuelyao107@gmail.com](mailto:samuelyao107@gmail.com)
