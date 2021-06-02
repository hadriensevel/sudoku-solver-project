%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% File: fillHypothesis.m
% Created on 10.3.2021
% Author: Hadrien Sevel
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function A = fillHypothesis(M)
%FILLHYPOTHESIS Génère la matrice A 9x9x9 qui contient tous les chiffres
%possibles pour chaque case du sudoku

% Crée la matrice A & met à zéro toutes les cases non vides et non égales
% au chiffre de chaque plan
[~,~,Z] = meshgrid(1:9);
A = M == Z | ~M;

% Boucle sur les 9 chiffres possibles du sudoku
for i=1:9
    % Boucle sur les 9 lignes et colonnes
    for j=1:9
        % Met la ligne j à zéro si elle contient le chiffre i
        if any(M(j,:) == i)
            A(j,:,i) = 0;
        end
        % Met la colonne j à zéro si elle contient le chiffre i
        if any(M(:,j) == i)
            A(:,j,i) = 0;
        end
    end
    % Boucle sur les blocs 3x3 du sudoku
    for k=1:3
        for l=1:3
            % Met à zéro le bloc si i est dedans sauf la case où il y a i
            % qui est égale à 1
            BoxLogical = (M(3*k-2:3*k,3*l-2:3*l) == i);
            isNumberInBox = find(BoxLogical);
            if isNumberInBox
                A(3*k-2:3*k,3*l-2:3*l,i) = BoxLogical;
            end
        end
    end
end
end