
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
                // Component is non-bipartite. In a non-bipartite connected component,
                // a vertex can be in an odd cycle if and only if it's not a cut vertex
                // that separates the component into bipartite pieces.
                
                // Simple approach: mark all vertices as potentially in odd cycles
                // then remove those that clearly cannot be
                for (int v : component_vertices) {
                    can_be_in_odd_cycle[v] = true;
                }
                
                // Find vertices that are leaves (degree 1) - they cannot be in odd cycles
                for (int v : component_vertices) {
                    if (adj[v].size() == 1) {
                        can_be_in_odd_cycle[v] = false;
                    }
                }
                
                // For each same-color edge, mark all vertices on paths between them
                for (auto [u, v] : same_color_edges) {
                    // BFS from u to find shortest path to v
                    vector<int> parent(n + 1, -1);
                    vector<bool> bfs_visited(n + 1, false);
                    queue<int> bfs_q;
                    
                    bfs_q.push(u);
                    bfs_visited[u] = true;
                    
                    while (!bfs_q.empty() && !bfs_visited[v]) {
                        int curr = bfs_q.front();
                        bfs_q.pop();
                        
                        for (int next : adj[curr]) {
                            if (!bfs_visited[next]) {
                                bfs_visited[next] = true;
                                parent[next] = curr;
                                bfs_q.push(next);
                            }
                        }
                    }
                    
                    // Reconstruct path from u to v
                    if (bfs_visited[v]) {
                        vector<int> path;
                        int curr = v;
                        while (curr != -1) {
                            path.push_back(curr);
                            curr = parent[curr];
                        }
                        
                        // All vertices on this path can definitely be in odd cycles
                        for (int vertex : path) {
                            can_be_in_odd_cycle[vertex] = true;
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
