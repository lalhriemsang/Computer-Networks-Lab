#include <bits/stdc++.h>

using namespace std;

class Router
{
private:
    int router_id;

    vector<pair<Router *, int>> neighbours;

    unordered_map<int, pair<int, int>> routing_table;

    void ospf()
    {
        // cout << "hi\n";
        stack<Router *> nodes;
        priority_queue<pair<int, Router *>> pq;
        map<int, int> dist;
        unordered_map<int, Router *> parent;

        nodes.push(this);

        while (!nodes.empty())
        {
            int router = nodes.top()->router_id;
            Router *node = nodes.top();
            nodes.pop();
            // cout << "router:" << router << "\n";

            dist[router] = -10000000;
            if (router == router_id)
                dist[router] = 0;
            pq.push({dist[router], node});

            for (int i = 0; i < node->neighbours.size(); i++)
            {
                if (node->neighbours[i].first->router_id != router_id && dist.count(node->neighbours[i].first->router_id) == 0)
                {
                    nodes.push(node->neighbours[i].first);
                    dist[node->neighbours[i].first->router_id] = -10000000;
                }
            }
        }

        // cout << "hey\n";
        while (!pq.empty())
        {
            Router *u = pq.top().second;
            // cout << "router:" << u->router_id << " dist:" << dist[u->router_id] << " nghbr:" << u->neighbours.size() << "\n";
            pq.pop();

            for (int i = 0; i < u->neighbours.size(); i++)
            {

                // cout << "router:" << u->router_id << " neighbour:" << u->neighbours[i].first->router_id << "\n";

                if (-dist[u->neighbours[i].first->router_id] > -dist[u->router_id] + u->neighbours[i].second)
                {
                    dist[u->neighbours[i].first->router_id] = dist[u->router_id] - u->neighbours[i].second;
                    pq.push({dist[u->neighbours[i].first->router_id], u->neighbours[i].first});
                    parent[u->neighbours[i].first->router_id] = u;
                }
            }
        }

        for (auto dest = dist.begin(); dest != dist.end(); dest++)
        {
            int router = dest->first;

            if (router != router_id)
            {
                while (parent[router]->router_id != router_id)
                {
                    router = parent[router]->router_id;
                }
            }

            routing_table[dest->first] = {router, -dest->second};
        }

        // cout << "the end\n";
    }

public:
    Router(int id)
    {
        router_id = id;
    }
    void add_neighbour(Router *neighbour, int link_cost)
    {
        neighbours.push_back({neighbour, link_cost});
    }

    void update_routing_table()
    {
        routing_table.clear();
        ospf();
    }

    void print_routing_table()
    {
        update_routing_table();
        cout << "source router:" << router_id << endl;
        for (auto routers = routing_table.begin(); routers != routing_table.end(); routers++)
        {
            cout << "destination_router:" << routers->first << " next hop router:" << routers->second.first << " cost:" << routers->second.second;
            cout << endl;
        }
    }
};

int main()
{
    Router *router1 = new Router(111);
    Router *router2 = new Router(222);
    Router *router3 = new Router(333);
    Router *router4 = new Router(444);
    Router *router5 = new Router(123);
    Router *router6 = new Router(456);

    (*router1).add_neighbour(router2, 2);
    (*router2).add_neighbour(router1, 2);
    (*router1).add_neighbour(router3, 1);
    (*router3).add_neighbour(router1, 1);
    (*router2).add_neighbour(router4, 1);
    (*router4).add_neighbour(router2, 1);
    (*router2).add_neighbour(router5, 1);
    (*router5).add_neighbour(router2, 1);
    (*router3).add_neighbour(router5, 3);
    (*router5).add_neighbour(router3, 3);
    (*router5).add_neighbour(router6, 1);
    (*router6).add_neighbour(router5, 1);
    (*router4).add_neighbour(router6, 2);
    (*router6).add_neighbour(router4, 2);

    (*router1).print_routing_table();
    (*router2).print_routing_table();
    (*router3).print_routing_table();
    (*router4).print_routing_table();
    (*router5).print_routing_table();
    (*router6).print_routing_table();

    delete router1;
    delete router2;
    delete router3;
    delete router4;
    delete router5;
    delete router6;
}