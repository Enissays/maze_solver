#include<iostream>
#include<vector>
#include<string>
#include<cmath>
#include <queue>
#include <set>

using namespace std;

// On represente la valeur +inf par la plus grande valeur que peut stocker un entier non signé, et FULL une constante négative indiquant qu'une case est pleine
constexpr unsigned INF = 4294967295;
constexpr int FULL = -1;
constexpr int EXPL = -2;

struct Path
{
    int i_cell;
    string depl;
};

struct Cell_vide
{
    int i,j;
    unsigned distance = -1;
};

struct Point
{
    int x,y;
};


// Toutes les fonctions de vérifications d'erreurs

// Error messages
const string TOO_SMALL("Error: the number of lines/columns must be greater than 2");
const string BAD_LOCATION("Error: the cell is not in the inner space of the grid");
const string OVERLAP_AB("Error: self overlap of start and end cells is not allowed");
const string OVERLAP_FULL("Error: this cell is already full");
const string OVERLAP_AB_FULL("Error: overlap of start or end cells is not allowed");

// special case
const string NO_SOLUTION("No path from start to end cells");


void print_error(string message, bool with_cell = false, unsigned i=0, unsigned j=0)
{
    cout << message;
    if (with_cell)
    {
        cout << " : ligne = " << i << " colonne = " << j;
    }

    cout << endl;

    exit(0);
}

void check_size(int nbL, int nbC) 
{
    if (nbL<3 || nbC < 3) print_error(TOO_SMALL);
} 

void check_location(Point p, int nbL, int nbC)
{
    if (p.x <= 0 || p.y <= 0) print_error(BAD_LOCATION, true, p.x, p.y);
    else if (p.x >= nbL-1 || p.y >= nbC-1) print_error(BAD_LOCATION, true, p.x, p.y);
}

void check_overlap(Point a, Point b)
{
    if (a.x == b.x && a.y == b.y) print_error(OVERLAP_AB);
}

void check_overlap_fab(Point a, Point b, Point temp, vector<vector<int>> laby)
{
    // Si le point a remplir est sur le point de départ ou d'arrivée
    if ((temp.x == a.x && temp.y == a.y) || (temp.x == b.x && temp.y == b.y))
    {
        print_error(OVERLAP_AB_FULL, true, temp.x, temp.y);
    }

    if ((laby[temp.x][temp.y] < 0)) print_error(OVERLAP_FULL, true, temp.x, temp.y);
    
}

void check_nopath(vector <Cell_vide> lvide, int i_start)
{
    if (lvide[i_start].distance == INF) print_error(NO_SOLUTION);
}
int find_cell(Cell_vide c, vector<vector<int>> laby, char car)
{
    switch (car)
                {
                    case 'L' : return laby[c.i][c.j-1];
                    break;
                    case 'R' : return laby[c.i][c.j+1];
                    break;
                    case 'U' : return laby[c.i-1][c.j];
                    break;
                    case 'D' : return laby[c.i+1][c.j];
                    break;
                }
                return 0;
}
vector<Path> rech_chemin(vector<vector<int>> laby, vector<Cell_vide> lvide, int i_start, int i_end)
{
    // Logiquement, la case suivante d'un chemin est forcément de distance = (distance case courante-1)
    // On utilise car pour voir combien et quels sont les chemins alternatifs sont disponibles pour ensuite les repartir
    string car;
    // Initialisation de paths avec un seul chemin 
    vector<Path> paths;
    Path p;
    p.i_cell=i_start;
    paths.push_back(p);

    // Initialisation de nb_chemin et d'une cellule vide C comme référence, et k comme indice pour la boucle
    int nb_chemin, k, n;
    Cell_vide c;

    do
    {
        nb_chemin = paths.size();
        for (k=0;k<nb_chemin;k++)
        {
            c = lvide[paths[k].i_cell];
            // Examination des 4 cellules voisines dans laby
            if (laby[c.i][c.j-1] >= 0 && lvide[laby[c.i][c.j-1]].distance  == c.distance-1) car += "L";
            if (laby[c.i][c.j+1] >= 0 && lvide[laby[c.i][c.j+1]].distance  == c.distance-1) car += "R";
            if (laby[c.i-1][c.j] >= 0 && lvide[laby[c.i-1][c.j]].distance  == c.distance-1) car += "U";
            if (laby[c.i+1][c.j] >= 0 && lvide[laby[c.i+1][c.j]].distance  == c.distance-1) car += "D";
            // Ajout sur le chemin traité et des potenntiels chemins alternatifs
            paths[k].i_cell = find_cell(c, laby, car[car.size() - 1]);
            paths[k].depl += car[car.size() - 1];
            car.pop_back();

            n=car.length();

            if (n != -1)
            {
                for (int j=0;j<n;j++)
                {
                    Path np;
                    np.i_cell = find_cell(c, laby, car[n - 1]);
                    np.depl = paths[k].depl.substr(0, paths[k].depl.size()-1) + car[car.size() - 1];
                    car.pop_back();
                    paths.push_back(np);
                }
            }
        }
        
    } while (paths[0].i_cell!=i_end);
    return paths;
}


