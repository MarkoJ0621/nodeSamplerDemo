# Steps for adding a new node

i am making this because there are so many moving parts that it is easy to forget one

## basic process

- make a new .svelte file and copy paste all the stuff from one of the nodes as a baseline
- the only native function you need (for now) is setParameter(), where you send value then id and then the parameter ID of the parameter you want to change, this system allows for the least bulky code.
- add the new node as a type in the addNode() function in app.svelte
- MAKE SURE TO NPM RUN BUILD ONCE IT WORKS!!!!!!
- copy an existing class and rename it to your node, override any functions you need to and override setParameter to use whatever parameterIDs you have selected.
- add it to the newNode if statement in pluginprocessor.cpp.
- that should be it? but it can be hard to keep track of where you might've gone wrong with parameter typos and such.

good luck <333