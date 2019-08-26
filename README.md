# Combine a TLSF specification and AIG implementation for model checking

(This is a clone of Leander Tentrup's original tool)

Dependencies:

* `smvtoaig` from AIGER toolset

Build: `$ make`

Usage:

* Convert TLSF to AIGER monitor: `$ syfco --format smv -m fully example.tlsf | smvtoaig > monitor.aag`
* Combine monitor with implementation: `$ ./combine-aiger monitor.aag implementation.aag > combined.aag`
* `combined.aag` can be checked by an arbitrary AIGER model checker (possibly only after conversion to binary format using `aigtoaig combined.aag combined.aig`)
* To additionally check that `implementation.aag` is a Moore machine, use: `$ ./combine-aiger --moore monitor.aag implementation.aag > combined.aag`
