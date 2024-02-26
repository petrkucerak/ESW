# Notes

## Betley Rules

### Data strucutres

#### Packing & Encoding
- save variables to the registers
- using structures - save space and speed up

#### Argumentation

- add argument to speed up oparetion
- ex. add `tail` link to linked list

![alt text](image.png)

#### Precomputation

- example Pascal's Trinagle

![alt text](image-1.png)

#### Compile-Time Initialization

- store data during compilation => save work during executing time
- *hardcore values into code or precalucle during compilation*

![alt text](image-2.png)

#### Caching

- not only hardware, we can cache software too
- store results that have been accessed recently so that the program need not compute them again

![alt text](image-3.png)

#### Sparsity

- avoid storing and computation on zeroes

![alt text](image-4.png)
![alt text](image-5.png)

### Logic

#### Constant Folding and Propagation

- evaluate constant expressions and substitute the result into futher expressions, all during compilation

![alt text](image-6.png)

#### Common-Subexpression Elimination

- aviod computatationg the same expression multiple times by evaluating the same expression multiple times by evaluating the expression once and storing the result for later use

![alt text](image-7.png)

#### Algebraic Indetities

- to replace expensive algebraic expressions with algebraic equivalents that requiere less work

![alt text](image-8.png)
![alt text](image-9.png)

#### Short-Circuiting

- when perfroming a series of test, that idea of *short-circutting* is to stop evaluating as soon as you know the answare.

![alt text](image-10.png)

#### Ordering Tests

- order test that are more often "successful" befeore tests that are rarely successful

![alt text](image-11.png)

#### Creating a Fast Path

- first time try faster computation and only if the first type of cumputation is not success, do longer

![alt text](image-12.png)
![alt text](image-13.png)

#### Combining Tests

- the idea is to replace a sequence of test with one test or switch
- *insted of sequencial, create decisions trees*

![alt text](image-14.png)
![alt text](image-15.png)

