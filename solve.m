clear, close all
M = [0,0,0,4,0,0,0,8,0;0,4,0,6,0,0,3,0,0;0,3,0,8,1,0,0,2,0;1,0,0,3,9,0,0,0,7;0,0,6,0,7,0,0,0,0;0,5,0,1,6,0,0,9,0;0,0,3,0,0,0,0,0,0;9,0,0,0,0,0,0,5,1;0,0,5,0,0,2,0,0,3;];
global maxrlevel nIteration;
maxrlevel = 0;
nIteration = 0;
tic
[Solved,solvability] = solveSudoku(M,0);
elapsedTime = toc;
dispSudoku(M,Solved,'S7.jpg',nIteration,maxrlevel,elapsedTime);