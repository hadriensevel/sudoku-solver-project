clear, close all
M = [7,0,3,0,0,0,0,5,8;4,0,0,0,6,8,1,0,0;8,6,0,7,0,0,0,0,0;0,0,4,0,2,0,7,9,6;0,0,8,0,0,6,2,0,0;0,9,0,0,7,1,0,0,4;0,3,6,0,0,0,0,0,9;1,4,0,6,0,9,0,0,2;0,0,5,0,4,0,3,0,0;];
global maxrlevel nIteration;
maxrlevel = 0;
nIteration = 0;
tic
[Solved,solvability] = solveSudoku(M,0);
elapsedTime = toc;
dispSudoku(M,Solved,'s9.JPG',nIteration,maxrlevel,elapsedTime);