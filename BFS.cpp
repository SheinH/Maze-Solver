#include <queue>
#include <vector>
#include <unordered_set>
#include <unordered_map>

//CSC 220: Homework Project 3
//Name: Shein Htike

struct Point
{
    int y, x;
    bool operator==(const Point &rhs) const
    {
        return y == rhs.y &&
               x == rhs.x;
    }
    bool operator!=(const Point &rhs) const
    {
        return !(rhs == *this);
    }
};

//Hasher
namespace std
{
    template<>
    struct hash<Point>
    {
        std::size_t operator()(const Point &k) const
        {
            hash<int> h;
            return h(k.x) ^ (h(k.y) << 1);
        }
    };
}

//Returns point from 2d array with pointer arithmatic
int &get_point(int n, int *maze, const Point &p)
{
    return maze[n * p.y + p.x];
}

//Checks if p is in bounds and is either open or the endpoint
bool is_valid_path(int n, int *maze, const Point &p)
{
    bool x_valid = p.x >= 0 && p.x < n;
    bool y_valid = p.y >= 0 && p.y < n;
    if (x_valid && y_valid)
    {
        const auto val = get_point(n, maze, p);
        return val == 3 || val == 1;
    } else
        return false;
}

//Returns all valid neighbors of p as a vector<Point>
std::vector<Point> get_neighbors(int n, int *maze, const Point &p)
{
    Point neighbors[4];
    for (auto &item : neighbors)
        item = p;
    neighbors[0].x++;
    neighbors[1].x--;
    neighbors[2].y++;
    neighbors[3].y--;
    std::vector<Point> valid_neighbors;
    for (const auto &neighbor : neighbors)
    {
        if (is_valid_path(n, maze, neighbor))
            valid_neighbors.push_back(neighbor);
    }
    return valid_neighbors;
}
//Main BFS algorithm

void BFS(int n, int *maze)
{
    //find start & end
    Point start, end;
    for (int y = 0; y < n; y++)
    {
        for (int x = 0; x < n; x++)
        {
            Point point{y, x};
            const int &val = get_point(n, maze, point);
            if (val == 2)
                start = point;
            else if (val == 3)
                end = point;
        }
    }
    //Begin BFS
    //unordered_map tracks parent nodes
    //unordered_set tracks visited nodes
    std::unordered_map<Point, Point> paths;
    std::queue<Point> queue;
    std::unordered_set<Point> visited;
    paths.reserve(n * n);
    visited.reserve(n * n);
    //Put Start into queue
    queue.push(start);
    visited.insert(start);
    while (!queue.empty())
    {
        Point parent = queue.front();
        if (parent == end)
            break;
        queue.pop();
        const auto neighbors = get_neighbors(n, maze, parent);
        for (const auto &neighbor : neighbors)
        {
            if (visited.find(neighbor) != visited.end())
                continue;
            int &neighbor_val = get_point(n, maze, neighbor);
            //Remember parent node
            paths.emplace(neighbor, parent);
            //Mark path as visited
            visited.insert(neighbor);
            queue.push(neighbor);
        }
    }
    //No possible path
    if (queue.empty())
        return;
    //Retrace steps
    Point path = end;
    while (path != start)
    {
        const auto iter = paths.find(path);
        Point parent = (*iter).second;
        int &parent_val = get_point(n, maze, parent);
        if (parent_val != 2)
            parent_val = 4;
        path = parent;
    }
}