void set_distances(vector<Cell_vide>& l_vide, int i_end, int num_rows, int num_columns, vector<vector<int>> laby) {
  // On crée une file des cellules qui ont besoin de définir leur distances
  queue<Cell_vide> q;

  // Définition de la distance du i_end a 0
  l_vide[i_end].distance = 0;

  // On ajoute la cellule de l_vide
  q.push(l_vide[i_end]);

  // On utilisera l'algorithme BFS pour définir les distances qu'entre les voisins
  while (!q.empty()) {
    Cell_vide cell = q.front();
    q.pop();
    // Définition de la distance aux voisins (si non définis)
    for (int di = -1; di <= 1; di++) {
      for (int dj = -1; dj <= 1; dj++) {
        // Passage des cellules non_voisines et de soi-même
        if (di == 0 && dj == 0 || di != 0 && dj != 0) continue;

        // Nécessaire pour le calcul de l'index du voisin
        int ni = cell.i + di;
        int nj = cell.j + dj;

        // Vérification de si le voisin est dans les bords du laby
        if (ni < 0 || ni >= num_rows || nj < 0 || nj >= num_columns) continue;

        
        if (laby[ni][nj] != FULL)
        {
            // Définition de la distance des voisins
            int index = laby[ni][nj];

            if (l_vide[index].distance == INF) {
            l_vide[index].distance = cell.distance + 1;
            q.push(l_vide[index]);
            }
        }
      }
    }
  }
}


// Fonction d'initialisation de lvide, ainsi que du i_start et du i_end
void init_lvide(vector<Cell_vide> &lvide, vector<vector<int>> &laby, int nbL, int nbC, Point a, Point b, int &i_start, int &i_end)
{
    for (int i=0;i<nbL;i++)
    {
        for (int j=0;j<nbC;j++)
        {
            
            // On ajoute une cell dans lvide pour chaque cellule non négative
            if (laby[i][j] >= 0)
            {
                Cell_vide c;
                c.i=i;
                c.j=j;
                c.distance=INF;
                lvide.push_back(c);
                laby[i][j] = lvide.size() - 1;
            }
            // Initialisation du i_start et i_end
            if (a.x == i && a.y == j) i_start = lvide.size() - 1;
            else if (b.x == i && b.y == j) i_end = lvide.size() - 1;
        }
    }
}
// Fonction d'ajout des bordures du laby
void init_laby(vector<vector<int>> &laby, int nbL, int nbC)
{
    int i;
    for (i=0;i<nbC;i++)
    {
        laby[0][i] = FULL;
        laby[nbL-1][i] = FULL;
    }

    for (i=0;i<nbL;i++)
    {
        laby[i][0] = FULL;
        laby[i][nbC-1] = FULL;
    }
}
// Fonction d'affichage du laby
void display_laby(vector<vector<int>> laby, int nbL, int nbC, Point a, Point b)
{
    for (int i=0;i<nbL;i++)
    {
        for (int j=0;j<nbC;j++)
        {
            if (laby[i][j] < 0) cout << "#";
            else if (a.x == i && a.y == j) cout << "A";
            else if (b.x == i && b.y == j) cout << "B";
            else cout << " ";
        }
        cout << endl;
    }
}
int partition(vector<string>& vec, int low, int high) {
  string pivot = vec[high];
  int i = low - 1;
  for (int j = low; j <= high - 1; j++) {
    if (vec[j] < pivot) {
      i++;
      swap(vec[i], vec[j]);
    }
  }
  swap(vec[i + 1], vec[high]);
  return i + 1;
}

