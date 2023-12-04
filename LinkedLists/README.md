## Design decision implications
### Traverse list without locking
One can decide to avoid the cost of acquiring the lock of every node while traversing list.
Gains:
- Multiple threads can traverse different sections of the list without interfering.

Implications:
- Another thread may modify pred, curr between the time they are found and the time they are locked.
  => Solution: After locking, must make sure that nothing has changed around pred and curr. More precisely, check that pred and curr have not been removed, and that a node has not been added between them.
## FineList
Lock acquisitions: every node while traversing list.

## OptimisticList
Lock acquisitions: only nodes being modified.

Assumptions: works well if cost of traversing list twice w/o locking 
is less than that of traversing once with locking.

Algorithm to modify list:
- Do until success:
    - Iterate through list without locking to find the two nodes to modify: pred, curr.
    - Lock pred,curr.
    - If pred, curr were NOT modified by another thread between the time they were found and the time they were locked.
      - perform modification
    - Otherwise:
      - try again

## Possible Race Conditions: FineList
### RC1:
Before accessing the next node, you must have locked the current.
For example, this is wrong:
```
pred = head
curr = head->next
pred.lock()
curr.lock()
```
This is a problematic order of execution:
1. Consider an initially empty list.
2. Imagine thread T1 is currently inserting 42, which means it has
   locked the tail and head.
3. Suppose T2 now tries to remove 42. With the above locking pattern,
   T2 will set 'pred' and 'curr' to head and tail, and then block on
   'pred.lock()' since T1 currently holds it.
4. When T1 finishes the insertion, T2 will not find '42' in the list
   because its iteration begins from 'pred' and 'curr' being set to
   head and tail.


## Race Conditions and Bugs: LazyList 
### Bug 1:
#### Issue:
Constructors of MarkedNode did not initialize explicitly removed field.

#### Observed behavior:
Sometimes test would pass, but most times it would get stuck inside infinite loop.

#### How I solved it:
I spent several hours across many days trying to ensure that pointers in the list were as expected.
But the issue came down to the fact that the removed field would sometimes get set to true when
creating the node, which would cause the validate() call to fail, and thus it got stuck in an infinite loop.

#### Lesson:
Always initialize all fields explicitly.


## Log Examples:
RandomSPSCTest output for FineList (with annotations to check correctness):
```
THREAD_ID,OPE,VAL,RET,SIZ,TS
T1,ADD,0,true,1,144   -> {0}
T1,ADD,0,false,1,201
T1,ADD,9,true,2,206   -> {0, 9}
T1,ADD,7,true,3,209   -> {0, 7, 9}
T1,ADD,5,true,4,212   -> {0, 5, 7, 9}
T2,REM,8,false,4,225   -> {0, 5, 7, 9}
T1,ADD,0,false,4,238   -> {0, 5, 7, 9}
T1,ADD,8,true,5,256   -> {0, 5, 7, 8, 9}
T1,ADD,0,false,5,260
T1,ADD,0,false,5,263
T1,ADD,7,false,5,271   -> {0, 5, 7, 8, 9}
T2,REM,9,true,4,277   -> {0, 5, 7, 8}
T2,REM,3,false,4,313
T2,REM,8,true,3,318   -> {0, 5, 7}
T2,REM,0,true,2,322   -> {5, 7}
T2,REM,5,true,1,325   -> {7}
T2,REM,3,false,1,328
T2,REM,5,false,1,331
T2,REM,8,false,1,333
T2,REM,0,false,1,335   -> {7}
```

RandomSPSCTest output for OptimisticList:
```
 THREAD_ID,  OPE,  VAL,  RET, SIZE,   TS
        T1,  ADD,    0, true,    2,  265  -> {42, 0}
        T2,  REM,    8,false,    2,  276  -> {42, 0}
        T2,  REM,    9,false,    2,  281  -> {0, 42}
        T2,  REM,    7,false,    2,  283  -> {0, 42}
        T2,  REM,    5,false,    2,  285  ->
        T1,  ADD,    0,false,    2,  290  ->
        T1,  ADD,    9, true,    3,  295  -> {0, 9, 42}
        T1,  ADD,    8, true,    4,  298  -> {0, 8, 9, 42}
        T1,  ADD,    0,false,    4,  300  -> 
        T2,  REM,    0, true,    3,  304  -> {8, 9, 42}
        T2,  REM,    7,false,    3,  313  ->
        T2,  REM,    3,false,    3,  316  ->
        T2,  REM,    8, true,    2,  318  -> {9, 42}
        T2,  REM,    0,false,    2,  320  ->
        T2,  REM,    5,false,    2,  322  ->
        T1,  ADD,    0, true,    3,  334  -> {0, 9, 42}
        T1,  ADD,    3, true,    4,  339  -> {0, 3, 9, 42}
        T1,  ADD,    5, true,    5,  342  -> {0, 3, 5, 42}
        T1,  ADD,    8, true,    6,  345  -> {0, 3, 5, 8, 42}
        T1,  ADD,    0,false,    6,  348  -> {3, 5, 8, 42}
```

