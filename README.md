# cache-simulator
Memory cache simulator that calculates the number of memory-writes and reads a given memory access pattern executes.
<br /><br />
## Usage: <br />
`./SIM <CACHE_SIZE> <ASSOC> <REPLACEMENT><WB><TRACE_FILE>` <br/>
`<CACHE_SIZE> is the size of the simulated cache in bytes` <br/>
`<ASSOC> is the associativity` <br/>
`<REPLACEMENT> replacement policy: 0 means LRU, 1 means FIFO` <br/>
`<WB> Write-back policy:0 means write-through, 1 meanswrite-back` <br/>
`<TRACE_FILE> trace file name with full path` <br/>
### Example: <br />
```bash
gcc cachesim.c -o SIM
./SIM 32768 8 0 1 (path to tracefile)
```
