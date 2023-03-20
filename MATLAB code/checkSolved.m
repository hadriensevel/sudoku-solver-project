%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% File: checkSolved.m
% Created on 17.3.2021
% Author: Hadrien Sevel
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function solvability = checkSolved(A)
%CHECKSOLVED Vérifie si le sudoku est résolu ou non
%   1 : une seule possibilité pour chaque case, sudoku résolu
%   0 : au moins une case avec plusieurs possibilités, sudoku non résolu
%   -1 : au moins une case sans possibilité, sudoku non résolvable

%  Crée la matrice R qui contient le nombre de possibilités pour chaque case
R = sum(A,3);

% Si tous les coefficients de R sont égaux à 1, sudoku résolu
if all(R == 1,'all')
    solvability = 1;
    
% Si au moins un coefficient est nul, sudoku non résolvable
elseif any(R == 0,'all')
    solvability = -1;
    
% Sinon, sudoku non résolu
else
    solvability = 0;
end
end