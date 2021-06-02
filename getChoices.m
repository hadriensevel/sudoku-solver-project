function [x,y,choices] = getChoices(A,M)
%GETCHOICES Sélectionne une case avec le moins de possibilités et donne ses
%cordonnées x et y ainsi que le vecteur choices qui contient les
%possibilités

% Crée la matrice R qui contient le nombre de possibilités pour chaque case
% non résolue du sudoku
R = sum(A,3).*(M == 0);

% Sélectionne le nombre minimal de possibilités dans la matrice R
minNumberOfChoices = min(R(R>0),[],'all');

% Récupère les coordonnées x et y des cases ayant le nombre minimal de
% possibilités dans deux vecteurs X et Y
[X,Y] = find(R == minNumberOfChoices);

% Sélectionne les coordonnées de la première case et crée le vecteur
% choices contenant les chiffres possibles pour cette case
x = X(1);
y = Y(1);
choices = find(reshape(A(x,y,:),[1,9]) == 1);
end