
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
                // Component is non-bipartite. Use a more efficient approach:
                // In a non-bipartite connected component, all vertices except
                // those that are in bipartite blocks separated by odd cycle vertices
                // can be part of odd cycles.
                
                // Initially mark all vertices as potentially in odd cycles
                for (int v : component_vertices) {
                    can_be_in_odd_cycle[v] = true;
                }
                
                // Find the block-cut tree structure
                // For simplicity, use the fact that in a non-bipartite component,
                // vertices that are leaves or in bipartite "dangling" parts
                // cannot be in odd cycles
                
                // Find vertices that are definitely in odd cycles
                // (those on same-color edges and their neighbors)
                vector<bool> in_odd_cycle_core(n + 1, false);
                for (auto [u, v] : same_color_edges) {
                    in_odd_cycle_core[u] = true;
                    in_odd_cycle_core[v] = true;
                    for (int neighbor : adj[u]) {
                        in_odd_cycle_core[neighbor] = true;
                    }
                    for (int neighbor : adj[v]) {
                        in_odd_cycle_core[neighbor] = true;
                    }
                }
                
                // Mark vertices not in the core as potentially not in odd cycles
                for (int v : component_vertices) {
                    if (!in_odd_cycle_core[v]) {
                        // Check if this vertex can reach the core via a path
                        // that doesn't create odd cycles
                        vector<int> dist(n + 1, -1);
                        queue<int> bfs_q;
                        bfs_q.push(v);
                        dist[v] = 0;
                        bool can_reach_core = false;
                        
                        while (!bfs_q.empty() && !can_reach_core) {
                            int curr = bfs_q.front();
                            bfs_q.pop();
                            
                            if (in_odd_cycle_core[curr]) {
                                can_reach_core = true;
                                break;
                            }
                            
                            for (int next : adj[curr]) {
                                if (dist[next] == -1) {
                                    dist[next] = dist[curr] + 1;
                                    bfs_q.push(next);
                                }
                            }
                        }
                        
                        if (!can_reach_core) {
                            can_be_in_odd_cycle[v] = false;
                        }
                    }
                }
                
                // Additional check: vertices with degree 1 cannot be in odd cycles
                for (int v : component_vertices) {
                    if (adj[v].size() == 1) {
                        can_be_in_odd_cycle[v] = false;
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
