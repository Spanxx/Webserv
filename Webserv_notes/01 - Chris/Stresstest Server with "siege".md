📆 2025-05-31 🕓 17:13
Tags: #testing #server  

---

```ad-info
For more details: https://commandmasters.com/commands/siege-common/
```

# Basic execution and different flags

`siege https://example.com`

**Execution with multiple endpoints**

`siege --file <path to file>`

**Execution with multiple endpoints in a random order**

`siege --internet --file <path to file>`

`--internet` =  This flag indicates that the URLs will be accessed in a random order, simulating non-sequential user access patterns.

**Benchmark a list of URLS**

`siege --benchmark --file <path to file>`

`--benchmark` = removes the simulated delay between useres, allowing a constant surge of requests to benchmak absolute performance

**Set the amount of concurrent connections**

`siege --concurrent=50 --file <path to file>`

❗With more then 255 connections, a warning pops up and you have to change the value in the config-file for siege

`--concurrent=50` = configures siege to simulate 50 users making cocurrent requests, providing a measure of how performance scales with user count

**Set time for siege to run**

`siege --time=30s --file <path to file>`

# Complete command for stresstest

`siege --benchmark --internet --concurrent=255 --time=300s --file url_list.txt`

- 255 concurrent connections
- 5 minutes (300sec)
- random endpoints
- no simulated delay

