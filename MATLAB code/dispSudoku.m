%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% File: dispSudoku.m
% Created on 3.3.2021
% Author: Hadrien Sevel
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function dispSudoku(Initial,Solution,file,NBiterations,recLevel,duration)
%DISPSUDOKU Crée l'affichage de la grille de sudoku et l'exporte au format PDF

% Crée la matrice logique qui contient 0 pour les cases du sudoku résolues
% par l'algorithme et 1 pour les cases initiales
BoxSolved = (Initial ~= 0);

% Crée la figure avec des axes orthonormés et cache les axes
figure('Visible','off')
hold on
axis off
axis equal

% Crée les couleurs pour les cases (blanc pour une case résolue & gris pour
% un case initiale)
colormap([1,1,1;.9,.9,.9])
imagesc(0.5,0.5,flip(BoxSolved))

% Adapté depuis une exemple de la documentation MATLAB (fonction drawSudoku):
% https://www.mathworks.com/help/optim/ug/solve-sudoku-puzzles-via-integer-programming-solver-based.html#SolveSudokuExample-7

% Crée les rectangles de la grille de sudoku
rectangle('Position',[0,0,9,9],'LineWidth',1.75)
rectangle('Position',[3,0,3,9],'LineWidth',1.75)
rectangle('Position',[0,3,9,3],'LineWidth',1.75)
rectangle('Position',[0,1,9,1],'LineWidth',0.75)
rectangle('Position',[0,4,9,1],'LineWidth',0.75)
rectangle('Position',[0,7,9,1],'LineWidth',0.75)
rectangle('Position',[1,0,1,9],'LineWidth',0.75)
rectangle('Position',[4,0,1,9],'LineWidth',0.75)
rectangle('Position',[7,0,1,9],'LineWidth',0.75)

[X,Y] = meshgrid(1:9);
% Redéfinie la matrice Solution avec 4 colonnes (chaque ligne correspond à
% une case du sudoku) :
%   - cordonnée en x de la case
%   - coordonnée en y de la case
%   - valeur de la case
%   - si la case a été résolue (0) par l'algoritme ou si c'est une case
%   initiale (1)
Solution = [X(:),Y(:),Solution(:),BoxSolved(:)];

% Boucle sur chaque case de la matrice Solution
for i = 1:size(Solution,1)
    if Solution(i,4) == 1
        % Cas d'une case initiale : affiche le chiffre en gras
        text(Solution(i,1)-0.66,9.5-Solution(i,2),num2str(Solution(i,3)),'FontSize',16,'FontWeight','bold')
    else
        % Cas d'unce case résolue : affiche le chiffre en italique
        text(Solution(i,1)-0.66,9.5-Solution(i,2),num2str(Solution(i,3)),'FontSize',16,'FontAngle','italic')
    end
end

% Affiche les textes autour de la grille de sudoku
text(0,9.5,file,'Color','b','FontSize',16,'FontWeight','bold')
text(6,9.33,datestr(datetime))
text(0,-0.25,['#iterations = ' num2str(NBiterations) ','],'FontSize',8)
text(2.5,-0.25,['deepest recursion level = ' num2str(recLevel) ','],'FontSize',8)
text(6.5,-0.25,['duration = ' num2str(duration) ' [s]'],'FontSize',8)

% Enlève le '.jpg' du nom de fichier
pdfname = strrep(lower(file),'.jpg','');

% Exporte la figure au format PDF et ouvre le PDF
print(pdfname,'-dpdf','-fillpage')
open(strcat(pdfname,'.pdf'))
end