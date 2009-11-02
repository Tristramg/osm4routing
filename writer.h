#include "main.h"
#ifndef _WRITER_H
#define _WRITER_H

class Writer
{
    public:
    virtual int save_nodes(const NodeMapType & nodes) = 0;

    virtual void save_edge(int edge_id,
            node_t source, node_t target, float length,
            char car, char car_d,
            char bike, char bike_d,
            char foot,
            std::string geom) = 0;

    virtual ~Writer() {};

};

#endif
