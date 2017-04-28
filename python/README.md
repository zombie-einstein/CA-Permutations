## Python Implementation

This supports rulesets up to aribitary number of states, but only states consisting of three adjecent cells.

Note numbers grow quickly, for **s** states three cells have **S**<sup>3</sup> possible states, ca is then mapping from these cell permutations back to **S** so there are then (**S**)<sup>**S**<sup>3</sup></sup> possible rulesets. For example

| States | Cell Permutations | Number of Rules |
|--------|-------------------|-----------------|
|2       |8                  |256              |
|3       |27                 |7.6255975e+12    |
|4       |64                 |3.4028237e+38    |

rule_checker.py cn be run from the command line and will print the details of a ruleset given the number of states and a rule number (in base 10) e.g. "rule_checker.py 2 200" will print rule 200 for two states to the console.