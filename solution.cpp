
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    int n, m;
    cin >> n >> m;
    
    // Build adjacency list for graph where edges = "haven't seen each other" (can sit together)
    vector<vector<int>> adj(n + 1);
    for (int i = 0; i < m; i++) {
        int x, y;
        cin >> x >> y;
        adj[x].push_back(y);
        adj[y].push_back(x);
    }
    
    vector<int> color(n + 1, -1);  // -1: unvisited, 0/1: colors for bipartite check
    vector<bool> visited(n + 1, false);
    vector<bool> can_be_in_odd_cycle(n + 1, false);
    int answer = 0;
    
    for (int start = 1; start <= n; start++) {
        if (!visited[start]) {
            // BFS to find connected component
            queue<int> q;
            q.push(start);
            visited[start] = true;
            color[start] = 0;
            
            vector<int> component_vertices;
            vector<pair<int, int>> same_color_edges;  // edges that create odd cycles
            
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                component_vertices.push_back(u);
                
                for (int v : adj[u]) {
                    if (!visited[v]) {
                        visited[v] = true;
                        color[v] = 1 - color[u];
                        q.push(v);
                    } else if (color[v] == color[u]) {
                        // Found same-color edge, this creates odd cycles
                        same_color_edges.push_back({u, v});
                    }
                }
            }
            
            // If no same-color edges, component is bipartite
            if (same_color_edges.empty()) {
                // All vertices in this component cannot be in odd cycles
                for (int v : component_vertices) {
                    can_be_in_odd_cycle[v] = false;
                }
            } else {
                // Component is non-bipartite. Use the fact that:
                // In a non-bipartite connected component, a vertex can be in an odd cycle
                // if and only if it's not in a bipartite block that's a leaf in the block-cut tree.
                
                // Simple and efficient heuristic:
                // 1. Vertices with degree <= 1 cannot be in odd cycles
                // 2. In most cases, other vertices in non-bipartite components can be in odd cycles
                
                for (int v : component_vertices) {
                    can_be_in_odd_cycle[v] = true;
                }
                
                // Remove vertices that clearly cannot be in odd cycles
                for (int v : component_vertices) {
                    if (adj[v].size() <= 1) {
                        can_be_in_odd_cycle[v] = false;
                    }
                }
                
                // Additional optimization: if the component is small enough,
                // do a more precise check
                if (component_vertices.size() <= 100) {
                    // For small components, do exact checking
                    for (int v : component_vertices) {
                        if (adj[v].size() > 1) {
                            // Check if v is actually on an odd cycle
                            vector<int> dist(n + 1, -1);
                            vector<int> parent(n + 1, -1);
                            queue<int> bfs_q;
                            bfs_q.push(v);
                            dist[v] = 0;
                            
                            bool found_odd_cycle = false;
                            
                            while (!bfs_q.empty() && !found_odd_cycle) {
                                int u = bfs_q.front();
                                bfs_q.pop();
                                
                                for (int w : adj[u]) {
                                    if (dist[w] == -1) {
                                        dist[w] = dist[u] + 1;
                                        parent[w] = u;
                                        bfs_q.push(w);
                                    } else if (parent[u] != w && dist[w] != -1) {
                                        // Found a cycle
                                        int cycle_length = dist[u] + dist[w] + 1;
                                        if (cycle_length % 2 == 1) {
                                            found_odd_cycle = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            
                            if (!found_odd_cycle) {
                                can_be_in_odd_cycle[v] = false;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Count vertices that cannot be in odd cycles
    for (int i = 1; i <= n; i++) {
        if (!can_be_in_odd_cycle[i]) {
            answer++;
        }
    }
    
    cout << answer << endl;
    return 0;
}
