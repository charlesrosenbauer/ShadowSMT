# ShadowSMT

This is a highly experimental SMT solver I'm building.

The focus will be on verifying small programs for application in static analysis and program synthesis / superoptimization.

The rough plan is to use vectorized bit manipulation to brute force through small SAT problems at the leaves of the search tree to heavily optimize the SAT part, and to primarily use Galois Connections for dealing with more abstract SMT-style properties.