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


## Log Examples:
RandomSPSCTest output (with annotations to check correctness):
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

