QUESTION: How many cores will simple.cu use, max, as written? How many SMs?
1 sm
at max 16 cores can work

QUESTION: What was the problem and how did you fix it?

the problem was that all threads were doing the add at the same time, we solved it using atomicAdd