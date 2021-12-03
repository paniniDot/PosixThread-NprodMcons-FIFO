# PosixThread N producers M consumers 1 buffer problem modellation
> Simple posix thread example.

> It should model a problem in which **N producers** have to load their products in a **single common buffer** and **M consumers** have to store those products from the common buffer.
> Of course both producers and consumers have to work within a common variable which needs to be protected from possible race conditions.

> In order to ensure a fair distribuition of the resources beetween producers and consumers (which scheduler would not perfectly does) the implementation followed a **FIFO** policy.
