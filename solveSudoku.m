%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% File: solveSudoku.m
% Created on 24.3.2021
% Author: Hadrien Sevel
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [M,solvability] = solveSudoku(M,rlevel)
%SOLVESUDOKU Fonction récursive qui résoud le sudoku

% Définie les variables globales contenant le niveau maximum de récursion
% et le nombre d'itérations
global maxrlevel nIteration;

% Incrémente de 1 le nombre d'itérations à chaque appel de la fonction
nIteration = nIteration + 1;

% Si le niveau de récursion courant est plus élevé que le niveau de
% récursion maximum, associe le niveau courant au niveau max
if rlevel > maxrlevel
    maxrlevel = rlevel;
end

% Vérifie si le sudoku est résolu
A = fillHypothesis(M);
solvability = checkSolved(A);

% Si le sudoku n'est pas encore résolu, sélectionne une case avec le moins
% de possibilités
if solvability == 0
    [x,y,choices] = getChoices(A,M);
    
    % Boucle sur toutes les possibilités de la case
    for i=1:size(choices,2)
        
        % Associe la possibilité à la case et vérifie si le sudoku est
        % résolu ou non
        M(x,y) = choices(i);
        A = fillHypothesis(M);
        solvability = checkSolved(A);
        
        % Si le sudoku n'est pas résolu, appelle la fonction solveSudoku et
        % monte d'un niveau de récursion
        if solvability == 0
            [M,solvability] = solveSudoku(M,rlevel+1);        
        % Si le sudoku est résolu (i.e. une seule possibilité pour chaque
        % case), remplit toutes les cases restantes
        elseif solvability == 1
            while any(M==0,'all')
                [x,y,choices] = getChoices(A,M);
                M(x,y) = choices;
            end
        end
        
        % Bloc de conditions exécuté lorsque que l'algorithme backtrack
        % (i.e. redescent d'un niveau de récursion)
        
        % Si le sudoku est résolu, sort de la boucle for (car pas besoin de
        % tester d'autres possibilités) et redescent d'un niveau de
        % récursion
        if solvability == 1
            return
        % Si le sudoku n'est pas résolvable, efface la case et continue la
        % boucle si il y a d'autres possibilités pour cette case, sinon
        % redescent d'un niveau de récursion
        elseif solvability == -1
            M(x,y) = 0;
        end
        
    end
end
end