// Fonction recursive qui utilisera le quicksort (la méthode sort n'étant pas autorisée)
void quicksort(vector<string>& vec, int low, int high) {
  if (low < high) {
    int pivotIndex = partition(vec, low, high);
    quicksort(vec, low, pivotIndex - 1);
    quicksort(vec, pivotIndex + 1, high);
  }
}
void displayPathsInAlphabeticalOrder(vector<Path> paths) {
  vector<string> npaths;
  int n=paths.size();
  int i;
  for (i=0;i<n;i++)
  {
    npaths.push_back(paths[i].depl);
  }
    // On stocke le tout dans un vecteur de string puis on applique le quicksort
    quicksort(npaths, 0, n-1);
    
    // Une fois le quicksort appliqué, on re-affiche le tout trié
    for (i=0;i<n;i++)
        {
            cout << npaths[i];
            cout << endl;
        }
}

// Affichage du laby avec les chemins
void display_laby_path(vector <Path> paths, vector<vector<int>> laby, int nbL, int nbC, Point a, Point b)
{
        Path p;
        int l, k, i, j;
        for (l=0;l<int(paths.size());l++)
        {
            i=a.x;j=a.y;
            p = paths[l];
            for (k=0;k<int(p.depl.size())-1;k++)
            {
                if (p.depl[k] == 'U') i--;
                else if (p.depl[k] == 'D') i++;
                else if (p.depl[k] == 'L') j--;
                else if (p.depl[k] == 'R') j++;
                laby[i][j] = EXPL;
                
            }
        }
        for (i=0;i<nbL;i++)
        {
            for (j=0;j<nbC;j++)
            {
                if (laby[i][j] == EXPL) cout << ".";
                else if (laby[i][j] == FULL) cout << "#";
                else if (a.x == i && a.y == j) cout << "A";
                else if (b.x == i && b.y == j) cout << "B";
                else cout << " ";
            }
            cout << endl;
        }
        cout << endl;
        displayPathsInAlphabeticalOrder(paths);
}
int main(int argc, char *argv[])
{
    vector<Cell_vide> lvide;
    // nbL = Nombre de lignes, nbC = Nombre de colonnes, nbV = Nombre de cases vides, nbP = Nombre de cases pleines, i_start = Position de départ, i_end = Position de fin
    int nbL, nbC, nbP, i_start, i_end;
    Point a,b,temp;

    // On prend en compte les arguments de la commande
    // La ligne et colonne, les coordonnées de a (départ) et b (arrivée), le nombre de cases pleines
    cin >> nbL >> nbC;
    check_size(nbL, nbC);
    cin >> a.x >> a.y;
    check_location(a, nbL, nbC);
    cin >> b.x >> b.y;
    check_location(b, nbL, nbC);
    check_overlap(a, b);
    cin >> nbP;

    vector<vector<int>> laby(nbL, vector<int>(nbC, 0));
    // On ajoute des bordures a notre laby
    init_laby(laby, nbL, nbC);

    // Puis une boucle ou on demandera la position de chaque case pleine
    for (int i=0;i<nbP;i++)
    {
        cin >> temp.x;
        cin >> temp.y; 
        check_location(temp, nbL, nbC);
        check_overlap_fab(a, b, temp, laby);
        laby[temp.x][temp.y] = FULL;
    }
    // On affiche notre laby vide
    display_laby(laby, nbL, nbC, a, b);
    cout << endl;
    // On initialise le vecteur lvide avec les positions des cases vides, comme distance inf
    init_lvide(lvide, laby, nbL, nbC, a, b, i_start, i_end);
    
    
    // On calcule la distance entre toutes les cellules vide et la cellule vide d'arrivée
    set_distances(lvide, i_end, nbL, nbC, laby);
    check_nopath(lvide, i_start);
    // On effectue la recherche de chemins
    vector<Path> paths=rech_chemin(laby, lvide, i_start, i_end);

    display_laby_path(paths, laby, nbL, nbC, a, b);
    return 0;
}
