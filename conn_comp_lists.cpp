#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <sys/timeb.h>

typedef std::vector<int> Vector;
typedef std::map< int, Vector > Map;

void print_cc(std::vector< Vector > mat)
{
  int i,j,n, m;

  n = (int) mat.size();
  for ( i = 0; i < n; ++i ) {
    m = (int) mat[i].size();
    for ( j = 0; j < m; ++j ) {
      printf("%d ", mat[i][j]+1);
    }
    puts("");
  }
}

void print_vector(Vector v)
{
  Vector::iterator it;

  for( it = v.begin(); it!=v.end(); ++it ) {
    printf("%d ", *it);
  }
}

void print_map(Map m)
{
  Vector v;
  Map::iterator it;
  Vector::iterator itv;

  for( it = m.begin(); it != m.end(); ++it ) {
    printf("%d:", (*it).first);
    v = (*it).second;
    for( itv = v.begin(); itv != v.end(); ++itv ) {
      printf(" %d", (*itv));
    }
    puts("");
  }
}

Map read_graph(char* filename)
{
  FILE *file;
  Map graph;
  int n, i, j, k;

  file = fopen(filename, "r");
  if (file) {
    fscanf(file, "%d", &n);

    for( k = 0; k < n; ++k ) {
      graph.insert(std::pair<int, Vector>(k, Vector()));
    }
    while ( fscanf(file, "%d", &i) != EOF ) {
      fscanf(file, "%d", &j);
      graph[i-1].push_back(j-1);
    }
    fclose(file);
  }
  else {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }
  return graph;
}

Map invert(Map m)
{
  Map res;
  Vector v;
  Map::iterator it;
  Vector::iterator itv;
  int k, n, i;

  n = (int) m.size();
  for( i = 0; i < n; ++i ) {
    res.insert(std::pair<int, Vector>(i, Vector()));
  }
  for( it = m.begin(); it != m.end(); ++it ) {
    k = (*it).first;
    v = (*it).second;
    for( itv = v.begin(); itv != v.end(); ++itv ) {
      res[*itv].push_back(k);
    }
  }
  return res;
}

Vector dfs(Map graph, int root, Vector exclude = Vector())
{
  Vector res, S, discovered, neighbours, open;
  int n, node, i;
  Vector::iterator it;

  n = (int) graph.size();
  discovered.resize(n);
  open.resize(n);
  for ( i = 0; i < n; ++i ) {
    discovered[i] = 0;
    open[i] = 0;
  }
  for( it = exclude.begin(); it != exclude.end(); ++it ) {
    discovered[*it] = 1;
  }
  discovered[root]=1;
  S.push_back(root);
  while ( !S.empty() ) {
    node = S.back();
    if ( open[node] == 0 ) {
      neighbours = graph[node];
      for( it = neighbours.begin(); it != neighbours.end(); ++it ) {
        if ( discovered[*it] != 1 ) {
          discovered[*it] = 1;
          S.push_back(*it);
        }
      }
      open[node] = 1;
    }
    else {
      S.pop_back();
      res.push_back(node);
    }
  }
  return res;
}

std::vector< Vector > connectedComponents(Map graph)
{
  Vector S, s_temp, discovered, assigned, exclude;
  Map graph_T;
  std::vector< Vector > res;
  int n, node, k, i;
  Vector::iterator it;

  n = (int) graph.size();
  discovered.resize(n);
  for ( i = 0; i < n; ++i ) {
    discovered[i] = 0;
  }

  k = 0;

  while ( (int) S.size() < n ) {
    for ( k = k; k < n; ++k ) {
      if ( discovered[k] == 0 ){
        node = k;
        break;
      }
    }
    s_temp = dfs(graph, node);
    for( it = s_temp.begin(); it != s_temp.end(); ++it ) {
      S.push_back(*it);
      discovered[*it] = 1;
    }
  }


  graph_T = invert(graph);

  assigned.resize(n);
  for ( i = 0; i < n; ++i ) {
    assigned[i] = 0;
  }
  exclude = Vector();
  while ( !S.empty() ) {
    node = S.back();
    S.pop_back();
    if ( assigned[node] == 0 ) {
      s_temp = dfs(graph_T, node, exclude);
      for( it = s_temp.begin(); it != s_temp.end(); ++it ) {
        assigned[*it] = 1;
        exclude.push_back(*it);
      }
      res.push_back(s_temp);
    }
  }
  return res;
}

Map randomDigraph(int n, double p)
{
   double v;
   int i, j;
   Map graph;

   for ( i = 0; i < n; i++ ) {
     graph.insert( std::pair<int, Vector>(i, Vector()) );
     for ( j = 0; j < n; j++ ) {
       if ( j != i ) {
         v = random() / (double) RAND_MAX;
         if ( v <= p ) {
           graph[i].push_back(j);
         }
       }
     }
   }
   return graph;
}

int count_edges(Map graph)
{
  int n = 0;
  Map::iterator it;

  for( it = graph.begin(); it != graph.end(); ++it ) {
    n = n+ (int) (*it).second.size();
  }
  return n;
}

void time_conn_comp(float p)
{
  std::vector< std::vector<int> > cc;
  Map graph;
  int n, A, t;
  struct timeb tmb1, tmb2;

  puts("n A ms ms/(V+A)");
  for ( n=128; n<=5000; n = n*2 ) {
    graph = randomDigraph(n, p);
    A = count_edges(graph);
    ftime(&tmb1);
    cc = connectedComponents(graph);
    ftime(&tmb2);
    t = (tmb2.time-tmb1.time)*1000+(tmb2.millitm-tmb1.millitm);
    printf("%d %d %d %f\n", n, A , t, float(t)/(n+A));
  }
}

void n_cc_vs_n_p()
{
  std::vector< Vector > cc;
  Map graph;
  float p;
  int n, A, n_cc, i;

  n = 100;
  puts("n A p n_cc");
  for ( i = 0; i<=20; i=i+1) {
    p = float(i)/n;
    graph = randomDigraph(n, p);
    A = count_edges(graph);
    cc = connectedComponents(graph);
    n_cc = (int) cc.size();
    printf("%d %d %f %d \n", n, A, p, n_cc);
  }
}

int main(int argc, char *argv[])
{
  if ( argc > 1 ) {
    std::vector< Vector > cc;
    Map graph;
    char* filename;

    filename = argv[1];
    graph = read_graph(filename);
    cc = connectedComponents(graph);
    print_cc(cc);
  }
  else {
    //time_conn_comp(0.1);
    n_cc_vs_n_p();
  }
  return 0;
}
