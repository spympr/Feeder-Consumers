# Feeder-Consumers
In this mini project we solve one OS problem with main process be Feeder and child processes of Feeder be its Consumers.

# Summary
Here Feeder has a big array of integers and also we have a shared memory, which can keep only one integer. So child processes of Feeder (Consumers) are going to read each number of big Feeder's array. In the end each Consumer create a file in directory files/ and write inside the big array of Feeder and its average delay.

# Execution 
You can check it out, only with execution of: **bash script.sh Number_Of_Consumers Size_Of_Array**
Note: You need to give 2 arguments with exactly this order.
After that run **bash script2.sh**. This is for cleaning what we just created.
