#include <stdio.h>
#include <stdlib.h>
#include <vector>

typedef std::vector< std::vector<int> > Matrix;
typedef std::vector< int > Vector;


void print_matrix(Matrix mat){
   int n = (int) mat.size();
   for ( int i = 0; i < n; ++i ) {
	  int m = (int) mat[i].size();
      for ( int j = 0; j < m; ++j ) {
         printf("%d ", mat[i][j]);
      }
      puts("");
   }
}

void print_vector(Vector v){
   for( Vector::iterator it = v.begin(); it!=v.end(); ++it ) {
      printf("%d ", *it);
   }
}

Matrix init_matrix(int n){
   Matrix mat;
   mat.resize(n);
   for ( int i = 0; i < n; ++i ) {
      mat[i].resize(n);
      for ( int j = 0; j < n; ++j ) {
         mat[i][j] = 0;
      }  
   }
   return mat;
}

Matrix transpose_matrix(Matrix mat){
   Matrix res;
   int n = (int) mat[0].size();
   res.resize(n);
   for ( int i = 0; i < n; ++i ) {
	  int m = (int) mat.size();
	  res[i].resize(m);
      for ( int j = 0; j < m; ++j ) {
         res[i][j] = mat[j][i];
      }  
   }
   return res;
}


Matrix read_graph(char* filename)
{
   FILE *file;
   Matrix graph;
   
   file = fopen(filename, "r");
   if (file) {
      int n, i, j;
      fscanf(file, "%d", &n);
      graph = init_matrix(n);
      
      while ( fscanf(file, "%d", &i) != EOF ) {
         fscanf(file, "%d", &j);
         graph[i-1][j-1]=1;
      }
      fclose(file);
   }
   else {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }
   return graph;
}

Vector get_neighbours_of(int node, Matrix graph) {
	Vector neigh;
	int n = (int) graph.size();
	for ( int i = 0; i < n; ++i ) {
		if (graph[node][i] == 1) {
			neigh.push_back(i);
		}
	}
	return neigh;
}

Vector dfs(Matrix graph, int root, Vector exclude = Vector() )
{
   Vector res, S, discovered, neighbours, open;
   int n, node;
   
   n = (int) graph.size();
   
   discovered.resize(n);
   open.resize(n);
   for ( int i = 0; i < n; ++i ) {
	   discovered[i] = 0;
	   open[i] = 0;
   }
   
   Vector::iterator it;
   for( it = exclude.begin(); it != exclude.end(); ++it ) {
	   discovered[*it] = 1;
   }   
   
   discovered[root]=1;
   S.push_back(root);
   while ( !S.empty() ) {
      node = S.back();
      printf("\nnode %d\n", node);

      if ( open[node] == 0 ) {
		  neighbours = get_neighbours_of(node, graph);
		  print_vector(neighbours);
		  Vector::iterator it;
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

Matrix connectedComponents(Matrix graph)
{
	Vector S, s_temp, discovered, assigned, exclude;
	Matrix graph_T;
	Matrix res;
	int n, node; 
	n = (int) graph.size();
	
	discovered.resize(n);
	for ( int i = 0; i < n; ++i ) {
		discovered[i] = 0;
	}
	
	int k =0;
	while ( (int) S.size() < n) {
		for ( k = k; k < n; ++k ) {
			if ( discovered[k] == 0 ){
				node = k;
				break;
			}
		}		
		s_temp = dfs(graph, node);

		Vector::iterator it;
		for( it = s_temp.begin(); it != s_temp.end(); ++it ) {
			S.push_back(*it);
			discovered[*it] = 1;
		}
		print_vector(S);
	}
	
	graph_T = transpose_matrix(graph);
	
	assigned.resize(n);
	for ( int i = 0; i < n; ++i ) {
		assigned[i] = 0;
	}
	exclude = Vector();
	while ( !S.empty() ) {
		node = S.back();
		S.pop_back();
		if ( assigned[node] == 0 ) {
			s_temp = dfs(graph_T, node, exclude);
			Vector::iterator it;
			for( it = s_temp.begin(); it != s_temp.end(); ++it ) {
				assigned[*it] = 1;
				exclude.push_back(*it);
				
			}
			res.push_back(s_temp);
		}
	}
	
	return res;
}


int main( int argc, char *argv[])
{
   char* filename = argv[1];
   Matrix graph, cc;
   graph = read_graph(filename);
   cc = connectedComponents(graph);
   print_matrix(cc);
   return 0;
}
